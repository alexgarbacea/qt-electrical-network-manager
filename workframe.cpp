#include <QtWidgets>
#include <QDebug>
#include <QVector>
#include <vector>

#include "workframe.h"
#include "dragwidget.h"
#include "statsframe.h"


int gnr {0}, twr {0}, hse {0}, subSt {0}, smlTwr {0}, gnrOn {0}, ctGnr {0}, ctTwr {0}, ctsubSt {0}, ctsmlTwr{0}, ctHse {0}, hseOn {0};
unsigned long ctLn {0};
double linePoint[4] = {-1, -1, -1, -1};
bool lineOn {false}, isOn {false};
QString nm {}, ds {}, obj1 {}, obj2 {}, toDel {""};
//enumerations for context menus
enum menus {options, generator, house, tower, substation, smallTower, connector, none};
menus ctxMenu = none;

//class for drawing lines
class Lines {
public:
    const QLabel *ln = new QLabel;
    double xp1, yp1, xp2, yp2;
    QString ob1, ob2;
    unsigned long counter {0};
    Lines(const QLabel &l , QString o1, QString o2, double x1, double y1, double x2, double y2)
    {   counter = ctLn;
        ln = &l;
        ob1 = o1;
        ob2 = o2;
        xp1 = x1;
        yp1 = y1;
        xp2 = x2;
        yp2 = y2;
    }
    void display(){
        qDebug() << counter << ".Line: between " << ob1 << " and " << ob2;
    }
};

//class for elements in frame
class Elems {
public:
    const QLabel *el = new QLabel;
    QString name;
    Elems(const QLabel &l, QString s){ el= &l; name= s; };//constructor
};

std::vector<Lines> line {};//vector to store lines info

std::vector <Elems> elements;//vector to store added elements

workframe::workframe(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(600, 400);
    //setMaximumWidth(100);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    //background
    bg->move(10,55);
    bg->setMinimumSize(400,340);
    bg->setAccessibleName("info bg");
    bg->hide();

    ///INFO ON TOP
    //info = new QLabel(this);
    info->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    info->setAccessibleName("info");
    updateInfo();
    info->move(10,0);
    info->show();

    //power info
    powerInfo->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    powerInfo->setAccessibleName("info");
    powerInfo->move(10,20);
    updatePowerInfo();
    powerInfo->show();

    //position info
    posInfo->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    posInfo->setAccessibleName("info");
    posInfo->move(100,20);
    posInfo->setText("X: 1000000 Y: 1000000");
    posInfo->hide();

    //save info
    saveInfo->setFrameStyle(QFrame::Sunken | QFrame::Sunken);
    saveInfo->setAccessibleName("info");
    saveInfo->move(10,60);
    saveInfo->setText("SAVING.. 100%");
    saveInfo->hide();

    //draw line key
    lineInfo->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lineInfo->setAccessibleName("info");
    lineInfo->move(10,40);
    lineInfo->setText("Press L to start drawing lines   ");
    lineInfo->show();

    //highlight
    highL->setPixmap(QPixmap(":/images/highlight.png"));
    highL->move(0, 0);
    highL->hide();
    highL->setAccessibleName("Highlight info");
    highL->setAttribute(Qt::WA_DeleteOnClose);

    //connect to SQLite DB
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/Proj/QT/Project/final/db/scada_db.db");

}

void workframe::updateInfo(){
    if(gnrOn<0 || gnr == 0) gnrOn = 0;
    int ofs = gnr-gnrOn;
    info->setText("Generators: " + QString::number(gnr)+"(ON: "+
    QString::number(gnrOn) +
    "/OFF: " + QString::number(ofs) +") | "+
    "Towers: "+QString::number(twr)+" | "+
    "Houses: "+QString::number(hse)+" | "+
    "Substations: "+QString::number(subSt)+" | "+
    "Small towers: "+QString::number(smlTwr)+"             ");
}

void workframe::updatePowerInfo(){
    if(gnrOn<1){
        powerInfo->setStyleSheet("background-color: yellow; color: red; font-weight: bold;");
        powerInfo->setText("NO POWER!");
    }
    else{
        powerInfo->setStyleSheet("background-color: green; color: white; font-weight: bold;");
        powerInfo->setText("POWER ON");
    }
}

