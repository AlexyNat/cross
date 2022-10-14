#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include <vector>
#include <QtSql>

#include "view.h"
#include "dialog.h"
#include "objectitem.h"


class Widget : public QWidget
{
    Q_OBJECT
    friend class DialogPoint;
public:
    Widget(QWidget *parent = nullptr);
    void setObject(ObjectItem *data); /// установить объект
    QTime getTime() const { return time; }
    bool haveInter() const { return inter; }
    void setInter(bool inter) { this->inter = inter; }
    static bool createConnection(QString dbname, QString user, QString pass, QString host, QString port);
    void resetView();
    ObjectItem* getObject(quint8 number = 1); /// получить объект по Id
    std::vector<ObjectItem*> getObjects() { return points; } /// получить объекты
    ~Widget();
private:
    QTime time; /// время
    bool inter = false;
    QTimer *timer; /// таймер
    static bool dbCheck;
    QGraphicsScene scene; /// сцена
    View *view; /// отображение сцены
    std::vector<ObjectItem*> points; /// объекты
    SelectDialog *dialog[2]; /// диалоги для указания данных
    QLabel *timeLbl; /// счетчик
    QLineEdit *editTime; /// время с начала симуляции
    QPushButton *editTimeBtn;
    QLabel *lblIPos; /// пересечения
    QPushButton *save; /// кнопка сохранить в БД
    QPushButton *restore; /// кнопка востановить данные из БД
    QPushButton *stop; /// переключатель
    QPushButton *rt; /// сброс
    QBoxLayout *layout; /// менеджер компоновки
    void checkInter(); /// проверить пересечение
    QMenuBar *menuBar; /// меню
public slots:
    void step(); /// шаг приращения
    void reset(); /// сброс
    void state(); /// сменить состояние переключателя
    bool dataRestore(); /// получить данные из БД
    bool dataSave(); /// сохранить данные в БД
    void addTime(); /// установить начальное время
private slots:
    void slotDataBaseDialog(); /// диалог для установки БД
};


#endif // WIDGET_H
