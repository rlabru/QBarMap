#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QRectF>
#include <QPainter>
#include <qmath.h>
#include <QToolTip>
#include <QColor>
#include <QThread>

#include <QDebug>

#include "qbarmap.h"
#include "qimgfileloader.h"

QBarMap::QBarMap(QWidget *parent)
    : QWidget(parent)
    , cellHeight(10)
    , cellWidth(6)
    , scrollHeight(0)
    , scrollWidth(20)
    , mt(mtBar)
    , receiveMapFile(false)
    , needUpdate(true)
    , updatingMap(false)
    , totalBlocks(0)
    , currentLocation(0)
{
    setMinimumSize(cellWidth*128,cellHeight*32);
    this->setMouseTracking(true);
    //            0       1        2          3         4        5        6           7
    colorList<<"Black"<<"Teal"<<"Yellow"<<"Fuchsia"<<"Maroon"<<"Red"<<"darkGreen"<<"Green";
    //
    worker = new QThread(this);
    mapFileLoader = new QImgFileLoader(512,scrollWidth); // передаем размер блока и области прокрутки
    mapFileLoader->moveToThread(worker);
    connect(this,SIGNAL(loadFile(QString)),mapFileLoader,SLOT(loadFile(QString)));
    connect(mapFileLoader,SIGNAL(error(QString)),this,SLOT(slotError(QString)));
    connect(mapFileLoader,SIGNAL(loadStatus(int,quint64)),this,SLOT(slotImgFileStatus(int,quint64)));
    connect(this,SIGNAL(mapPage(quint64,quint32)),mapFileLoader,SLOT(makeMapPage(quint64,quint32)));
    connect(this,SIGNAL(mapView(quint32)),mapFileLoader,SLOT(makeMapView(quint32)));
    connect(mapFileLoader,SIGNAL(mapPage(QByteArray)),this,SLOT(slotReceivePage(QByteArray)));
    connect(mapFileLoader,SIGNAL(mapView(QByteArray)),this,SLOT(slotReceiveView(QByteArray)));
    worker->start();
}

QBarMap::~QBarMap()
{
    worker->quit();
    if(!worker->wait(10000)) qDebug()<<"mapFileLoader thread problem";
}

void QBarMap::clear()
{
    update();
}

void QBarMap::setLocation(quint64 block)
{
    currentLocation = block;
    update();
}

void QBarMap::setMapType(QBarMap::map_type t)
{
    mt = t;
    update();
}

/**
  Команда перехода в начало карты
*/
void QBarMap::home()
{
    currentLocation = 0;
    emit viewPress(currentLocation);
    update();
}

void QBarMap::backward()
{
    if(currentLocation >= static_cast<quint64>(cellCountX*cellCountY) ) {
        currentLocation -= cellCountX*cellCountY;
        emit viewPress(currentLocation);
        update();
    }
}

void QBarMap::forward()
{
    // переход вперед на размер отображения, который берется из прошлой отрисовки окна.
    if(currentLocation + cellCountX*cellCountY <= totalBlocks - cellCountX*cellCountY) {
        currentLocation += cellCountX*cellCountY;
        emit viewPress(currentLocation);
        update();
    }
}
/**
  Переход на финальный блок
*/
void QBarMap::end()
{
    currentLocation = totalBlocks - cellCountX*cellCountY;
}
/**
  Загрузка файла в отдельном потоке
*/
void QBarMap::setImageFile(const QString &f)
{
    receiveMapFile = false;
    emit loadFile(f);
}