//DRAW A NEW LINE
void workframe::drawNewLine(double x1,double y1,double x2,double y2){
    QPicture pi;
    QPainter p(&pi);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(x1, y1, x2, y2);
    p.end();
    //show
    QLabel *ln = new QLabel(this);
    ln->setPicture(pi);

    //Data for access
    ln->setAccessibleName("Line");
    ln->setAccessibleDescription(QString::number(ctLn));
    ln->setToolTip("Left-click to delete line");

    if(x1<=x2 && y1<=y2){
        ln->move(x1,y1);
    }
    else if(x1>=x2 && y1>=y2){
        ln->move(x2,y2);
    }
    else if(x1<=x2 && y1>=y2){
        ln->move(x1,y2);
    }
    else{
        ln->move(x2,y1);
    }

    std::unique_ptr<Lines> l (new Lines(*ln,obj1,obj2, x1, y1, x2, y2));
    line.push_back(*l);//ADD LINE INTO VECTOR

    ln->show();

    ctLn++;//increment counter

    //reset points
    linePoint[0] = -1;
    linePoint[1] = -1;
    linePoint[2] = -1;
    linePoint[3] = -1;

//    for(Lines i : line){
//        i.display();
//    }
}

void workframe::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            posInfo->show();
            event->setDropAction(Qt::MoveAction);
            event->accept();

        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}
void workframe::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            posInfo->setText("X: "+QString::number(event->position().rx())+" Y: "+QString::number(event->position().ry()));
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();

    }
}



