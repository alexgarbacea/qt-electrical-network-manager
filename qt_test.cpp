#include "qt_test.h"
#include <QtTest>

qt_test::qt_test()
{

}

qt_test::~qt_test()
{

}

void qt_test::stats_avPowerFill() {
    double res = stf.avPowerFillData(10);
    QVERIFY(res == 0.72);
}

void qt_test::stats_powerConsCalc(){
    double res = stf.powerConsCalc();
    QVERIFY(res == 0);
}

void qt_test::stats_avHouseCons(){
    double res = stf.avHouseCons(10);
    QVERIFY(res == 500);
}

void qt_test::stats_houseCons(){
    double res = 1000 - stf.avHouseCons(8);
    QVERIFY(res == 600);
}

void qt_test::stats_maxFillPow(){
    double res = stf.avPowerFillData(6000);
    QVERIFY(res == 360);
}

//QTEST_MAIN(qt_test);



