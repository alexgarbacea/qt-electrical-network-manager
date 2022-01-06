#include "deletespace.h"
#include "workframe.h"

#include <QtWidgets>
#include <QDebug>


deleteSpace::deleteSpace(QWidget *parent) : QFrame(parent)
{
    setMinimumSize(80, 80);
    setMaximumSize(100,100);
    //setMaximumWidth(100);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);
    setCursor(Qt::ForbiddenCursor);
    setToolTip("Drag widgets to delete here");

    QLabel *deleteIcon = new QLabel(this);
    deleteIcon->setPixmap(QPixmap(":/images/delete.png"));
    deleteIcon->move(10, 10);
    deleteIcon->show();
    deleteIcon->setAttribute(Qt::WA_DeleteOnClose);

}

void deleteSpace::dragEnterEvent(QDragEnterEvent *event)
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
void deleteSpace::dragMoveEvent(QDragMoveEvent *event)
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


void deleteSpace::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if(nm.contains("Tower") && twr>0) {twr--;}
        else if(nm.contains("Substation") && subSt>0) {subSt--;}
        else if(nm.contains("smallTower") && smlTwr>0) {smlTwr--;}
        else if(nm.contains("House") && ds != "ON" && hse>0) {hse--;}
        else if(nm.contains("House") && ds == "ON" && hse>0 && hseOn>0) {hse--; hseOn--;}
        else if(nm.contains("Generator") && ds != "ON" && gnr>0) {gnr--;}
        else if(nm.contains("Generator") && ds == "ON" && gnr>0 && gnrOn>0) {gnr--; gnrOn--;}

        event->setDropAction(Qt::MoveAction);

        event->accept();

        toDel = nm;

        qDebug() << "Delete " <<nm<< " " << ds;
    } else {
        event->ignore();
    }

}

void deleteSpace::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){//only on left button click move the icons

        QLabel *child = static_cast<QLabel*>(childAt(event->position().toPoint()));
        if (!child)
            return;

        QPixmap pixmap = child->pixmap(Qt::ReturnByValue);

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixmap << QPoint(event->position().toPoint() - child->pos());

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-dnditemdata", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->position().toPoint() - child->pos());

        QPixmap tempPixmap = pixmap;
        QPainter painter;
        painter.begin(&tempPixmap);
        painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
        painter.end();
    }
    else{
        QString x = QString::number(event->position().rx()) +", "+ QString::number(event->position().ry());
        QMessageBox::information(this,"OK","You clicked.."+x);
    }

}