void workframe::dropEvent(QDropEvent *event)
{
    posInfo->hide();

    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {

        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;

        dataStream >> pixmap >> offset;


        QLabel *newIcon = new QLabel(this);
        newIcon->setPixmap(pixmap);
        newIcon->move(event->position().toPoint() - offset);
        newIcon->setAttribute(Qt::WA_DeleteOnClose);

        if(fromDragger){//Check to see if it s from the widget dragger and increment counters
            if(descr == "Tower"){
                twr++;
                ctTwr++;
                newIcon->setAccessibleName(descr + " " + QString::number(ctTwr));
                newIcon->setToolTip(newIcon->accessibleName());
            }
            else if(descr == "Generator"){
                gnr++;
                ctGnr++;
                newIcon->setAccessibleName(descr + " " + QString::number(ctGnr));
                newIcon->setToolTip(newIcon->accessibleName());
            }
            else if(descr == "House"){
                hse++;
                ctHse++;
                newIcon->setAccessibleName(descr + " " + QString::number(ctHse));
                newIcon->setToolTip(newIcon->accessibleName());
            }
            else if(descr == "Substation"){
                subSt++;
                ctsubSt++;
                newIcon->setAccessibleName(descr + " " + QString::number(ctsubSt));
                newIcon->setToolTip(newIcon->accessibleName());
            }
            else if(descr == "smallTower"){
                smlTwr++;
                ctsmlTwr++;
                newIcon->setAccessibleName(descr + " " + QString::number(ctsmlTwr));
                newIcon->setToolTip(newIcon->accessibleName());
            }
            fromDragger = false;
            //add element to qvector
            std::unique_ptr<Elems> e (new Elems(*newIcon, newIcon->accessibleName()));
            elements.push_back(*e);
        }
        else{//IF IT DROPS FROM THE WORKFRAME
            newIcon->setAccessibleName(nm);
            newIcon->setAccessibleDescription(ds);
            newIcon->setToolTip(newIcon->accessibleName());
        }

        updateInfo();

        newIcon->show();

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    }else {
        event->ignore();
    }


}
//mouse press event check
void workframe::mousePressEvent(QMouseEvent *event)
{
    //context menu set to none
    this->setContextMenuPolicy(Qt::NoContextMenu);
    //
    checkElemDel();

    highL->hide();

    QLabel *child = static_cast<QLabel*>(childAt(event->position().toPoint()));

    //only on left button click move the icons
    if(child && event->button() == Qt::LeftButton && !child->accessibleName().contains("info") && child->accessibleName() != "Line"){

        bool isConnected {false};

        for(auto& ln : line){//check if the child is connected with a line
            if(ln.ob1 == child->accessibleName() || ln.ob2 == child->accessibleName()){
                isConnected = true;
                ln.display();
                break;
            }
        }

        if(!isConnected){//if child is not connected with a line

            QPixmap pixmap = child->pixmap(Qt::ReturnByValue);

            //qDebug() << child->accessibleDescription();
            nm = child->accessibleName();
            ds = child->accessibleDescription();

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

            child->setPixmap(tempPixmap);

            if (drag->exec() == Qt::MoveAction) {
                child->close();
            } else {
                child->show();
                child->setPixmap(pixmap);
            }
        }
        else{
            QMessageBox msgBox;
            msgBox.setWindowTitle("Already connected");
            msgBox.setText("Delete the line connecting the elements before moving them.");
            msgBox.exec();
        }
    }
    else if(child && !lineOn && event->button() == Qt::RightButton && child->accessibleName() == "Line"){//right click line
        //set enumerator
        ctxMenu = connector;
        //set qlabel
        objNow = child;
        //generator context menu show
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showContextMenu(const QPoint &)));

    }
    else if(child && !lineOn && event->button() == Qt::RightButton && child->accessibleName().contains("Generator")){//right click the generator
        //set enumerator
        ctxMenu = generator;
        //check generator state
        child->accessibleDescription()=="ON" ? isOn = true : isOn = false;
        //set qlabel
        objNow = child;
        //generator context menu show
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showContextMenu(const QPoint &)));
    }
    else if(child && !lineOn && event->button() == Qt::RightButton && child->accessibleName().contains("House")){//right click house
        //set enumerator
        ctxMenu = house;
        //check house state
        child->accessibleDescription()=="ON" ? isOn = true : isOn = false;
        //set qlabel
        objNow = child;
        //generator context menu show
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showContextMenu(const QPoint &)));
    }
    else if(child && !lineOn && event->button() == Qt::RightButton && child->accessibleName().contains("Tower")){//right click tower
        //set enumerator
        ctxMenu = tower;
        //set qlabel
        objNow = child;
        //generator context menu show
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showContextMenu(const QPoint &)));
    }
    else if(child && !lineOn && event->button() == Qt::RightButton && child->accessibleName().contains("Substation")){//right click tower
        //set enumerator
        ctxMenu = substation;
        //set qlabel
        objNow = child;
        //generator context menu show
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showContextMenu(const QPoint &)));
    }
    else if(child && !lineOn && event->button() == Qt::RightButton && child->accessibleName().contains("smallTower")){//right click tower
        //set enumerator
        ctxMenu = smallTower;
        //set qlabel
        objNow = child;
        //generator context menu show
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showContextMenu(const QPoint &)));
    }
    else if(event->button() == Qt::RightButton && !lineOn){//Operations box ~ save ~ load ~ from database ~ right click menu
        try{
            //set enumerator
            ctxMenu = options;
            //options context menu show
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showContextMenu(const QPoint &)));
        }
        catch(const std::exception& e){
            qDebug() << e.what();
        }
    }
    else if(event->button() == Qt::RightButton && lineOn){//DRAW NEW LINE ON CLICK

        if(child){//check if there is a child
            if(child->accessibleName().contains("Line")){//check for child not to be a line
                QMessageBox msgBox;
                msgBox.setWindowTitle("Connexion not possible");
                msgBox.setText("You can not connect to a line.");
                switchLineEdit(false);
                msgBox.exec();
                switchLineEdit(false);

                return;
            }
            nm = child->accessibleName();

            if(linePoint[0] == -1 && linePoint[1] == -1){//first point of line

                linePoint[0] = event->position().rx();
                linePoint[1] = event->position().ry();
                obj1 = nm;

                if(child){//highlight the selected component
                    highL->move((child->pos().x()-10),(child->pos().y()-8));
                    highL->show();

                }
            }
            else if(linePoint[2] == -1 && linePoint[3] == -1){//second point of line and line draw
                linePoint[2] = event->position().rx();
                linePoint[3] = event->position().ry();
                obj2 = nm;

                //draw new line
                if(obj1 != obj2 && obj1 != "" && obj2 != ""
                        && !obj1.contains("Line") && !obj2.contains("Line")
                        && !obj1.contains("info") && !obj2.contains("info"))
                {
                    drawNewLine(linePoint[0],linePoint[1],linePoint[2],linePoint[3]);
                }
                else{
                    QMessageBox msgBox;
                    msgBox.setWindowTitle("Connexion not possible");
                    msgBox.setText("All lines must bind at least 2 elements.");
                    switchLineEdit(false);
                    msgBox.exec();
                }

            }
        }
        else{
            QMessageBox msgBox;
            msgBox.setWindowTitle("Connexion not possible");
            msgBox.setText("Select an element.");
            switchLineEdit(false);
            msgBox.exec();

            switchLineEdit(false);
        }

    }


}

