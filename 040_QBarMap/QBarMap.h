#ifndef QBARMAP_H
#define QBARMAP_H

#include <QWidget>
#include <QQueue>


class QBarMap : public QWidget
{
    Q_OBJECT
public:
    explicit QBarMap(QWidget *parent = 0);
    ~QBarMap();
    void addResult(const QList< QPair<quint64, quint32> > &data);
    void clear(); // clear
    //
    enum map_type
    {
        mtBar        = 0,
        mtPixel      = 1
    };
    void setMapType(map_type t);
signals:
    void onDoubleClick(quint64 lba);
public slots:
    void slotUpdate(const QPair<quint64,quint32> &p);
protected:
    void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);

private slots:
    void paintGrid(QPainter &p);
    void paintUniverse(QPainter &p);

private:
    QQueue< QPair<quint64,quint32> > m_map;
    QStringList colorList;
    quint8 m_byGridDegree;
    quint8 RowAndColumnCount;
    double cellHeight;
    double cellWidth;
    void CutRow();
    void CutRowPixel();
    void EnsureCell();
    map_type mt;
};

#endif // QBARMAP_H
