#ifndef DELETESPACE_H
#define DELETESPACE_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QDropEvent;
class QDragEnterEvent;
QT_END_NAMESPACE

class deleteSpace : public QFrame
{
public:
    explicit deleteSpace(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DELETESPACE_H