//pressed key ~ L ~ to toggle line edit
void workframe::keyPressEvent(QKeyEvent *event){
    event->accept();
    if(event->key() == Qt::Key_L && !lineOn)
    {
        switchLineEdit(true);
    }
    else if(event->key() == Qt::Key_L && lineOn){
        switchLineEdit(false);
    }

}

void workframe::switchLineEdit(bool x){
    lineOn = x;
    highL->hide();

    if(lineOn){
        lineInfo->setStyleSheet("background-color: green; color: white; font-weight: bold;");
        lineInfo->setText("DRAW LINES (right click)");
    }
    else{
        lineInfo->setStyleSheet("background-color: none; color: black; font-weight: normal;");
        lineInfo->setText("Press L to start drawing lines   ");
        //reset line pointers
        linePoint[0] = -1;
        linePoint[1] = -1;
        linePoint[2] = -1;
        linePoint[3] = -1;
    }
}

//check to see if there is a need for element to be deleted
void workframe::checkElemDel(){

    if(toDel != ""){
        //search the deleted element in vector
        int i = 0;
        for(auto &el : elements){
            if(el.name.contains(toDel)){
                //erase the deleted element from vector
                qDebug() << "Erased " << el.name << " remaining " << elements.size()-1;
                elements.erase(elements.begin()+i);
                break;
            }
            else
                i++;
        }
        toDel = "";
    }
    //update stats
    updateInfo();
    updatePowerInfo();
}

//~~~ db ~~~ function SAVE DATA
void workframe::saveData(){
    if(db.open()){
        //saving display percent
        int savePrc = 0;
        saveInfo->setText("SAVING..         ");
        saveInfo->show();
        saveInfo->repaint();

        //calculate percent
        int totalLoad = elements.size() + line.size();
        int prcNow = 0;

        //elements
        if(elements.size()>0){
            //query
            QSqlQuery qry;
            //delete all existing elements from db table
            qry.exec("DELETE FROM elements");
            //insert elements to db
            for(auto &el : elements){
                try{
                    qry.prepare("INSERT INTO elements (posX, posY, name, description) VALUES (:posX, :posY, :name, :description)");
                    qry.bindValue(":posX", (el.el->pos().x)());
                    qry.bindValue(":posY", (el.el->pos().y)());
                    qry.bindValue(":name", (el.el->accessibleName()));
                    qry.bindValue(":description", (el.el->accessibleDescription()));
                    if(qry.exec()){//saved OK
                        //display save percent
                        savePrc++;
                        prcNow = (savePrc * 100)/totalLoad;
                        saveInfo->setText("SAVING.. " + QString::number(prcNow) + "%");
                        saveInfo->show();
                        saveInfo->repaint();
                    }
                    else{//error saving
                        QMessageBox::information(this,"ERROR","Error saving data..");
                        saveInfo->hide();
                        return;
                    }
                }
                catch(const std::exception& e){
                    qDebug() << e.what();
                }
            }

        }

        //lines
        if(line.size() > 0){
            QSqlQuery qry;
            //delete all existing lines from db table
            qry.exec("DELETE FROM lines");

            //insert lines to db
            for(auto &ln : line){
                try{
                    qry.prepare("INSERT INTO lines (posX, posY, obj1, obj2, x1, y1, x2, y2) VALUES (:posX, :posY, :obj1, :obj2, :x1, :y1, :x2, :y2)");
                    qry.bindValue(":posX", (ln.ln->pos().x)());
                    qry.bindValue(":posY", (ln.ln->pos().y)());
                    qry.bindValue(":obj1", (ln.ob1));
                    qry.bindValue(":obj2", (ln.ob2));
                    qry.bindValue(":x1", (ln.xp1));
                    qry.bindValue(":y1", (ln.yp1));
                    qry.bindValue(":x2", (ln.xp2));
                    qry.bindValue(":y2", (ln.yp2));
                    if(qry.exec()){//saved OK
                        savePrc++;
                        prcNow = (savePrc * 100)/totalLoad;
                        saveInfo->setText("SAVING.. " + QString::number(prcNow) + "%");
                        saveInfo->show();
                        saveInfo->repaint();
                    }
                    else{//error saving
                        QMessageBox::information(this,"ERROR","Error saving data..");
                        saveInfo->hide();
                        return;
                    }
                }
                catch(const std::exception& e){
                    qDebug() << e.what();
                }
            }
        }

        saveInfo->hide();
        updateInfo();
        updatePowerInfo();

    }
    else{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Connexion not possible");
        msgBox.setText("Database not connected.");
        msgBox.exec();
    }
}

