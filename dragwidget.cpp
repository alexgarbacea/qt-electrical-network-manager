#include <QtWidgets>
#include <QDebug>

#include "dragwidget.h"


QString descr;
bool fromDragger {false};
//! [0]
DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(100, 300);
    setMaximumWidth(100);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(false);
    setToolTip("Drag widgets from here into the main window");

    QLabel *towerIcon = new QLabel(this);
    towerIcon->setPixmap(QPixmap(":/images/tower.png"));
    towerIcon->move(10, 20);
    towerIcon->show();
    towerIcon->setAccessibleName("Tower");
    towerIcon->setAttribute(Qt::WA_DeleteOnClose);
    towerIcon->setToolTip(towerIcon->accessibleName());


    QLabel *generatorIcon = new QLabel(this);
    generatorIcon->setPixmap(QPixmap(":/images/generatorOff.png").scaled(75,54,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    generatorIcon->move(10, 90);
    generatorIcon->show();
    generatorIcon->setAccessibleName("Generator");
    generatorIcon->setAttribute(Qt::WA_DeleteOnClose);
    generatorIcon->setToolTip(generatorIcon->accessibleName());

    QLabel *houseIcon = new QLabel(this);
    houseIcon->setPixmap(QPixmap(":/images/houseOff.png"));
    houseIcon->move(10, 170);
    houseIcon->show();
    houseIcon->setAccessibleName("House");
    houseIcon->setAttribute(Qt::WA_DeleteOnClose);
    houseIcon->setToolTip(houseIcon->accessibleName());

    QLabel *substationIcon = new QLabel(this);
    substationIcon->setPixmap(QPixmap(":/images/substation.png"));
    substationIcon->move(10, 250);
    substationIcon->show();
    substationIcon->setAccessibleName("Substation");
    substationIcon->setAttribute(Qt::WA_DeleteOnClose);
    substationIcon->setToolTip(substationIcon->accessibleName());

    QLabel *smallTowerIcon = new QLabel(this);
    smallTowerIcon->setPixmap(QPixmap(":/images/smallTower.png"));
    smallTowerIcon->move(10, 330);
    smallTowerIcon->show();
    smallTowerIcon->setAccessibleName("smallTower");
    smallTowerIcon->setAttribute(Qt::WA_DeleteOnClose);
    smallTowerIcon->setToolTip(smallTowerIcon->accessibleName());
}

void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{

    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
            //qInfo("Drag event enter");
        } else {
            event->acceptProposedAction();

        }
    } else {
        event->ignore();
    }
}
void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();

        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void DragWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset;

        QLabel *newIcon = new QLabel(this);
        newIcon->setPixmap(pixmap);
        newIcon->move(event->position().toPoint() - offset);
        newIcon->show();
        newIcon->setAccessibleName(descr);
        newIcon->setAttribute(Qt::WA_DeleteOnClose);
        newIcon->setToolTip(descr);


        if (event->source() == this) {
            //event->setDropAction(Qt::MoveAction);
            //qInfo("Drop event");
            //event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }

}

//! [1]
void DragWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){//only on left button click move the icons

        QLabel *child = static_cast<QLabel*>(childAt(event->position().toPoint()));
        if (!child)
            return;
        fromDragger = true;

        //qDebug() << child->accessibleDescription();
        descr = child->accessibleName();

        QPixmap pixmap = child->pixmap(Qt::ReturnByValue);

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixmap << QPoint(event->position().toPoint() - child->pos());
    //! [1]

    //! [2]
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-dnditemdata", itemData);
    //! [2]

    //! [3]
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->position().toPoint() - child->pos());
    //! [3]

        QPixmap tempPixmap = pixmap;
        QPainter painter;
        painter.begin(&tempPixmap);
        painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
        painter.end();

        child->setPixmap(tempPixmap);

        if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {//CANNOT DELETE FROM THIS WIDGET BAR
            //child->close();
            child->show();
            child->setPixmap(pixmap);
        } else {
            child->show();
            child->setPixmap(pixmap);
        }
    }
    else{
        QString x = QString::number(event->position().rx()) +", "+ QString::number(event->position().ry());
        QMessageBox::information(this,"OK","You clicked.."+x);
    }

}