void QBarMap::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(receiveMapFile) {
        wndWidth = width();
        cellCountX = (wndWidth-scrollWidth) / cellWidth;
        wndHeight = height();
        cellCountY = wndHeight / cellHeight;
        //
        if( (needUpdate==true)&&(updatingMap==false) ) {
            updatingMap = true;
            needUpdate = false;
            wndWidthLast = wndWidth;
            wndHeightLast = wndHeight;
            scrollHeight = wndHeight;
            cellCountXlast = cellCountX;
            cellCountYlast = cellCountY;
            emit mapPage(currentLocation,cellCountX*cellCountY);
        }
        if( (needUpdate==false)&&(updatingMap==false) ) { // тут выполняем отрисовку полученных карт
            // проверить, не изменились ли параметры отображения с момента
            // посылки запроса?
            if( (wndWidthLast == wndWidth)&&(wndHeightLast == wndHeight) ) {
                QPainter p(this);
                for(int y = 0; y < cellCountY; ++y)
                    for(int x = 0; x < cellCountX; ++x) {
                        qreal left = (qreal)( x * cellWidth); // margin from left
                        qreal top  = (qreal)( y * cellHeight); // margin from top
                        QRectF r(left, top, (qreal)(cellWidth-1), (qreal)(cellHeight-1));
                        if(currentPage[y][x]==0) p.fillRect(r, QBrush(QColor(Qt::darkBlue)));
                        else p.fillRect(r, QBrush(QColor(Qt::darkGreen)));
                    }
                //
                int scrollBegin = width()-scrollWidth;
                for(quint32 y = 0; y < scrollHeight; ++y)
                    for(quint32 x = 0; x < scrollWidth; ++x) {
                        if(baMapView.at( (y*scrollWidth) + x ) == 0)
                            p.setPen(Qt::darkBlue);
                        else
                            p.setPen(Qt::darkGreen);
                        p.drawPoint(scrollBegin+1+x,y);
                    }
                needUpdate = true; // завершили полностью запрос на построение и нарисовали его
            } else { // запрашиваем новые параметры отрисовки карты в случае изменения размера окна пока думали...
                updatingMap = true;
                needUpdate = false;
                wndWidthLast = wndWidth;
                wndHeightLast = wndHeight;
                cellCountXlast = cellCountX;
                cellCountYlast = cellCountY;
                emit mapPage(currentLocation,cellCountX*cellCountY);
            }
        }
    }
}
/**
  отображение процентов в статусе
*/
void QBarMap::slotImgFileStatus(int p, quint64 blocks)
{
    emit mapLoadPercent(p);
    if(p<0) {
        receiveMapFile = true; // загрузка файла карты завершена
        needUpdate = true;
        updatingMap = false;
        totalBlocks = blocks;
        update();
    }
}
/**
    Функция, которая по данным: начальное положение, размер окна x , размер окна y
    вернет заполненный QVector< QVector <quint8> > по полученным
    из потока mapFileLoader данными.
*/
void QBarMap::slotReceivePage(const QByteArray &map)
{
    emit mapView(scrollHeight); // запускаем построение
    currentPage.clear();
    for(int y = 0; y < cellCountYlast; ++y) {
        QVector <quint8> vtmp;
        for(int x = 0; x < cellCountXlast; ++x) {
            // заполнить вектор кодами цвета в соответствии с содержимым сектора
            vtmp.push_back( static_cast<quint8>(map.at(y*cellCountXlast + x)) );
        }
        currentPage.append(vtmp);
    }
}

void QBarMap::slotReceiveView(const QByteArray &map)
{
    baMapView = map;
    updatingMap = false; // установить сигнал получения запроса построения карты
    update();
}

void QBarMap::slotError(const QString &e)
{
    updatingMap = true;
    needUpdate = false;
    qDebug()<<e;
}

void QBarMap::mouseMoveEvent(QMouseEvent *event)
{
    if(receiveMapFile) {
        QString strToolTip;
        int i,j,k;
        i = event->x()/cellWidth;
        if(i<cellCountX) {
            j = event->y()/cellHeight;
            k = i + j * cellCountY;
            strToolTip.append(QString("Block: %1").arg(currentLocation + k));
            QToolTip::showText(cursor().pos(),strToolTip,this);
        } else if( event->x() >= width()-static_cast<int>(scrollWidth) ) {
            i = event->x()+scrollWidth-width();
            k = event->y()*scrollWidth + i;
            quint32 u = scrollWidth*height();
            quint64 l = (totalBlocks / u)*k;
            strToolTip.append(QString("Block: %1").arg(l));
            QToolTip::showText(cursor().pos(),strToolTip,this);
        }
    }
    QWidget::mouseMoveEvent(event);
}

void QBarMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void QBarMap::mousePressEvent(QMouseEvent *event)
{
    if( event->x() > width()-static_cast<int>(scrollWidth) ) {
        quint32 k = event->y()*scrollWidth + event->x()+scrollWidth-width();
        quint32 u = scrollWidth*height();
        currentLocation = (totalBlocks / u)*k;
        emit viewPress(currentLocation);
        update();
    }
    QWidget::mousePressEvent(event);
}

