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

int curlstart(const std::string &inUrl, std::string &inProxy, int inHeader, const std::string inKeys, MainWindow *obj) {
    int state = 0;
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
            state = 1;
            std::ofstream fout(curls.GetResponseFile().c_str(), std::ios_base::app);
            fout <<buffer <<std::endl;
            fout.close();
            if(!inKeys.empty() && checkSelect(buffer, inKeys)) {
                state = 2;
                std::ofstream fselect(curls.GetSelectedFile().c_str(), std::ios_base::app);
                fselect <<std::endl <<buffer <<std::endl;
                fselect.close();
            }
            obj->setResponseText(buffer);
            buffer.clear();
        } else {
            state = 3;
            std::ofstream fout(curls.GetErrorFile().c_str(), std::ios_base::app);
            fout <<errorBuffer <<std::endl;
            fout.close();
            std::string eb = errorBuffer;
            if(!inKeys.empty() && checkSelect(eb, inKeys)) {
                state = 4;
                std::ofstream fselect(curls.GetSelectedFile().c_str(), std::ios_base::app);
                fselect <<std::endl <<eb <<std::endl;
                fselect.close();
            }
            obj->setResponseText(eb);
        //    errorBuffer = "";
        }
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return state;
}

bool checkSelect(const std::string &inStr, const std::string &selected) {
    if(inStr.empty() || selected.empty()) {
        return false;
    }
    // in "selected"  " " (space) - is separator
    std::vector <std::string> items;
    std::string str_tail = selected;
    const char separator = ' ';
    while(!str_tail.empty()) {
        size_t pos = 0;
        if((pos = str_tail.find(separator)) != std::string::npos) {
            items.push_back(str_tail.substr(0, pos));
            str_tail = str_tail.substr(pos + 1, std::string::npos);
            while(0 == str_tail.find(separator)) {
                str_tail = str_tail.substr(1, std::string::npos);
            }
        } else {
            items.push_back(str_tail);
            str_tail = "";
        }
    }
    for(size_t i = 0; i < items.size(); ++i) {
        if(inStr.find(items[i]) != std::string::npos) {
            return true;
        }
    }

    return false;
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
    plb2->setGeometry(40, 55, 80, 24);
    plb2->setStyleSheet("color: lightBlue");

    plb3 = new QLabel(tr("Passed:"), this);
    plb3->setGeometry(40, 125, 80, 24);
    plb3->setStyleSheet("color: lightBlue");

    plb4 = new QLabel(tr("Failed:"), this);
    plb4->setGeometry(350, 125, 40, 24);
    plb4->setStyleSheet("color: lightBlue");

    plb5 = new QLabel(tr("Selected:"), this);
    plb5->setGeometry(660, 125, 60, 24);
    plb5->setStyleSheet("color: lightBlue");

    plb6 = new QLabel(tr("Keywords:"), this);
    plb6->setGeometry(40, 90, 80, 24);
    plb6->setStyleSheet("color: lightBlue");

    ple1 = new QLineEdit(this);
    ple1->setGeometry(120, 20, 600, 24);
    ple1->setStyleSheet("background-color: white");

    ple2 = new QLineEdit(this);
    ple2->setGeometry(120, 55, 600, 24);
    ple2->setStyleSheet("background-color: white");

    keys = new QLineEdit(this);
    keys->setGeometry(120, 90, 600, 24);
    keys->setStyleSheet("background-color: white");

    ppb1 = new QPushButton(tr("Load"), this);
    ppb1->setGeometry(15, 200, 90, 24);
    ppb1->setStyleSheet("background-color: lightGray");
    connect(ppb1, SIGNAL(clicked(bool)), this, SLOT(downButtonLoad()));

    ppb2 = new QPushButton(tr("Clear URLs"), this);
    ppb2->setGeometry(15, 240, 90, 24);
    ppb2->setStyleSheet("background-color: lightGray");
    connect(ppb2, SIGNAL(clicked(bool)), this, SLOT(downButtonClear()));

    ppb3 = new QPushButton(tr("Start"), this);
    ppb3->setGeometry(15, 340, 90, 24);
    ppb3->setStyleSheet("background-color: lightGray");
    connect(ppb3, SIGNAL(clicked(bool)), this, SLOT(downButtonStart()));

    ppb4 = new QPushButton(tr("Add"), this);
    ppb4->setGeometry(740, 55, 40, 24);
    ppb4->setStyleSheet("background-color: lightGray");
    connect(ppb4, SIGNAL(clicked(bool)), this, SLOT(downButtonAdd()));

    clearAllButton = new QPushButton(tr("Clear results"), this);
    clearAllButton->setGeometry(15, 280, 90, 24);
    clearAllButton->setStyleSheet("background-color: lightGray");
    connect(clearAllButton, SIGNAL(clicked(bool)), this, SLOT(downAllClearButton()));

    table1 = new QTableWidget(this);
    table1->setGeometry(120, 170, 860, 300);
    table1->setStyleSheet("background-color: lightGray");
    table1->setShowGrid(true);
    table1->setRowCount(1);
    table1->setColumnCount(2);

    lcdnumber1 = new QLCDNumber(this);
    lcdnumber1->setGeometry(120, 125, 180, 32);
    lcdnumber1->setStyleSheet("background-color: lightGray");

    lcdnumber2 = new QLCDNumber(this);
    lcdnumber2->setGeometry(420, 125, 180, 32);
    lcdnumber2->setStyleSheet("background-color: lightGray");

    lcdnumber3 = new QLCDNumber(this);
    lcdnumber3->setGeometry(740, 125, 180, 32);
    lcdnumber3->setStyleSheet("background-color: lightGray");

    responseWindow = new QTextEdit(this);
    responseWindow->setGeometry(120, 480, 860, 250);
    responseWindow->setStyleSheet("background-color: lightGray");

    alltogether = new QCheckBox(tr("All together"), this);
    alltogether->setGeometry(740, 90, 120, 24);
    alltogether->setStyleSheet("background-color: white");

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

bool MainWindow::downAllClearButton() {
    std::ofstream fout(curls.GetResponseFile().c_str(), std::ios_base::trunc);
    fout.close();
    fout.open(curls.GetErrorFile().c_str(), std::ios_base::trunc);
    fout.close();
    fout.open(curls.GetSelectedFile().c_str(), std::ios_base::trunc);
    fout.close();
    return true;
}

bool MainWindow::downButtonStart() {
    int state = 0;
    std::string outUrl;
    std::string outProxy = "";
    int outHeader = 1;
    for(size_t i = 0; i < curls.GetSize(); ++i) {
        outUrl = curls.GetCurls()[i];
        responseWindow->clear();
        state = curlstart(outUrl, outProxy, outHeader, keys->text().toStdString(), this);
        switch (state) {
        case 1:
            lcdnumber1->display(lcdnumber1->intValue() + 1);
            break;
        case 2:
            lcdnumber1->display(lcdnumber1->intValue() + 1);
            lcdnumber3->display(lcdnumber3->intValue() + 1);
            break;
        case 3:
            lcdnumber2->display(lcdnumber2->intValue() + 1);
            break;
        case 4:
            lcdnumber2->display(lcdnumber2->intValue() + 1);
            lcdnumber3->display(lcdnumber3->intValue() + 1);
            break;
        default:
            break;
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
