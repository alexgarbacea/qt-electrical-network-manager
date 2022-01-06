#ifndef STATSFRAME_H
#define STATSFRAME_H

#include <QFrame>
#include <QtWidgets>
#include <QObject>

class StatsFrame : public QFrame
{
    Q_OBJECT
public:
    explicit StatsFrame(QWidget *parent = nullptr);
    double avPowerFillData(double x);
    double powerConsCalc();
    double avHouseCons(double x);

public slots:
    void updateStats();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    //display energy available
    QImage *powerAv = new QImage(":/images/bgFillerKw.png");
    QImage *powerAvFill = new QImage(":/images/fillerKw.png");
    QLabel *powerAvTxt = new QLabel(this);
    QLabel *displayEnergyFill = new QLabel(this);
    //display energy consumption
    QImage *powerCons = new QImage(":/images/bgFillerKw.png");
    QImage *powerConsFill = new QImage(":/images/fillerKw.png");
    QLabel *powerConsTxt = new QLabel(this);
    QLabel *displayConsFill = new QLabel(this);
    //buttons for energy
    //~generator
    QLabel *powGenTitle = new QLabel(this);
    QLabel *powGenDisplay = new QLabel(this);
    QLabel *plusGen = new QLabel(this);
    QLabel *minusGen = new QLabel(this);
    //~house
    QLabel *powHseTitle = new QLabel(this);
    QLabel *powHseDisplay = new QLabel(this);
    QLabel *plusHse = new QLabel(this);
    QLabel *minusHse = new QLabel(this);
    //misc
    QLabel *powerAlert = new QLabel(this);
    //functions
    void energyCalc(QString s);//calculate energy/cons change


};

#endif // STATSFRAME_H
