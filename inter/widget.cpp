#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent), time(0, 0, 0), timer(new QTimer),
      view(new View(&scene)),timeLbl(new QLabel("Время: ")), editTime(new QLineEdit), lblIPos(new QLabel),
      layout(new QBoxLayout(QBoxLayout::TopToBottom))
{
    this->setWindowTitle("Настройка курсов объектов");
    /// создадим два диалоговых окна
    dialog[0] = new SelectDialog("1 объект", 1);
    dialog[0]->setDisabled(false);
    dialog[1] = new SelectDialog("2 объект", 2);
    dialog[1]->setDisabled(false);

    /// сглаживание
    view->setRenderHint(QPainter::Antialiasing, true);
    scene.setSceneRect(view->viewport()->rect());
    layout->setContentsMargins(0, 50, 0, 0);
   // layout->addWidget(view); /// добавить поле рисования в виджет
    view->setMinimumSize(QSize(640, 480));
    view->setWindowTitle("Отображение курсов");
    view->show();
    /// дополнительный менеджер компоновки
    QHBoxLayout *h1layout = new QHBoxLayout;

    save = new QPushButton("Сохранить в БД");
    restore = new QPushButton("Получить данные из БД");
    editTimeBtn = new QPushButton("Изменить время");

    h1layout->addWidget(timeLbl); /// метка для таймера
    h1layout->addWidget(editTime); /// изменение таймера
    h1layout->addWidget(editTimeBtn); /// кнопка для изменения таймера
    h1layout->addStretch(1);
    h1layout->addWidget(lblIPos); /// метка позиции пересечения
    h1layout->addWidget(save); /// кнопка сохранить в БД
    h1layout->addWidget(restore); /// кнопка получить данные из БД

    layout->addLayout(h1layout); /// установить слой

    /// добавим диалоговые окна на виджет
    layout->addWidget(dialog[0]);
    layout->addWidget(dialog[1]);

    /// переключатель старт/стоп
    stop = new QPushButton("Старт");
    stop->setCheckable(true);
    stop->setChecked(false);

    /// кнопка сброса
    rt = new QPushButton("Сброс");

    /// осоновной менеджер компоновки
    QHBoxLayout *h2layout = new QHBoxLayout;
    h2layout->addWidget(stop);
    h2layout->addWidget(rt);
    layout->addLayout(h2layout);

    /// установить
    setLayout(layout);


    menuBar = new QMenuBar(this);
    QMenu *pmnu = new QMenu("&DB");
    pmnu->addAction("Установить БД", this, SLOT(slotDataBaseDialog()), Qt::CTRL | Qt::Key_D);
    menuBar->addMenu(pmnu);

    /// установка соединений сигналов и слотов
    connect(editTimeBtn, SIGNAL(clicked()),SLOT(addTime()));
    connect(save, SIGNAL(clicked()), SLOT(dataSave()));
    connect(restore, SIGNAL(clicked()), SLOT(dataRestore()));
    connect(stop, SIGNAL(clicked()),SLOT(state()));
    connect(timer, SIGNAL(timeout()), SLOT(step()));
    connect(stop, SIGNAL(clicked()),SLOT(state()));
    connect(rt, SIGNAL(clicked()), SLOT(reset()));

    if(!Widget::dbCheck) {
        save->setDisabled(true);
        restore->setDisabled(true);
    }

    setObject(new ObjectItem{1, QPointF(),
                             QPointF(), std::optional<QPointF>{}, std::optional<qreal>{}, 0, 0, std::optional<QTime>{}});
    setObject(new ObjectItem{2, QPointF(),
                             QPointF(), std::optional<QPointF>{}, std::optional<qreal>{}, 0, 0, std::optional<QTime>{}});
}

bool Widget::dbCheck = false;

void Widget::slotDataBaseDialog()
{
    DBDialog* pdbDialog = new DBDialog(this);
    if(pdbDialog->exec() == DBDialog::Accepted) {
        if(Widget::createConnection(pdbDialog->getDBName(), pdbDialog->getUserName(),
                                  pdbDialog->getPassword(), pdbDialog->getHostname(), pdbDialog->getPort().size() ? pdbDialog->getPort() : "5432")) {
            dbCheck = true;
            save->setDisabled(false);
            restore->setDisabled(false);
        } else {
            dbCheck = false;
            save->setDisabled(true);
            restore->setDisabled(true);
        }
    }
}


