#include <string.h>
#include <stdio.h>
#include <fstream>
#include <curl/curl.h>
#include "mainwindow.h"
#include "curlset.h"
#include "QColor"

TestCurls curls;
static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer){
    int result = 0;
    if(buffer != NULL) {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
    return result;
}

bool curlstart(std::string &inUrl, std::string &inProxy, int inHeader, MainWindow *obj) {
    bool state = true;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl;
    CURLcode result;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, inUrl.c_str());
   //     curl_easy_setopt(curl, CURLOPT_PROXY, inProxy.c_str());
        inProxy = "";  // for kill warning
        curl_easy_setopt(curl, CURLOPT_HEADER, inHeader);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        result = curl_easy_perform(curl);
        if(result == CURLE_OK) {
            state = true;
            std::ofstream fout(curls.GetResponseFile().c_str(), std::ios_base::app);
            fout <<buffer <<std::endl;
            fout.close();
            obj->setResponseText(buffer);
            buffer.clear();
        } else {
            state = false;
            std::ofstream fout(curls.GetErrorFile().c_str(), std::ios_base::app);
            fout <<errorBuffer <<std::endl;
            fout.close();
            std::string eb = errorBuffer;
            obj->setResponseText(eb);
        //    errorBuffer = "";
        }
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return state;
}

bool checkSelect(std::string &inStr, std::string &selected) {
    if(inStr.empty()) {
        return false;
    }
    std::vector <std::string> items;

    return true;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle(tr("curlgun v.0.1"));
    setMinimumSize(1000, 740);
    setStyleSheet("background-color: rgb(50, 50, 50)");

    plb1 = new QLabel(tr("Sourse file:"), this);
    plb1->setGeometry(40, 20, 80, 24);
    plb1->setStyleSheet("color: lightBlue");

    plb2 = new QLabel(tr("Add URL:"), this);
    plb2->setGeometry(40, 370, 80, 24);
    plb2->setStyleSheet("color: lightBlue");

    plb3 = new QLabel(tr("Passed:"), this);
    plb3->setGeometry(40, 410, 80, 24);
    plb3->setStyleSheet("color: lightBlue");

    plb4 = new QLabel(tr("Failed:"), this);
    plb4->setGeometry(350, 410, 40, 24);
    plb4->setStyleSheet("color: lightBlue");

    ple1 = new QLineEdit(this);
    ple1->setGeometry(120, 20, 600, 24);
    ple1->setStyleSheet("background-color: white");

    ple2 = new QLineEdit(this);
    ple2->setGeometry(120, 370, 600, 24);
    ple2->setStyleSheet("background-color: white");

    ppb1 = new QPushButton(tr("Load"), this);
    ppb1->setGeometry(20, 50, 80, 24);
    ppb1->setStyleSheet("background-color: lightGray");
    connect(ppb1, SIGNAL(clicked(bool)), this, SLOT(downButtonLoad()));

    ppb2 = new QPushButton(tr("Clear"), this);
    ppb2->setGeometry(20, 90, 80, 24);
    ppb2->setStyleSheet("background-color: lightGray");
    connect(ppb2, SIGNAL(clicked(bool)), this, SLOT(downButtonClear()));

    ppb3 = new QPushButton(tr("Start"), this);
    ppb3->setGeometry(20, 144, 80, 24);
    ppb3->setStyleSheet("background-color: lightGray");
    connect(ppb3, SIGNAL(clicked(bool)), this, SLOT(downButtonStart()));

    ppb4 = new QPushButton(tr("Add"), this);
    ppb4->setGeometry(730, 370, 40, 24);
    ppb4->setStyleSheet("background-color: lightGray");
    connect(ppb4, SIGNAL(clicked(bool)), this, SLOT(downButtonAdd()));

    table1 = new QTableWidget(this);
    table1->setGeometry(120, 60, 860, 300);
    table1->setStyleSheet("background-color: lightGray");
    table1->setShowGrid(true);
    table1->setRowCount(1);
    table1->setColumnCount(2);

    lcdnumber1 = new QLCDNumber(this);
    lcdnumber1->setGeometry(120, 410, 180, 32);
    lcdnumber1->setStyleSheet("background-color: lightGray");

    lcdnumber2 = new QLCDNumber(this);
    lcdnumber2->setGeometry(420, 410, 180, 32);
    lcdnumber2->setStyleSheet("background-color: lightGray");

    responseWindow = new QTextEdit(this);
    responseWindow->setGeometry(120, 460, 860, 250);
    responseWindow->setStyleSheet("background-color: lightGray");

}

void MainWindow::downButtonLoad() {
    std::string path = ple1->text().toStdString();
    curls.SetSourseFile(path);
    std::ifstream fin(curls.GetSourseFile().c_str(), std::ios_base::in);
    std::string newcurl;
    while(std::getline(fin, newcurl)) {
        if(newcurl.empty() || newcurl.size() < 3) {
            continue;
        }
        curls.AddCurl(newcurl);
    }
    fin.close();
 //   lcdnumber1->display(int(curls.GetSize()));
    table1->setRowCount(int(curls.GetSize()));

    for (size_t i = 0; i < curls.GetSize(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(curls.GetCurls()[i]));
     //   item->setTextAlignment(Qt::AlignCenter);
        table1->setItem(i, 1, item);
    }
}

void MainWindow::downButtonClear() {
    curls.Clear();
    table1->clearContents();
    table1->setRowCount(1);
}

bool MainWindow::downButtonStart() {
    bool state = true;
    std::string outUrl;
    std::string outProxy = "";
    int outHeader = 1;
    for(size_t i = 0; i < curls.GetSize(); ++i) {
        outUrl = curls.GetCurls()[i];
        responseWindow->clear();
        state = curlstart(outUrl, outProxy, outHeader, this);
        if(state) {
            lcdnumber1->display(lcdnumber1->intValue() + 1);
        } else {
            lcdnumber2->display(lcdnumber2->intValue() + 1);
        }
    }
    return true;
}

bool MainWindow::downButtonAdd() {
    if(ple2->text().isEmpty() || ple2->text().size() < 3) {
        return false;
    }

    table1->setRowCount(table1->rowCount() + 1);
    std::string newcurl = ple2->text().toStdString();
    curls.AddCurl(newcurl);
    QTableWidgetItem *item = new QTableWidgetItem(ple2->text());
    table1->setItem(table1->rowCount() - 1, 1, item);

    return true;
}

bool MainWindow::setResponseText(std::string &newtext) {
    if(newtext.empty()) {
        return false;
    }
    MainWindow::responseWindow->setText(QString::fromStdString(newtext));
    return true;
}

MainWindow::~MainWindow()
{

}
