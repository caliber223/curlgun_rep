// curlset.cpp
#include "curlset.h"

TestCurls::TestCurls() {
    mResponseFile = "response.log";
    mErrorFile = "error.log";
    mSourseFile = "curls.dat";
}

TestCurls::~TestCurls() {
}

void TestCurls::Clear() {
    mCurls.clear();
}

std::vector <std::string> TestCurls::GetCurls() {
    return mCurls;
}

void TestCurls::AddCurl(std::string &newcurl) {
    mCurls.push_back(newcurl);
}

size_t TestCurls::GetSize() {
    return mCurls.size();
}

std::string TestCurls::GetSourseFile() {
    return mSourseFile;
}

bool TestCurls::SetSourseFile (std::string &sf) {
    if(sf.empty()) {
        return false;
    }
    mSourseFile = sf;
    return true;
}

std::string TestCurls::GetResponseFile() {
    return mResponseFile;
}

bool TestCurls::SetResponseFile(std::string &rf) {
    if(rf.empty()) {
        return false;
    }
    mResponseFile = rf;
    return true;
}

std::string TestCurls::GetErrorFile() {
    return mErrorFile;
}

bool TestCurls::SetErrorFile(std::string &ef) {
    if(ef.empty()) {
        return false;
    }
    mErrorFile = ef;
    return true;
}
