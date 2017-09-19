#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QRectF>
#include <QPainter>
#include <qmath.h>
#include <QToolTip>
#include <QColor>

#include <QDebug>

#include "QBarMap.h"

QBarMap::QBarMap(QWidget *parent) :
    QWidget(parent),
    RowAndColumnCount(100),
    mt(mtBar)
{
    setMinimumSize(100,100);
    this->setMouseTracking(true);
    //            0       1        2          3         4        5        6           7
    colorList<<"Black"<<"Teal"<<"Yellow"<<"Fuchsia"<<"Maroon"<<"Red"<<"darkGreen"<<"Green";
    m_byGridDegree = 1;
}

QBarMap::~QBarMap()
{
}

void QBarMap::clear()
{
    m_map.clear();
    update();
}

void QBarMap::setMapType(QBarMap::map_type t)
{
    mt = t;
    update();
}

void QBarMap::slotUpdate(const QPair<quint64, quint32> &p)
{
    for (int i = 0; i < m_map.size(); ++i) { // search for item
        if(m_map.at(i).first == p.first) {
            // change item
            m_map[i].second = p.second;
            update();
            return;
        }
    }
    // case point for update not found
    m_map.append(p);
    update();
}

void QBarMap::addResult(const QList<QPair<quint64, quint32> > &data)
{
    m_map.append(data);
    switch (mt) {
    case mtBar:
        while (m_map.size() >= (RowAndColumnCount*(RowAndColumnCount-0)) ) {
            CutRow();
        }
        break;
    case mtPixel:
        while (m_map.size() >= (width()*height()) ) {
            CutRowPixel();
        }
    default:
        break;
    }
    update();
}

void QBarMap::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    paintUniverse(p);
    paintGrid(p);
}

void QBarMap::paintGrid(QPainter &p)
{
    EnsureCell();
    if(mt==mtBar) {
        p.setPen(QPen(QColor(colorList.at(0)),m_byGridDegree));
        for(int  i = 0; i < m_map.size(); i++) {
            p.drawLine((i%RowAndColumnCount)*cellWidth,(i/RowAndColumnCount+1)*cellHeight,(i%RowAndColumnCount+1)*cellWidth,(i/RowAndColumnCount+1)*cellHeight);

            p.drawLine((i%RowAndColumnCount+1)*cellWidth,(i/RowAndColumnCount)*cellHeight,(i%RowAndColumnCount+1)*cellWidth,(i/RowAndColumnCount+1)*cellHeight);
        }
    }
    QColor gridColor = QColor("#000000");//color of the border
    p.setPen(QPen(gridColor,1));
    p.drawLine(0,0,cellWidth*RowAndColumnCount,0);
    p.drawLine(0,cellHeight*RowAndColumnCount,cellWidth*RowAndColumnCount,cellHeight*RowAndColumnCount);
    p.drawLine(0,0,0,cellHeight*RowAndColumnCount);
    p.drawLine(cellWidth*RowAndColumnCount,0,cellWidth*RowAndColumnCount,cellHeight*RowAndColumnCount);
}

void QBarMap::paintUniverse(QPainter &p)
{
    for (int i = 0; i < m_map.size(); ++i) {
        switch (mt) {
        case mtBar: {
            qreal left = (qreal)((i % RowAndColumnCount)*cellWidth); // margin from left
            qreal top  = (qreal)((i / RowAndColumnCount)*cellHeight); // margin from top
            QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
            //
            QPair<quint64,quint32> var = m_map.at(i);
            if(var.second < static_cast<quint32>(colorList.size()) )
                p.fillRect(r, QBrush(QColor(colorList.at(var.second))));
            else
                p.fillRect(r, QBrush(QColor(Qt::white)));
            break; }
        case mtPixel: {
            QPair<quint64,quint32> var = m_map.at(i);
            p.setPen(QColor(colorList.at(var.second)));
            p.drawPoint(i % p.window().width(),i / p.window().width());
            break; }
        default:
            break;
        }
    }
}

void QBarMap::mouseMoveEvent(QMouseEvent *event)
{
    if( (m_map.size() != 0) && (mt == mtBar) ) { // prevent crash at no map filled
        QString strToolTip;
        int i,j,k;
        i = event->x()/cellWidth;
        j = event->y()/cellHeight;
        k = i+j*RowAndColumnCount;
        if(k < m_map.size()) {
            strToolTip.append(QString("LBA: %1").arg(m_map.at(k).first));
            QToolTip::showText(cursor().pos(),strToolTip,this);
        }
    }
    QWidget::mouseMoveEvent(event);
}

void QBarMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    if( (m_map.size() != 0) && (mt == mtBar) ) { // prevent crash at no map filled
        int i,j,k;
        i = event->x()/cellWidth;
        j = event->y()/cellHeight;
        k = i+j*RowAndColumnCount;
        if(k < m_map.size()) {
            emit onDoubleClick(m_map.at(k).first);
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void QBarMap::CutRow()
{
    for(int i=0; i < RowAndColumnCount * 10 ; i++) {
        m_map.pop_front();
    }
}

void QBarMap::CutRowPixel()
{
    for(int i=0; i < width() * 10 ; i++) {
        m_map.pop_front();
    }
}

void QBarMap::EnsureCell()
{
    cellWidth = (double)(width()-1)/RowAndColumnCount;
    cellHeight = (double)(height()-1)/RowAndColumnCount;
}
