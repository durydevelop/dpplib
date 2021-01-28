#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <boost/beast/core.hpp>
#include "libdpp/network/DRESTClient.h"


namespace beast = boost::beast;
namespace http = beast::http;
using namespace std;
using namespace DTools::DNetwork;

void OnResponse(DRESTClient::DHttpResponse HttpResponse)
{
    cout << "HttpResponse:" << endl;
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
    if(argc != 4 && argc != 5) {
        std::cerr <<
            "Usage: rest-client <url> [-bs<body as string>] [-bf<body as file>] [-t<content-type>] [-v<HTTP version: 1.0 or 1.1(default)>]\n" <<
            "Example:\n" <<
            "    http-client-async www.example.com 80 /\n" <<
            "    http-client-async www.example.com 80 / 1.0\n";
        return EXIT_FAILURE;
    }
    auto const host = argv[1];
    auto const port = argv[2];
    auto const target = argv[3];
    int version = argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11;

    boost::asio::io_context ioc;
    auto RestClient=std::make_shared<DRESTClient>(ioc);

    RestClient->SetUrl("http://api.photorec.it/orders/new");
    RestClient->SetOnResponse(OnResponse);
    RestClient->SetOnLog(OnLog);
    RestClient->SetOnError(OnError);
    std::cout << RestClient->GetUri().PrintUriParts() << std::endl;

    RestClient->SetContentType(DRESTClient::CONTENT_TYPE_URL_ENCODED);
    //RestClient->SetEncodeType(DRESTClient::ENCODE_FRAGMENT);
    RestClient->AddReqBodyParam("customer","99961");
    RestClient->AddReqBodyParam("customer","99961");
    RestClient->AddReqBodyParam("envelope","745123");
    RestClient->AddReqBodyParam("downloadDate","20201118");
    RestClient->AddReqBodyParam("article","MED-VINTAGE");
    RestClient->AddReqBodyParam("status","Confezionamento");
    RestClient->AddReqBodyParam("software","Rikordait");
    RestClient->AddReqBodyParam("webportal","rikorda");
    RestClient->AddReqBodyParam("quantity","23");
    RestClient->AddReqBodyParam("price","57");

    RestClient->SendPOST();

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
