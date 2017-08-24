#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //
    actScan = new QAction(QIcon(":/adept_update.png"),tr("Append sectors"),this);
    ui->mainToolBar->addAction(actScan);
    connect(actScan,SIGNAL(triggered()), this, SLOT(slotScan()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotScan()
{
    static quint32 up = 0;
    QPair<quint64, quint32> pa;
    QList< QPair<quint64, quint32> > listData;
    for(int i = 0; i < 256; ++i) {
        if( (up%37) == 0)
            pa.second = 5;
        else
            pa.second = 7;
        pa.first = up;
        listData.append(pa);
        up++;
    }
    ui->bmWidget->addResult(listData);
}
