#include "dialog.h"

DialogPoint::DialogPoint(quint8 number, QWidget *parent)
    : QDialog{parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint},
      number(number), xLblStart(new QLabel("X1: ")), yLblStart(new QLabel("Y1: ")), kLbl(new QLabel("K: ")),
      vLbl(new QLabel("V: ")), dLbl(new QLabel("D: -")), tLbl(new QLabel("T: -"))
{
    setWindowTitle(QString("Параметры %1 объекта").arg(number + 1));

    kLbl->setToolTip("Угол наклона в градусах");
    vLbl->setToolTip("Скорость движения в м/с");

    xStart = new QLineEdit;
    xLblStart->setToolTip("Начальная координата объекта по оси X (м)");
    yStart = new QLineEdit;
    yLblStart->setToolTip("Начальная координата объекта по оси Y (м)");

    xEnd = new QLabel("X2 (м): ");
    xEnd->setToolTip("Текущая координата объекта по оси X (м)");
    yEnd = new QLabel("Y2 (м): ");
    yEnd->setToolTip("Текущая координата объекта по оси Y (м)");

    lineK = new QLineEdit;
    lineV = new QLineEdit;

    dLbl->setToolTip("Расстояние от точки пересечения до объекта");
    tLbl->setToolTip("Время персечения объектов");

    QLabel *name = new QLabel(QString::number(number + 1) + " объект");

    QPushButton* pOk = new QPushButton("&Принять изменения");
    QPushButton* pCan = new QPushButton("&Отмена");

    QGridLayout *pLayout = new QGridLayout;

    pLayout->addWidget(name, 0, 0, 1, 2);

    pLayout->addWidget(xLblStart, 1, 0);
    pLayout->addWidget(xStart, 1, 1);
    pLayout->addWidget(yLblStart, 2, 0);
    pLayout->addWidget(yStart, 2, 1);

    pLayout->addWidget(xEnd, 3, 0);
    pLayout->addWidget(yEnd, 3, 1);

    pLayout->addWidget(kLbl, 4, 0);
    pLayout->addWidget(lineK, 4, 1);

    pLayout->addWidget(vLbl, 5, 0);
    pLayout->addWidget(lineV, 5, 1);

    pLayout->addWidget(dLbl, 6, 0);
    pLayout->addWidget(tLbl, 6, 1);

    pLayout->addWidget(pOk, 7, 0);
    pLayout->addWidget(pCan, 7, 1);

    setLayout(pLayout);

    connect(pOk, SIGNAL(clicked()), SLOT(accept()));
    connect(pCan, SIGNAL(clicked()), SLOT(reject()));
}


void DialogPoint::setData(ObjectItem *data)
{
    /// отрезок [объект]
    QPointF start = data->getStartPoint();
    QPointF end = data->getEndPoint();
    /// начальная позиция
    xStart->setText(QString::number(start.x()));
    yStart->setText(QString::number(start.y()));

    /// текущая позиция
    xEnd->setText("X2 (м): " + QString::number(end.x()));
    yEnd->setText("Y2 (м): " + QString::number(end.y()));

    /// улон наклона
    lineK->setText(QString::number(data->getAngle()));
    /// скорость
    lineV->setText(QString::number(data->getVelocity()));

    /// добавим данные о пересечении, если есть
    if(data->getDistance().has_value()) {
        dLbl->setText(QString("D (м): %1").arg(data->getDist()));
    }

    if(data->getTime().has_value()) {
        tLbl->setText(QString("T (с): %1").arg(data->getTime()->toString(("h:m:s"))));
    }
}


SelectDialog::SelectDialog(QString name, quint8 number, QWidget* pwgt) : QPushButton(name, pwgt), name(name), number(number - 1)
{
    connect(this, SIGNAL(clicked()), SLOT(slotButtonClicked()));
}


DBDialog::DBDialog(QWidget* pwgt) : QDialog(pwgt)
{
    setWindowTitle("Указать Базу данных");
    dbNameLbl = new QLabel("&Database");
    dbNameLbl->setToolTip("Укажите название БД в СУБД PostreSQL");
    userNameLbl = new QLabel("&Username");
    dbNameLbl->setToolTip("Укажите пользователя БД");
    passLbl = new QLabel("&Password");
    passLbl->setToolTip("Укажите пароль пользователя");
    hostNameLbl = new QLabel("&Hostname");
    hostNameLbl->setToolTip("Укажите хост");
    portNameLbl = new QLabel("Po&rt");
    portNameLbl->setToolTip("Укажите порт, по умолчанию 5432");

    dbName = new QLineEdit;
    dbNameLbl->setBuddy(dbName);
    userName = new QLineEdit;
    userNameLbl->setBuddy(userName);
    pass = new QLineEdit;
    passLbl->setBuddy(pass);
    hostName = new QLineEdit;
    hostNameLbl->setBuddy(hostName);
    portName = new QLineEdit;
    portNameLbl->setBuddy(portName);

    ok = new QPushButton("&Ok");
    cancel = new QPushButton("&Cancel");
    cancel->setToolTip("Отмена подключения");

    connect(ok, SIGNAL(clicked()), SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), SLOT(reject()));

    QGridLayout* pgridLayout = new QGridLayout;
    pgridLayout->addWidget(dbNameLbl, 0, 0);
    pgridLayout->addWidget(dbName, 0, 1);
    pgridLayout->addWidget(userNameLbl, 1, 0);
    pgridLayout->addWidget(userName, 1, 1);
    pgridLayout->addWidget(passLbl, 2, 0);
    pgridLayout->addWidget(pass, 2, 1);
    pgridLayout->addWidget(hostNameLbl, 3, 0);
    pgridLayout->addWidget(hostName, 3, 1);
    pgridLayout->addWidget(portNameLbl, 4, 0);
    pgridLayout->addWidget(portName, 4, 1);
    pgridLayout->addWidget(ok, 5, 0);
    pgridLayout->addWidget(cancel, 5, 1);
    setLayout(pgridLayout);
}
