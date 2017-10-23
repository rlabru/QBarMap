#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QLineEdit>
#include <QProgressBar>

#include <QDebug>

#include "QtAwesome.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("042_QBarMap"));
    awesome = new QtAwesome(this);
    awesome->initFontAwesome();
    //
    actLoad = new QAction(QIcon(":/load.png"),tr("Load image file"),this);
    ui->mainToolBar->addAction(actLoad);
    connect(actLoad,SIGNAL(triggered()), this, SLOT(slotLoad()));
    ui->mainToolBar->addSeparator();
    //
    homeAction = new QAction(awesome->icon(fa::stepbackward), tr("home"),this);
    connect(homeAction, SIGNAL(triggered()), ui->bmWidget, SLOT(home()) );
    ui->mainToolBar->addAction(homeAction);
    //
    backAction = new QAction(awesome->icon(fa::backward), tr("backward"),this);
    connect(backAction, SIGNAL(triggered()), ui->bmWidget, SLOT(backward()));
    ui->mainToolBar->addAction(backAction);
    //
    editBlockNum = new QLineEdit();
    editBlockNum->setMaximumWidth(100);
    editBlockNum->setMinimumWidth(100);
    editBlockNum->setText("0");
    // todo: сделать строку ввода только положительной
    ui->mainToolBar->addWidget(editBlockNum);
    //
    selectAction = new QAction(awesome->icon(fa::th), tr("select"),this);
    connect(selectAction, SIGNAL(triggered()), this, SLOT(slotSelect()));
    ui->mainToolBar->addAction(selectAction);
    //
    nextAction = new QAction(awesome->icon(fa::forward), tr("forward"),this);
    connect(nextAction, SIGNAL(triggered()), ui->bmWidget, SLOT(forward()));
    ui->mainToolBar->addAction(nextAction);
    //
    endAction = new QAction(awesome->icon(fa::stepforward), tr("end"),this);
    connect(endAction, SIGNAL(triggered()), ui->bmWidget, SLOT(end()));
    ui->mainToolBar->addAction(endAction);
    ui->mainToolBar->addSeparator();
    //
    progressitem = new QProgressBar(this);
    progressitem->setRange(0,100);
    progressitem->setTextVisible(true);
    progressitem->setMaximumWidth(200);
    ui->mainToolBar->addWidget(progressitem);
    connect(ui->bmWidget,SIGNAL(mapLoadPercent(int)),this,SLOT(slotUpdateProgress(int)));
    //
    connect(ui->bmWidget,SIGNAL(viewPress(quint64)),this,SLOT(slotViewPress(quint64)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotLoad()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open image file"),"f:/2GB_img",tr("IMG Files (*.img)"));
    if (fileName.isEmpty())
        return;
    ui->bmWidget->setImageFile(fileName);
}

void MainWindow::slotUpdateProgress(int p)
{
    if(p < 0) progressitem->reset();
    else progressitem->setValue(p);
}

void MainWindow::slotViewPress(quint64 block)
{
    editBlockNum->setText(QString::number(block));
}

void MainWindow::slotSelect()
{
    qlonglong a = editBlockNum->text().toLongLong();
    ui->bmWidget->setLocation(a);
}

