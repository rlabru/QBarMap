#include "qimgfileloader.h"

#include <QFile>

#include <QDebug>

QImgFileLoader::QImgFileLoader(quint32 blkSz, quint32 scrWdh, QObject *parent)
    : QObject(parent)
    , ptrMap(NULL)
    , totalBlocks(0)
    , blockSize(blkSz)
    , scrollWidth(scrWdh)
{

}

QImgFileLoader::~QImgFileLoader()
{
    if(ptrMap) delete ptrMap;
}

void QImgFileLoader::loadFile(const QString &imgFile)
{
    QFile dataFile(imgFile);
    if(!dataFile.open(QIODevice::ReadOnly)) { // открыть файл
        qWarning()<<"can't open:"<<imgFile;
    } else {
        quint64 fileSize = dataFile.size();
        totalBlocks = fileSize / blockSize;
        if(ptrMap) delete ptrMap;
        ptrMap = new quint8[totalBlocks];
        QByteArray ba;
        for(quint64 i = 0; i < totalBlocks; ++i) {
            dataFile.seek(i*blockSize);
            ba = dataFile.read(blockSize);
            ptrMap[i] = checkZero(ba);
            //
            int pcnt = (i*100) / totalBlocks;
            emit loadStatus(pcnt,0);
        }
        dataFile.close();
        emit loadStatus(-1,totalBlocks);
    }
}

void QImgFileLoader::makeMapPage(quint64 begin, quint32 pageSize)
{
    if(begin+pageSize > totalBlocks) emit error("Input size over current map.");
    else {
        emit mapPage(QByteArray::fromRawData(reinterpret_cast<char*>(ptrMap)+begin,pageSize));
    }
}

void QImgFileLoader::makeMapView(quint32 scrollHeight)
{
    quint32 totalScrollPixels = scrollWidth*scrollHeight;
//        qDebug()<<"TSP="<<totalScrollPixels;
    QByteArray scrollMap(totalScrollPixels,0);
    quint32 blocksPerScrollPixel = (totalBlocks / totalScrollPixels)+1; //? fix last row ?
//        qDebug()<<"BPSP="<<blocksPerScrollPixel;
    quint64 greenCnt = 0;
    quint64 blueCnt = 0;
    for(quint64 i = 0; i < totalBlocks; ++i) {
        quint32 p = i%blocksPerScrollPixel;
        if(ptrMap[i]==0) blueCnt++;
        else greenCnt++;
        if(p==(blocksPerScrollPixel-1)) {
//               qDebug()<<"P="<<i/blocksPerScrollPixel;
            if(blueCnt<greenCnt)
                scrollMap.insert(i/blocksPerScrollPixel, 1);
            greenCnt = 0;
            blueCnt = 0;
        }
    }
    emit mapView(scrollMap);
}

quint8 QImgFileLoader::checkZero(const QByteArray &b)
{
    for(int i = 0; i < blockSize; ++i) {
        if( b.at(i) != 0 ) return 1;
    }
    return 0;
}
