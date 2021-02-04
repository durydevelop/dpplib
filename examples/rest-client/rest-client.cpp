#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <boost/beast/core.hpp>
#include "libdpp/network/DRESTClient.h"


namespace beast = boost::beast;
namespace http = beast::http;
using namespace std;
using namespace DTools::DNetwork;

void OnResponse(DRESTClient::DHttpResponse HttpResponse)
{
    cout << "Response:" << endl << endl;
    cout << HttpResponse << endl;
}

void OnLog(std::string LogMsg)
{
    cout << "DEBUG : " << LogMsg << endl;
}

void OnError(std::string ErrorMsg)
{
    cout << "ERROR : " << ErrorMsg << endl;
}

int main(int argc, char** argv) {
    // Check command line arguments.
    if(argc < 2) {
        cerr <<
            "Usage: rest-client <url> "
            "                   [-t<content-type (default=\"text/plain\")>]"
            "                   [-bs<body as string (default=\"\")>]"
            "                   [-bf<body as file> filename]"
            "                   [-v<HTTP version: 1.0 or 1.1 (default 1.1)>]" << endl << endl <<
            "Example:" << endl <<
            "rest-client \"www.example.com:8080/portal/req\" -t\"application/x-www-form-urlencoded\" -bs\"key1=value1;key2=value2\"\n";
        return EXIT_FAILURE;
    }
    string Url=argv[1];
    string ContentType="text/plain";
    string Body="";
    string Version="1.1";

    for (int ixArg=2; ixArg<argc; ixArg++) {
        if (strncmp(argv[ixArg],"-t",2) == 0) {
            ContentType=string(argv[ixArg]+2);
        }
        else if (strncmp(argv[ixArg],"-bs",3) == 0) {
            Body=string(argv[ixArg]+3);
        }
        else if (strncmp(argv[ixArg],"-bf",3) == 0) {
            string Filename=string(argv[ixArg]+3);
            ifstream FileStream(Filename);
            if (!FileStream.is_open()) {
                cerr << "Error opening " << Filename << endl;
                return(EXIT_FAILURE);
            }
            Body=FileStream.get();
        }
        else if (strncmp(argv[ixArg],"-v",2) == 0) {
            Version=string(argv[ixArg]+2);
        }
    }

    boost::asio::io_context ioc;
    auto RestClient=std::make_shared<DRESTClient>(ioc);

    if (!RestClient->SetUrl(Url)) {
        cerr << RestClient->GetLastStatus();
        return(EXIT_FAILURE);
    }
    RestClient->SetOnResponse(OnResponse);
    RestClient->SetOnLog(OnLog);
    RestClient->SetOnError(OnError);
    RestClient->SetHttpVersion(Version);
    RestClient->SetContentType(ContentType);
    RestClient->SetBody(Body);
    RestClient->SendPOST();
    ioc.run();

    return EXIT_SUCCESS;
}
