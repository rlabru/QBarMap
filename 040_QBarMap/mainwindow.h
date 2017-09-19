#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotScan();
    void slotSwitchMapType(bool togg);
private:
    Ui::MainWindow *ui;
    QAction        *actScan;
    QAction        *actSwitchMapType;
};

#endif // MAINWINDOW_H
