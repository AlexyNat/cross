#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets>
#include <QtSql>
#include "objectitem.h"

class SelectDialog;

class DialogPoint : public QDialog
{
    Q_OBJECT
    friend class SelectDialog;
public:
    DialogPoint(quint8 number, QWidget *parent = nullptr);
    void setData(ObjectItem *data);
private:
    quint8 number; /// Id объекта
    QLabel *xLblStart, *yLblStart; /// метки для начальной позиции
    QLineEdit *xStart, *yStart; /// поля ввода начальной позиции
    QLabel *xEnd, *yEnd; /// текущая позиция
    QLabel *kLbl, *vLbl; /// метки для угла и скорости
    QLineEdit *lineK, *lineV; /// поля ввода для угла и скорости
    QLabel *dLbl, *tLbl; /// метки для дистанции и времени
};


class SelectDialog : public QPushButton
{
    Q_OBJECT
public:
    SelectDialog(QString name,quint8 number, QWidget* pwgt = 0);
public slots:
    void slotButtonClicked();
private:
    QString name;
    quint8 number;
};

class DBDialog : public QDialog
{
    Q_OBJECT
public:
    DBDialog(QWidget* pwgt = 0);
    QString getDBName() const { return dbName->text(); }
    QString getUserName() const { return userName->text(); }
    QString getPassword() const { return pass->text(); }
    QString getHostname() const { return hostName->text(); }
    QString getPort() const { return portName->text(); }
private:
    QLabel *dbNameLbl, *userNameLbl, *passLbl, *hostNameLbl, *portNameLbl;
    QLineEdit *dbName, *userName, *pass, *hostName, *portName;
    QPushButton *ok, *cancel;
};

#endif // DIALOG_H
