#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLCDNumber>
#include <QTableWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void downButtonLoad();
    void downButtonClear();
    bool downButtonStart();
    bool downButtonAdd();

private:
    QLabel        *plb1;
    QLabel        *plb2;
    QLineEdit     *ple1;
    QLineEdit     *ple2;
    QPushButton   *ppb1;
    QPushButton   *ppb2;
    QPushButton   *ppb3;
    QPushButton   *ppb4;
    QTableWidget  *table1;
    QLCDNumber    *lcdnumber1;
};

#endif // MAINWINDOW_H