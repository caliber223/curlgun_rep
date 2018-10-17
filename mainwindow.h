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
    bool setResponseText(std::string &newtext);

private slots:
    void downButtonLoad();
    void downButtonClear();
    bool downButtonStart();
    bool downButtonAdd();

private:
    QLabel        *plb1;
    QLabel        *plb2;
    QLabel        *plb3;  // passed
    QLabel        *plb4;  // failed
    QLabel        *plb5;  // selected
    QLabel        *plb6;  // keywords
    QTextEdit     *responseWindow;
    QLineEdit     *ple1;
    QLineEdit     *ple2;
    QLineEdit     *keys;  // keywords
    QPushButton   *ppb1;
    QPushButton   *ppb2;
    QPushButton   *ppb3;
    QPushButton   *ppb4;
    QTableWidget  *table1;
    QLCDNumber    *lcdnumber1;  // failed
    QLCDNumber    *lcdnumber2;  // passed
    QLCDNumber    *lcdnumber3;  // selected

};

int curlstart(const std::string &inUrl, std::string &inProxy, int inHeader, const std::string inKeys, MainWindow *obj);
bool checkSelect(const std::string &inStr, const std::string &selected);

#endif // MAINWINDOW_H