//~ db ~ LOAD DATA
void workframe::loadData(){
    if(db.open()){
        //query
        QSqlQuery qry;
        //elements
        qry.prepare(QString("SELECT * FROM elements"));

        if(!qry.exec()){
            //not executed
            QMessageBox::information(this,"ERROR","Error fetching data..");
        }
        else{
            try{
                clearView();
                while(qry.next()){
                    //populate workframe with elements
                    QLabel *newIcon = new QLabel(this);
                    newIcon->move(qry.value(1).toInt(),qry.value(2).toInt());//position
                    newIcon->setAttribute(Qt::WA_DeleteOnClose);
                    newIcon->setAccessibleDescription(qry.value(4).toString());//description

                    if(qry.value(3).toString().contains("Generator")){
                        gnr++;
                        ctGnr++;
                        newIcon->setAccessibleName("Generator " + QString::number(ctGnr));
                        newIcon->setToolTip(newIcon->accessibleName());
                        if(newIcon->accessibleDescription().contains("ON")){
                            newIcon->setPixmap(QPixmap(":/images/generator.png"));
                            gnrOn++;
                        }
                        else{
                            newIcon->setPixmap(QPixmap(":/images/generatorOff.png"));
                        }
                    }
                    else if(qry.value(3).toString().contains("smallTower")){
                        smlTwr++;
                        ctsmlTwr++;
                        newIcon->setAccessibleName("smallTower " + QString::number(ctsmlTwr));
                        newIcon->setToolTip(newIcon->accessibleName());
                        newIcon->setPixmap(QPixmap(":/images/smallTower.png"));
                    }
                    else if(qry.value(3).toString().contains("Substation")){
                        subSt++;
                        ctsubSt++;
                        newIcon->setAccessibleName("Substation " + QString::number(ctsubSt));
                        newIcon->setToolTip(newIcon->accessibleName());
                        newIcon->setPixmap(QPixmap(":/images/substation.png"));
                    }
                    else if(qry.value(3).toString().contains("Tower")){
                        twr++;
                        ctTwr++;
                        newIcon->setAccessibleName("Tower " + QString::number(ctTwr));
                        newIcon->setToolTip(newIcon->accessibleName());
                        newIcon->setPixmap(QPixmap(":/images/tower.png"));
                    }
                    else if(qry.value(3).toString().contains("House")){
                        hse++;
                        ctHse++;
                        newIcon->setAccessibleName("House " + QString::number(ctHse));
                        newIcon->setToolTip(newIcon->accessibleName());
                        if(newIcon->accessibleDescription().contains("ON")){
                            newIcon->setPixmap(QPixmap(":/images/house.png"));
                            hseOn++;
                        }
                        else{
                            newIcon->setPixmap(QPixmap(":/images/houseOff.png"));
                        }
                    }
                    //add element to vector
                    std::unique_ptr<Elems> e (new Elems(*newIcon, newIcon->accessibleName()));
                    elements.push_back(*e);

                    newIcon->show();


                }
            }
            catch(const std::exception& e){
                qDebug() << e.what();
            }

            //lines
            qry.prepare(QString("SELECT * FROM lines"));

            if(!qry.exec()){
                //not executed
                QMessageBox::information(this,"ERROR","Error fetching data..");
            }
            else{
                try{
                    while(qry.next()){
                        QPicture pi;
                        QPainter p(&pi);
                        p.setRenderHint(QPainter::Antialiasing);
                        p.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
                        p.drawLine(qry.value(5).toInt(), qry.value(6).toInt(), qry.value(7).toInt(), qry.value(8).toInt());
                        p.end();
                        //show
                        QLabel *ln = new QLabel(this);
                        ln->setPicture(pi);

                        //Data for access
                        ln->setAccessibleName("Line");
                        ln->setAccessibleDescription(QString::number(ctLn));
                        ln->setToolTip("Left-click to delete line");

                        ln->move(qry.value(1).toInt(),qry.value(2).toInt());

                        std::unique_ptr<Lines> l (new Lines(*ln,
                                                            qry.value(3).toString(),
                                                            qry.value(4).toString(),
                                                            qry.value(5).toInt(),
                                                            qry.value(6).toInt(),
                                                            qry.value(7).toInt(),
                                                            qry.value(8).toInt()));
                        line.push_back(*l);//ADD LINE INTO VECTOR

                        ln->show();

                        ctLn++;//increment counter
                    }
                }
                catch(const std::exception& e){
                    qDebug() << e.what();
                }
            }

            updateInfo();
            updatePowerInfo();
        }
    }
    else{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Connexion not possible");
        msgBox.setText("Database not connected.");
        msgBox.exec();
    }
}

