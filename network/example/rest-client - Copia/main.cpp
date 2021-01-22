#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <boost/beast/core.hpp>


#include "../../DRESTClient.h"


namespace beast = boost::beast;
namespace http = beast::http;
/*
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
*/
using namespace std;
using namespace DTools::DNetwork;

std::string trim_prefix(std::string s, std::string prefix)
{
    return s.find(prefix) == 0 ? s.substr(prefix.length()) : s;
}

int main(int argc, char** argv) {
    // The io_context is required for all I/O
    boost::asio::io_context ioc;
    // Launch the asynchronous operation
    auto RestClient=std::make_shared<DRESTClient>(ioc);

    RestClient->SetUrl("http://api.intra.photorec.it/orders/new");
    std::cout << RestClient->GetUri().PrintUriParts() << std::endl;

    DRESTClient::DPost dPost(DRESTClient::CONTENT_TYPE_URL_ENCODED);
    dPost.AddBodyParam("customer","99961");
    dPost.AddBodyParam("envelope","745123");
    dPost.AddBodyParam("downloadDate","20201118");
    dPost.AddBodyParam("article","MED-VINTAGE");
    dPost.AddBodyParam("status","Confezionamento");
    dPost.AddBodyParam("software","Rikordait");
    dPost.AddBodyParam("webportal","rikorda");
    dPost.AddBodyParam("quantity","23");
    dPost.AddBodyParam("price","57");
    //RestClient->SendPOST(dPost);
    std::string Body=dPost.GetBodyAsString();
    RestClient->SendPOST(DRESTClient::CONTENT_TYPE_URL_ENCODED,DRESTClient::ENCODE_NONE,Body);

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