void Widget::addTime()
{
    time.fromString("0:0:0");
    QTime tmp;

    if(QTime().fromString(editTime->text(), "hh:mm:ss").isValid()) {
        tmp = QTime().fromString(editTime->text(), "hh:mm:ss");
    } else if(QTime().fromString(editTime->text(), "h:m:s").isValid()) {
        tmp = QTime().fromString(editTime->text(), "h:m:s");
    } else if(QTime().fromString(editTime->text(), "s").isValid()) {
        tmp = QTime().fromString(editTime->text(), "s");
    } else {
        QMessageBox msgBox;
        msgBox.setText(QString("Не верный формат времени: h:m:s|hh:mm:ss|s"));
        msgBox.exec();
        return;
    }
    reset(); /// сбросить значения
    uint mx = tmp.hour() * 3600 + tmp.minute() * 60 + tmp.second();
    for(uint i = 1;i <= mx;i++) {
        points[0]->step(); /// приращение 1 траектории
        points[1]->step(); /// приращение 2 траектории
        checkInter();
        time = time.addSecs(1);
    }
    editTime->setText(time.toString("hh:mm:ss"));
}


/// получить объект по Id
ObjectItem* Widget::getObject(quint8 number) {
    if(points.size() <= number)
        throw std::out_of_range(QString("number <= %1").arg(points.size()).toStdString());
    return points[number];
}

/// слот смены состояния переключателя
void Widget::state()
{

    if(stop->isChecked()) {
        stop->setText("Стоп");
        timer->start(1000);
        dialog[0]->setDisabled(true);
        dialog[1]->setDisabled(true);
        editTime->setDisabled(true);
        editTimeBtn->setDisabled(true);
        if(Widget::dbCheck) {
            save->setDisabled(true);
            restore->setDisabled(true);
        }
    } else {
        stop->setText("Старт");
        timer->stop();
        dialog[0]->setDisabled(false);
        dialog[1]->setDisabled(false);
        editTime->setDisabled(false);
        editTimeBtn->setDisabled(false);
        if(Widget::dbCheck) {
            save->setDisabled(false);
            restore->setDisabled(false);
        }
    }
}

/// слот сброса
void Widget::reset()
{
    /// сбросить курсы объектов
    for(ObjectItem* val : points) {
        val->setEndPoint(val->getStartPoint());
        val->resetIPos();
        val->resetDist();
        val->resetTime();
    }
    /// сброс окружения
    resetView();
}

void Widget::resetView()
{
    time = time.fromString("0:0:0", "h:m:s");
    editTime->setText("");
    lblIPos->setText("");
    stop->setChecked(false);
    inter = false;
    state();
}

/// установка объектов на сцену
void Widget::setObject(ObjectItem *data)
{
    /// добавим данные на сцену и сразу в память для удобства
    points.push_back(data); /// сохраняем для удобства в вектор
    data->setPos(scene.sceneRect().center());
    scene.addItem(data);/// добавляем на сцену
}

/* Шаг каждую секунду */
void Widget::step()
{
    points[0]->step(); /// прирощение 1 траектории
    points[1]->step(); /// прирощение 2 траектории
    checkInter();
    time = time.addSecs(1); /// приращение таймера
    editTime->setText(time.toString("hh:mm:ss")); /// обновить метку
}

void Widget::checkInter()
{
    if(!inter && points[0]->checkIntersection(points[1])) {
        lblIPos->setText(QString("Точка пересечения: ") + "X (м): " +
                         QString::number(points[0]->getInterPoint().x()) + " Y (м): " +
                         QString::number(points[0]->getInterPoint().y()));
        for(int i = 0;i < 2;i++) {
            points[i]->distance();
            points[i]->setTime(time);
        }
        inter = true;
    } else if(inter) {
        for(int i = 0;i < 2;i++) {
            /// вычисление дистанции
            points[i]->distance();
        }
    }
}

/// создание диалогового окна
void SelectDialog::slotButtonClicked()
{
    /// создать диалог
    DialogPoint* pdialog = new DialogPoint(number, this);
    Widget *parent = qobject_cast<Widget*>(this->parent());

    /// получить индекс объекта
    ObjectItem* val = parent->getObject(number);
    pdialog->setData(val); /// установить данные в диалог

    if(pdialog->exec() == QDialog::Accepted) { /// установить данные из диалога
        val->setStartPoint(QPointF(pdialog->xStart->text().toDouble(), pdialog->yStart->text().toDouble()));
        /// сброс
        val->setEndPoint(QPointF(pdialog->xStart->text().toDouble(), pdialog->yStart->text().toDouble()));
        val->resetIPos();
        val->resetDist();
        /// установка значений
        val->setAngle(pdialog->lineK->text().toDouble()); /// % 360
        val->setVelocity(pdialog->lineV->text().toDouble());
        parent->resetView();
    }
}