//~~~ context menu and options ~~~
void workframe::showContextMenu(const QPoint &pos){
    try{
        //check if you can delete element
        bool canDelete = true;
        try{
            if(ctxMenu != options && ctxMenu != connector && line.size() > 0){
                QString nm = objNow->accessibleName();
                for(auto& ln : line){
                    if(ln.ob1 == nm || ln.ob2 == nm){
                        canDelete = false;
                        break;
                    }
                }
            }
        }
        catch(const std::exception& e){
            qDebug() << e.what();
        }

        switch(ctxMenu){
        case options:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Save data", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionSave()));
            contextMenu.addAction(&action1);
            //
            QAction action2("Load data", this);
            connect(&action2, SIGNAL(triggered()), this, SLOT(optionLoad()));
            contextMenu.addAction(&action2);
            //
            contextMenu.addSeparator();
            QAction action3("Add background", this);
            connect(&action3, SIGNAL(triggered()), this, SLOT(addBg()));
            contextMenu.addAction(&action3);
            //
            contextMenu.addSeparator();
            QAction action4("Delete all", this);
            connect(&action4, SIGNAL(triggered()), this, SLOT(clearView()));
            contextMenu.addAction(&action4);
            //
            contextMenu.addSeparator();
            QAction action5("Exit", this);
            connect(&action5, SIGNAL(triggered()), this, SLOT(optionExit()));
            contextMenu.addAction(&action5);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case generator:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Turn on", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionGeneratorOn()));
            contextMenu.addAction(&action1);
            //
            QAction action2("Turn off", this);
            connect(&action2, SIGNAL(triggered()), this, SLOT(optionGeneratorOff()));
            contextMenu.addAction(&action2);
            //
            contextMenu.addSeparator();
            QAction action3("Delete generator", this);
            connect(&action3, SIGNAL(triggered()), this, SLOT(optionDelete()));
            contextMenu.addAction(&action3);
            //set disabled menu
            isOn ? action1.setEnabled(false) : action2.setEnabled(false);
            if(!canDelete) action3.setEnabled(false);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case house:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Power on", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionHouseOn()));
            contextMenu.addAction(&action1);
            //
            QAction action2("Power off", this);
            connect(&action2, SIGNAL(triggered()), this, SLOT(optionHouseOff()));
            contextMenu.addAction(&action2);
            //
            contextMenu.addSeparator();
            QAction action3("Delete house", this);
            connect(&action3, SIGNAL(triggered()), this, SLOT(optionDelete()));
            contextMenu.addAction(&action3);
            //set disabled menu
            isOn ? action1.setEnabled(false) : action2.setEnabled(false);
            if(!canDelete) action3.setEnabled(false);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case tower:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Delete tower", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionDelete()));
            contextMenu.addAction(&action1);
            if(!canDelete) action1.setEnabled(false);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case substation:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Delete substation", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionDelete()));
            contextMenu.addAction(&action1);
            if(!canDelete) action1.setEnabled(false);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case smallTower:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Delete small tower", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionDelete()));
            contextMenu.addAction(&action1);
            if(!canDelete) action1.setEnabled(false);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case connector:
        {
            QMenu contextMenu(tr("Context menu"), this);
            QAction action1("Delete line", this);
            connect(&action1, SIGNAL(triggered()), this, SLOT(optionDelete()));
            contextMenu.addAction(&action1);
            //show menu
            contextMenu.exec(mapToGlobal(pos));
        }
            break;
        case none:
            break;
        default:
            break;
        }

        ctxMenu = none;
    }
    catch(const std::exception& e){
        qDebug() << e.what();
    }

}

