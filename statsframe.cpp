#include "statsframe.h"
#include "workframe.h"

#include <QtWidgets>
#include <QtDebug>

double availablePower, usedPower;
int genPower {1000}, houseCons {50};

StatsFrame::StatsFrame(QWidget *parent) : QFrame(parent)
{
    setMinimumSize(400, 540);
    setMaximumWidth(400);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(false);

    //Title
    QLabel *wTitle = new QLabel(this);
    wTitle->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    wTitle->setAccessibleName("window title");
    wTitle->setText("Data");
    wTitle->move(10,0);
    wTitle->show();

    ///Available energy
    // First label TEXT
    powerAvTxt->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    powerAvTxt->setAccessibleName("Power text");
    powerAvTxt->move(10, 50);
    powerAvTxt->setStyleSheet("font-weight: bold;");
    powerAvTxt->setText("Available power:                    ");
    powerAvTxt->show();

    //Second label IMAGE FILL
    //QLabel *displayEnergyFill = new QLabel(this);
    displayEnergyFill->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    displayEnergyFill->setAccessibleName("Display enegry FILL");
    displayEnergyFill->move(15, 68);
    displayEnergyFill->setPixmap(QPixmap::fromImage((*powerAvFill).scaled(360,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    displayEnergyFill->show();

    //Second label IMAGE BACKGROUND
    QLabel *displayEnergy = new QLabel(this);
    displayEnergy->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    displayEnergy->setAccessibleName("Display enegry");
    displayEnergy->move(10, 60);
    displayEnergy->setPixmap(QPixmap::fromImage((*powerAv).scaled(400,100,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
    displayEnergy->show();

    ///Consumed energy
    // First label TEXT
    powerConsTxt->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    powerConsTxt->setAccessibleName("Consumption text");
    powerConsTxt->move(10, 200);
    powerConsTxt->setStyleSheet("font-weight: bold;");
    powerConsTxt->setText("Energy consumption:                    ");
    powerConsTxt->show();

    //Second label IMAGE FILL
    //QLabel *displayEnergyFill = new QLabel(this);
    displayConsFill->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    displayConsFill->setAccessibleName("Display cons FILL");
    displayConsFill->move(15, 218);
    displayConsFill->setPixmap(QPixmap::fromImage((*powerAvFill).scaled(360,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    displayConsFill->show();

    //Second label IMAGE BACKGROUND
    QLabel *displayCons = new QLabel(this);
    displayCons->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    displayCons->setAccessibleName("Display cons");
    displayCons->move(10, 210);
    displayCons->setPixmap(QPixmap::fromImage((*powerAv).scaled(400,100,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
    displayCons->show();

    //power alert
    powerAlert->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    powerAlert->move(100, 68);
    powerAlert->setStyleSheet("font-size:15px; color:red; font-weight: bold;");
    powerAlert->setText("NOT ENOUGH ENERGY");
    powerAlert->hide();

    //Buttons
    //~~~generator~~~
    //title
    powGenTitle->move(140,350);
    powGenTitle->setText("Power/Generator");
    powGenTitle->setStyleSheet("font-weight: bold;");
    powGenTitle->show();
    //power display
    powGenDisplay->move(155,380);
    powGenDisplay->setText(QString::number(genPower) + "kw   ");
    powGenDisplay->setStyleSheet("font-size: 18px; font-weight: bold; color: black;");
    powGenDisplay->show();
    //minus
    minusGen->setPixmap(QPixmap(":/images/minus.png").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    minusGen->setAccessibleName("minus generator");
    minusGen->setAccessibleDescription("isButton");
    minusGen->move(90, 375);
    minusGen->setToolTip("Decrease generator power by 100");
    minusGen->show();
    //plus
    plusGen->setPixmap(QPixmap(":/images/plus.png").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    plusGen->setAccessibleName("plus generator");
    plusGen->setAccessibleDescription("isButton");
    plusGen->move(230, 370);
    plusGen->setToolTip("Increase generator power by 100");
    plusGen->show();

    //~~~house~~~
    //title
    powHseTitle->move(140,450);
    powHseTitle->setText("Consumption/House");
    powHseTitle->setStyleSheet("font-weight: bold;");
    powHseTitle->show();
    //power display
    powHseDisplay->move(155,480);
    powHseDisplay->setText(QString::number(houseCons) + "kw     ");
    powHseDisplay->setStyleSheet("font-size: 18px; font-weight: bold; color: black;");
    powHseDisplay->show();
    //minus
    minusHse->setPixmap(QPixmap(":/images/minus.png").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    minusHse->setAccessibleName("minus house");
    minusHse->setAccessibleDescription("isButton");
    minusHse->move(90, 475);
    minusHse->setToolTip("Decrease house consumption by 50");
    minusHse->show();
    //plus
    plusHse->setPixmap(QPixmap(":/images/plus.png").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    plusHse->setAccessibleName("plus house");
    plusHse->setAccessibleDescription("isButton");
    plusHse->move(230, 470);
    plusHse->setToolTip("Increase house consumption by 50");
    plusHse->show();


    //update timer to update stats
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStats()));
    timer->start(1000);

}

double StatsFrame::avPowerFillData(double x){
    if(x<=5000 && x>=0){
        return (x*360)/5000;
    }
    else if(x>5000){
        return 360;
    }
    else{
        return 0;
    }
}

double StatsFrame::avHouseCons(double x){
    if(x > 0){
        return x * houseCons;
    }
    else{
        return 0;
    }
}

double StatsFrame::powerConsCalc(){
    return (hseOn > 0) ? (houseCons * hseOn) : 0;
}

void StatsFrame::updateStats(){
    //power consumption
    usedPower = powerConsCalc();

    powerConsTxt->setText("Energy consumption: "+QString::number(usedPower)+"kw/h");
    displayConsFill->setPixmap(QPixmap::fromImage((*powerAvFill).scaled(avPowerFillData(usedPower),22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));

    //available power
    availablePower = gnrOn*genPower;
    powerAvTxt->setText("Available power: "+QString::number(availablePower-usedPower)+"kw/h");
    displayEnergyFill->setPixmap(QPixmap::fromImage((*powerAvFill).scaled(avPowerFillData(availablePower-usedPower),22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));

    if(usedPower >= availablePower && usedPower > 0){
        powerAlert->show();
    }
    else{
        powerAlert->hide();
    }
}

void StatsFrame::mousePressEvent(QMouseEvent *event){
    QLabel *child = static_cast<QLabel*>(childAt(event->position().toPoint()));
    if(child && child->accessibleDescription().contains("isButton"))
        energyCalc(child->accessibleName());
    else
        return;
}

void StatsFrame::energyCalc(QString s){
    if(s.contains("plus")){
        if(s.contains("generator")){
            if(genPower < 3000){
                int endVal = genPower + 100;
                for(int i = genPower; i < endVal; i++){
                    genPower++;
                    powGenDisplay->setText(QString::number(genPower) + "kw   ");
                    powGenDisplay->repaint();
                    powGenDisplay->show();
                }
            }
        }
        else if(s.contains("house")){
            if(houseCons < 1000){
                int endVal = houseCons + 50;
                for(int i = houseCons; i < endVal; i++){
                    houseCons++;
                    powHseDisplay->setText(QString::number(houseCons) + "kw     ");
                    powHseDisplay->repaint();
                    powHseDisplay->show();
                }
            }
        }
        else
            return;
    }
    else if(s.contains("minus")){
        if(s.contains("generator")){
            if(genPower > 500){
                int endVal = genPower - 100;
                for(int i = genPower; i > endVal; i--){
                    genPower--;
                    powGenDisplay->setText(QString::number(genPower) + "kw   ");
                    powGenDisplay->repaint();
                    powGenDisplay->show();
                }
            }
        }
        else if(s.contains("house")){
            if(houseCons > 50){
                int endVal = houseCons - 50;
                for(int i = houseCons; i > endVal; i--){
                    houseCons--;
                    powHseDisplay->setText(QString::number(houseCons) + "kw     ");
                    powHseDisplay->repaint();
                    powHseDisplay->show();
                }
            }
        }
        else
            return;
    }
    else
        qDebug() << "Not a working button";

    updateStats();
}