/// соединение с БД
bool Widget::createConnection(QString dbname, QString user, QString pass, QString host, QString port)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(pass);
    db.setHostName(host);
    db.setPort(port.toInt());
    if(!db.open()) {
        qDebug() << QString("Ошибка подключения к %1").arg(dbname) << db.lastError();
        Widget::dbCheck = false;
        QMessageBox msgBox;
        msgBox.setText(QString("Ошибка подключения к БД: ") + db.lastError().text());
        msgBox.exec();
        return false;
    }
    /// проверка на существование таблицы
    if(db.tables().filter("dot", Qt::CaseSensitivity::CaseInsensitive).empty()) {
        qDebug() << "Нет таблицы dot в БД";
    }
    return true;
}

/// получить данные из БД
bool Widget::dataRestore()
{
    for(ObjectItem *val : points) {
        QSqlQuery query;
        if(!query.exec(QString("SELECT * FROM dot WHERE \"Id\"=%1").arg(val->getId()))) { /// получим все данные
            qDebug() << "Не удалось получить данные" << query.lastError();
            return false;
        }
        QSqlRecord rec = query.record();
        qreal x1, y1, k, v;
        qreal x2, y2, x3, y3;
        qreal dist;
        /// извлечение данных
        while(query.next()) {
            x1 = query.value(rec.indexOf("x1")).toReal(); /// начальная позиция x
            y1 = query.value(rec.indexOf("y1")).toReal(); /// начальная позиция y
            k = query.value(rec.indexOf("k")).toReal(); /// угловой коэффициент
            v = query.value(rec.indexOf("v")).toReal(); /// скорость
            x2 = query.value(rec.indexOf("x2")).toReal(); /// конечная позиция x
            y2 = query.value(rec.indexOf("y2")).toReal(); /// конечная позиция y
            if(!query.value(rec.indexOf("x3")).isNull()) {
                x3 = query.value(rec.indexOf("x3")).toReal(); /// x точки пересечения
                y3 = query.value(rec.indexOf("y3")).toReal(); /// y точки пересечения
                val->setInterPos(QPointF(x3, -y3));
                setInter(true);
                lblIPos->setText(QString("Точка пересечения: ") + "X: " +
                                 QString::number(val->getInterPoint().x()) + " Y: " + QString::number(val->getInterPoint().y()));
            }
            if(!query.value(rec.indexOf("Dist")).isNull()) {
                dist = query.value(rec.indexOf("Dist")).toReal(); /// растояние
                val->setDist(dist);
            }
            if(!query.value(rec.indexOf("Time")).isNull()) {
                val->setTime(query.value(rec.indexOf("Time")).toTime());
            }

            if(!query.value(rec.indexOf("Counter")).isNull()) {
                time = query.value(rec.indexOf("Counter")).toTime();
                editTime->setText(time.toString("hh:mm:ss"));
            }

            val->setStartPoint(QPointF(x1, y1));
            val->setEndPoint(QPointF(x2, y2));
            val->setAngle(k);
            val->setVelocity(v);
        }
    }
    checkInter();
    return true;
}

/// сохранить данные в БД
bool Widget::dataSave()
{
    for(ObjectItem *val : points) {
        QSqlQuery query;
        QString dot = "UPDATE dot SET \"X1\"=%1,\"Y1\"=%2,\"K\"=%3,"
                      "\"V\"=%4,\"X2\"=%5,\"Y2\"=%6,\"X3\"=%7,"
                      "\"Y3\"=%8,\"Dist\"=%9,\"Time\"=%10, \"Counter\"=%11 WHERE \"Id\"=%12";
        QString queryStr = dot.arg(val->getStartPoint().x()) /// X1
                           .arg(val->getStartPoint().y()) /// Y1
                           .arg(val->getAngle()) /// K
                           .arg(val->getVelocity()) /// V
                           .arg(val->getEndPoint().x()) /// X2
                           .arg(val->getEndPoint().y()) /// Y2
                           .arg(val->getIPos().has_value() ? QString::number(val->getInterPoint().x()) : "null") /// X3
                           .arg(val->getIPos().has_value() ? QString::number(val->getInterPoint().y()) : "null") /// Y3
                           .arg(val->getDistance().has_value() ? QString::number(val->getDist()) : "null") /// Dist
                           .arg(val->getTime().has_value() ? ("'" + val->getTime().value().toString("h:m:s") + "'") : "null") /// Time
                           .arg(time.isValid() ? ("'" + time.toString("h:m:s") + "'") : "null") /// Counter
                           .arg(val->getId()); /// Id
        if(!query.exec(queryStr)) {
            qDebug() << "Не удалось обновить данные" << query.lastError();
            return false;
        }
    }
    return true;
}

Widget::~Widget()
{
    /// освободить ресурсы таймера
    delete timer;
}

