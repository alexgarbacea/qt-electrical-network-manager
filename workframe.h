#ifndef WORKFRAME_H
#define WORKFRAME_H

#include <QLabel>
#include <QFrame>
#include <QObject>
//db
#include <QtSql>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

extern int gnr, twr, hse, gnrOn, ctGnr, ctTwr, ctHse, hseOn, subSt, smlTwr;
extern QString nm, ds, toDel;
extern QLabel el;

class workframe : public QFrame
{
    Q_OBJECT

private:
    QSqlDatabase db;

public:
    explicit workframe(QWidget *parent = nullptr);

public slots:
    //context menu
    void showContextMenu(const QPoint &pos);
    //context menus options

    //#1 options for options
    void optionSave();
    //#2 options for options
    void optionLoad();
    //#3 options for options
    void optionExit();
    //#4
    void addBg();

    //generator menu options
    //#1 turn on
    void optionGeneratorOn();
    //#2 turn off
    void optionGeneratorOff();

    //house menu options
    //#1 turn on
    void optionHouseOn();
    //#2 turn off
    void optionHouseOff();

    //options available for all menus
    //delete element
    void optionDelete();
    //delete all elements from view
    void clearView();

protected:
    //override functions
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    //normal functions
    void updateInfo();
    void updatePowerInfo();
    void drawNewLine(double x1,double y1,double x2,double y2);
    void switchLineEdit(bool x);
    void checkElemDel();
    //db related functions
    void saveData();
    void loadData();
    //Qt elements
        //Labels
    //background
    QLabel *bg = new QLabel(this, Qt::WindowStaysOnBottomHint);
    //other
    QLabel *info = new QLabel(this);
    QLabel *powerInfo = new QLabel(this);
    QLabel *posInfo = new QLabel(this);
    QLabel *saveInfo = new QLabel(this);
        //used for object manipulation
    QLabel *objNow = new QLabel(this);
        //Highlight
    QLabel *highL = new QLabel(this);
        //LINE
    QLabel *lineInfo = new QLabel(this);
};

#endif // WORKFRAME_H
