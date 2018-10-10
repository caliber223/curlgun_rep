#ifndef CURLSET_H
#define CURLSET_H

#include <vector>
#include <string>
#include <cstdio>

class TestCurls {
public:
    TestCurls();
    ~TestCurls();

    void Clear();
    std::vector <std::string> GetCurls();
    void AddCurl(std::string &newcurl);
    size_t GetSize();
    bool SetSourseFile(std::string &sf);
    std::string GetSourseFile ();
    bool SetResponseFile(std::string &rf);
    std::string GetResponseFile();
    bool SetErrorFile(std::string &ef);
    std::string GetErrorFile();

private:
    std::vector <std::string> mCurls;
    std::string mSourseFile;
    std::string mResponseFile;
    std::string mErrorFile;
};

#endif // CURLSET_H

