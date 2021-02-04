#include <iostream>
#include "DUdpServer.h"
//#include "DTcpServer.h"
#include <functional>

using namespace std;
DUdpServer *dUdpServer;

void UdpDataRecvd(DUdpServer::shared_session session, uint8_t *data, size_t len) {
    std::cout << "FROM IP\t:\t" << session->RemoteEndpoint.address().to_string() << "\t:\t" << std::string((char*)data,len) << "\n";
    session->Server->Send(session,data,len);
}

void UdpDataSent(DUdpServer::shared_session session, uint8_t *data, size_t len) {
    std::cout << "TO IP\t:\t" << session->RemoteEndpoint.address().to_string() << "\t:\t" << std::string((char*)data,len) << "\n";
}

void UdpError(DUdpServer::shared_session session,std::string Error) {
    std::cout << "ERR IP\t:\t" << session->RemoteEndpoint.address().to_string() << "\t:\t" << Error << "\n";
}

int main(int argc, char* argv[])
{
    try {
        asio::io_context iocontext;
        dUdpServer=new DUdpServer(iocontext);
        dUdpServer->SetOnDataRecvd(UdpDataRecvd);
        dUdpServer->SetOnDataSent(UdpDataSent);
        dUdpServer->SetOnError(UdpError);
        //dUdpServer->Run();

        asio::detail::thread_group group;
        for (unsigned i = 0; i < thread::hardware_concurrency(); ++i)
            group.create_thread([&dUdpServer](){dUdpServer->Run();});

        group.join();

    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    delete dUdpServer;
/*
    try {
        asio::io_context Context;

        dUdpServer=new DUdpServer(Context,666);
        dUdpServer->SetOnData(UdpData);
        dUdpServer->SetOnError(UdpError);

        DTcpServer dTcpServer(Context,666);
        Context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
*/
    system("pause");
    return 0;
}
