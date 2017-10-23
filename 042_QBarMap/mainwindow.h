#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QtAwesome;
class QLineEdit;
class QProgressBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotLoad();
    void slotUpdateProgress(int p);
    void slotViewPress(quint64 block);
    //
    void slotSelect();

private:
    Ui::MainWindow *ui;
    QtAwesome *awesome;
    //
    QAction *actLoad;
    //
    QAction *homeAction;
    QAction *backAction;
    QAction *selectAction;
    QAction *nextAction;
    QAction *endAction;
    //
    QLineEdit *editBlockNum;
    QProgressBar *progressitem;
};

#endif // MAINWINDOW_H
