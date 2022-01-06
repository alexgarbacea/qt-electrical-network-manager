#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QTest>

#include "dragwidget.h"
#include "workframe.h"
#include "deletespace.h"
#include "statsframe.h"
#include "qt_test.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(final);

    QApplication app(argc, argv);

    //the layouts
    QWidget mainWidget;
    QHBoxLayout *horizontalLayout = new QHBoxLayout(&mainWidget);
    QVBoxLayout *verticalLayout = new QVBoxLayout(&mainWidget);

    //Title for the draggable icons
    QLabel *title = new QLabel;
    title->setText("Drag icons from here");
    title->setMaximumHeight(15);
    title->move(10,0);


    //adding widgets to the layouts
    ///VERTICAL
    verticalLayout->addWidget(title);
    verticalLayout->addWidget(new DragWidget);
    verticalLayout->addWidget(new deleteSpace);
    ///HORIZONTAL
    horizontalLayout->addWidget(new workframe);
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addWidget(new StatsFrame);


    ///SHOW
    mainWidget.setWindowTitle(QObject::tr("Power grid"));
    mainWidget.setWindowIcon(QIcon(":/images/tower.png"));
    mainWidget.showMaximized();

    //init qt test
    QTest::qExec(new qt_test, argc, argv);

    return app.exec();
}