//options menu options
void workframe::optionSave(){
    qDebug() << "SAVE";
    saveData();
}

void workframe::optionLoad(){
    qDebug() << "LOAD";
    loadData();
}

void workframe::optionExit(){
    qDebug() << "Exiting..";
    QApplication::quit();
}

void workframe::addBg(){
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose a file"), "", tr("Images (*.png *.jpg *.jpeg)"));

    if(QString::compare(filename, QString()) != 0){
        QImage img;
        bool valid = img.load(filename);

        if(valid){
            bg->setMinimumSize(this->width()-30, this->height()-60);
            img = img.scaledToWidth(bg->width(), Qt::SmoothTransformation);
            bg->setPixmap(QPixmap::fromImage(img));
            bg->show();
        }
    }
}

//options generator menu
void workframe::optionGeneratorOn(){
    qDebug() << "Turning on generator..";
    objNow->setPixmap(QPixmap(":/images/generator.png"));
    objNow->setAccessibleDescription("ON");
    gnrOn++;
    updateInfo();
    updatePowerInfo();
}

void workframe::optionGeneratorOff(){
    qDebug() << "Turning off generator..";
    objNow->setPixmap(QPixmap(":/images/generatorOff.png"));
    objNow->setAccessibleDescription("");
    if(gnrOn>0) gnrOn--;
    updateInfo();
    updatePowerInfo();
}

//options house menu
void workframe::optionHouseOn(){
    qDebug() << "Turning on house..";
    objNow->setPixmap(QPixmap(":/images/house.png"));
    objNow->setAccessibleDescription("ON");
    hseOn++;
    updateInfo();
    updatePowerInfo();
}

void workframe::optionHouseOff(){
    qDebug() << "Turning off house..";
    objNow->setPixmap(QPixmap(":/images/houseOff.png"));
    objNow->setAccessibleDescription("");
    if(hseOn>0) hseOn--;
    updateInfo();
    updatePowerInfo();
}

//general option
void workframe::optionDelete(){
    toDel = objNow->accessibleName();

    if(objNow->accessibleName().contains("Tower") && twr>0) {twr--;}
    else if(objNow->accessibleName().contains("Substation") && subSt>0) {subSt--;}
    else if(objNow->accessibleName().contains("smallTower") && smlTwr>0) {smlTwr--;}
    else if(objNow->accessibleName().contains("House") && objNow->accessibleDescription() != "ON" && hse>0) {hse--;}
    else if(objNow->accessibleName().contains("House") && objNow->accessibleDescription() == "ON" && hse>0 && hseOn>0) {hse--; hseOn--;}
    else if(objNow->accessibleName().contains("Generator") && objNow->accessibleDescription() != "ON" && gnr>0) {gnr--;}
    else if(objNow->accessibleName().contains("Generator") && objNow->accessibleDescription() == "ON" && gnr>0 && gnrOn>0) {gnr--; gnrOn--;}
    else if(objNow->accessibleName().contains("Line")){
        toDel = "";
        //search the deleted line in vector
        int i = 0;
        for(auto &ln : line){
            if(ln.ln->accessibleDescription().contains(objNow->accessibleDescription())){
                //erase the deleted element from vector
                qDebug() << "Erased Line " << ln.ln->accessibleDescription() << " remaining " << line.size()-1;
                line.erase(line.begin()+i);
                break;
            }
            else
                i++;
        }
    }
    objNow->close();
    //delete from vector
    checkElemDel();
}

void workframe::clearView(){
    try{
        if(elements.size() > 0 || line.size() > 0){
            QList<QLabel *> allChildren = this->findChildren<QLabel *>();

            if(allChildren.size()>0){
                for(auto c : allChildren){
                    if(!c->accessibleName().contains("info") && c->accessibleName() != ""){
                        objNow = c;
                        optionDelete();
                    }
                }
                //set counters to 0
                ctGnr = 0;
                ctTwr = 0;
                ctHse = 0;
                ctLn = 0;
                ctsubSt = 0;
                ctsmlTwr = 0;
            }
        }
    }
    catch(const std::exception& e){
        qDebug() << e.what();
    }
}
