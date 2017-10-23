#ifndef QBARMAP_H
#define QBARMAP_H

#include <QWidget>
#include <QVector>

QT_BEGIN_NAMESPACE
class QThread;
class QImgFileLoader;
QT_END_NAMESPACE

/**
\brief визуальное отображение информационного пространства носителя информации

Класс позволяет отобразить в виде набора прямоугольников блоки по 512 байт из
загруженного файла. Если блок состоит из нулей, то отобразится синий прямоугольник,
если есть хотя бы один не нулевой байт, то зеленый.

Планы на будущее:
- перенести тулбар управления сюда, по типу как в хекс редакторе
- как получить дефолтный цвет фона?
- отображать треугольником текущее положение на виде сверху
- добавить реакцию на колесо прокрутки мыши
- по одному клику в цепочку секторов сделать выделение непрерывной цепочки одного типа

\authors Sergey Yatsenko
\date 2017.10.23
*/
class QBarMap : public QWidget
{
    Q_OBJECT
public:
    explicit QBarMap(QWidget *parent = 0);
    ~QBarMap();
    void setImageFile(const QString &f);
    void clear(); // clear
    void setLocation(quint64 block);
    /*! перечисление типов отображения карты */
    enum map_type
    {
        /// отображение в виде прямоугольников
        mtBar        = 0,
        /// отображение в виде точек
        mtPixel      = 1
    };
    /*! в данной версии отключено */
    void setMapType(map_type t);
signals:
    void onDoubleClick(quint64 lba);
    void mapLoadPercent(int p);
    void loadFile(const QString &file);
    void mapPage(quint64 begin, quint32 pageSize);
    void mapView(quint32 scrollHeight);
    void viewPress(quint64 block);

public slots:
    void home();
    void backward();
    void forward();
    void end();

protected:
    void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);

private slots:
    void slotImgFileStatus(int p, quint64 blocks);
    void slotReceivePage(const QByteArray &map);
    void slotReceiveView(const QByteArray &map);
    void slotError(const QString& e);

private:
    quint32 cellHeight;
    quint32 cellWidth;
    quint32 scrollHeight;
    quint32 scrollWidth;
    QStringList colorList;
    map_type mt;
    QString fileImage;
    QThread *worker;
    QImgFileLoader *mapFileLoader;
    bool receiveMapFile;
    bool needUpdate;
    bool updatingMap;
    quint64 totalBlocks; // копия общего количества блоков приходящая из slotImgFileStatus
    quint64 currentLocation; // top left block address in map
    QVector< QVector <quint8> > currentPage;
    int cellCountX;
    int cellCountY;
    int cellCountXlast;
    int cellCountYlast;
    int wndHeight;
    int wndWidth;
    int wndHeightLast;
    int wndWidthLast;
    QByteArray baMapView;
};

#endif // QBARMAP_H
