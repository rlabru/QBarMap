#ifndef QIMGFILELOADER_H
#define QIMGFILELOADER_H

#include <QObject>
/**
\brief загрузка и обработка файла в отдельном потоке


\authors Sergey Yatsenko
\date 2017.10.23
*/
class QImgFileLoader : public QObject
{
    Q_OBJECT
public:
    /** в качестве параметров передается размер блока и размер отображения полосы прокрутки */
    explicit QImgFileLoader(quint32 blkSz, quint32 scrWdh, QObject *parent = 0);
    ~QImgFileLoader();

signals:
    /// произошла ошибка
    void error(const QString& e);
    /// статус загрузки файла в процентах и количество блоков
    void loadStatus(int percent, quint64 blocks);
    /// отдаем данные страницы
    void mapPage(const QByteArray &map);
    /// отдаем данные вида сверху
    void mapView(const QByteArray &map);
public slots:
    /// запрос на загрузку файла, требуется название и полный путь
    void loadFile(const QString &imgFile);
    /// запрос на получение данных для отображения карты
    void makeMapPage(quint64 begin, quint32 pageSize);
    /// запрос на получение данных вида сверху
    void makeMapView(quint32 scrollHeight);
private:
    quint8* ptrMap;
    quint64 totalBlocks; // общее число блоков в файле
    int blockSize;       // размер блока в байтах
    quint32 scrollWidth;
    quint8 checkZero(const QByteArray& b);
};

#endif // QIMGFILELOADER_H
