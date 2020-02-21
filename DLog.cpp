#include "DLog.hpp"
#include <cstdarg>
#include <memory>
#include <iostream>
#include <cstdio>
#include <chrono>
#include <iomanip>

using namespace std;

/*! \mainpage DLog class
*
* \section intro_sec Intro
*
*
*
* \section futures_sec Futures
**/

// TODO ParsedPrint()
/*
    for (int ixP=0;ixP < len; ixP++) {
        if (isprint(buff[ixP])) putchar(buff[ixP]);
        else putchar('.');
    }
    */

/**
* Create the log object
* @params LogFilename   ->  filename of a destination log file, if empty no file is crated.
* @params StdoutEnabled ->  if true al leg messages are output on stdout or stderr
**/
DLog::DLog(string LogFilename, bool StdoutEnabled)
{
    LogToStdout=StdoutEnabled;
    //outStream=NULL;
    Filename=LogFilename;
    if (Filename == "") {
        hFile=nullptr;
        LogToFile=false;
    }
    else {
        hFile=fopen(Filename.c_str(),"aw+");
        if (hFile == nullptr) {
            perror("Log file not opened");
            LogToFile=false;
            StdoutEnabled=true; // se non posso loggare su file uso comunque lo stdout
        }
        else {
            LogToFile=true;
        }
    }
}

DLog::~DLog()
{
    if (hFile != nullptr) fclose(hFile);
}

/*
void DLog::SetAdditionalStream(ostringstream *outputStream)
{
    outStream=outputStream;
}
*/

std::string DLog::Read(void)
{
    fseek(hFile,0,SEEK_END);
    long int fSize=ftell(hFile);
    std::string Buff(fSize,' ');
    fseek(hFile,0,SEEK_SET);
    fread((char*)Buff.c_str(),fSize,1,hFile);
    fseek(hFile,0,SEEK_END);

    return(Buff);
}

void DLog::d(const std::string fmt,...)
{
    va_list args;
    va_start(args,fmt);
    size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
    unique_ptr<char[]> msg(new char[len+1]);
    vsprintf(msg.get(),fmt.c_str(),args);
    Write(DL_LEVEL_DEBUG,msg.get());
    va_end(args);

}

void DLog::i(const std::string fmt,...)
{
    va_list args;
    va_start(args,fmt);
    size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
    unique_ptr<char[]> msg(new char[len+1]);
    vsprintf(msg.get(),fmt.c_str(),args);
    Write(DL_LEVEL_INFO,msg.get());
    va_end(args);

}

void DLog::e(const std::string fmt,...)
{
    va_list args;
    va_start(args,fmt);
    size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
    unique_ptr<char[]> msg(new char[len+1]);
    vsprintf(msg.get(),fmt.c_str(),args);
    Write(DL_LEVEL_ERROR,msg.get());
    va_end(args);

}

void DLog::w(const std::string fmt,...)
{
    va_list args;
    va_start(args,fmt);
    size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
    unique_ptr<char[]> msg(new char[len+1]);
    vsprintf(msg.get(),fmt.c_str(),args);
    Write(DL_LEVEL_WARNING,msg.get());
    va_end(args);

}

void DLog::Write(unsigned char Level,string LogMsg)
{
    auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << put_time(localtime(&now_time_t),"%Y/%m/%d %H.%M.%S : ");
    string HdrMsg=ss.str();
	/*
    time_t now=time(0);
    tm *ltm=localtime(&now);
    string HdrMsg=format_string("%.04d/%.02d/%.02d %.02d.%.02d.%.02d : ",ltm->tm_year+1900,ltm->tm_mon+1,ltm->tm_mday,ltm->tm_hour+1,ltm->tm_min+1,ltm->tm_sec+1);
    */
    string Color;
    string LevelMsg;

    // Debug level
    switch (Level) {
        case DL_LEVEL_DEBUG:
            LevelMsg+="DEBUG   : ";
            Color=CL_MAGENTA;
            break;
        case DL_LEVEL_INFO:
            LevelMsg+="INFO    : ";
            Color=CL_DEFAULT;
            break;
        case DL_LEVEL_ERROR:
            LevelMsg+="ERROR   : ";
            Color=CL_RED;
            break;
        case DL_LEVEL_WARNING:
            LevelMsg+="WARNING : ";
            Color=CL_YELLOW;
            break;
        default:
            break;
    }

    //Msg+=LogMsg+CL_DEFAULT+"\r\n";

    // Print log message
    if (LogToFile) {
        fprintf(hFile,(HdrMsg+LevelMsg+LogMsg+"\r").c_str());
    }
    if (LogToStdout) {
        if (Level == DL_LEVEL_ERROR)  {
            perror((HdrMsg+Color+LevelMsg+LogMsg+CL_DEFAULT+"\r").c_str());
        }
        else {
            //cout << HdrMsg << Color << LevelMsg << LogMsg << CL_DEFAULT << "\r";
            printf((HdrMsg+Color+LevelMsg+LogMsg+CL_DEFAULT+"\r").c_str());
        }
    }

    /*
    if (outStream != NULL) {
        *outStream << HdrMsg << LevelMsg << LogMsg;
    }
    */

    fflush(hFile); // Scrivi subito
}

template<typename ... Args>
string DLog::format_string(const string& format,Args ... args)
{
    size_t len = snprintf(nullptr,0,format.c_str(),args ...)+1; // Extra space for '\0'
    unique_ptr<char[]> buf(new char[len]);
    snprintf(buf.get(),len, format.c_str(),args ...);
    return string(buf.get(),buf.get()+len-1); // We don't want the '\0' inside
}
