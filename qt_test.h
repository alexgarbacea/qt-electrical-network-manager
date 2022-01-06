#ifndef QT_TEST_H
#define QT_TEST_H

#include <QObject>
#include <QtTest>

#include "statsframe.h"

class qt_test : public QObject
{
    Q_OBJECT
public:
    qt_test();
    ~qt_test();

private:
    StatsFrame stf;

private slots:
    void stats_avPowerFill();
    void stats_powerConsCalc();
    void stats_avHouseCons();
    void stats_houseCons();
    void stats_maxFillPow();
};

#endif // QT_TEST_H
