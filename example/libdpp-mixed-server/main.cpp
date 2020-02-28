#include <iostream>
#include "DUdpServer.hpp"
#include "DTcpServer.hpp"

using namespace std;
DUdpServer *dUdpServer;

void UdpDataRecvd(DUdpServer::shared_session session, uint8_t *data, size_t len) {
    std::cout << "FROM IP\t:\t" << session->RemoteEndpoint.address().to_string() << "\t:\t" << std::string((char*)data,len) << "\n";
    dUdpServer->Send(session,data,len);
}

void UdpDataSent(DUdpServer::shared_session session, uint8_t *data, size_t len) {
    std::cout << "TO IP\t:\t" << session->RemoteEndpoint.address().to_string() << "\t:\t" << std::string((char*)data,len) << "\n";
}

void UdpError(DUdpServer::shared_session session,std::string Error) {
    std::cout << "ERROR IP\t:\t" << session->RemoteEndpoint.address().to_string() << "\t:\t" << Error << "\n";
}

int main(int argc, char* argv[])
{
    try {
        asio::io_service io_service;
        dUdpServer=new DUdpServer(io_service);
        dUdpServer->SetOnDataRecvd(UdpDataRecvd);
        dUdpServer->SetOnDataSent(UdpDataSent);
        dUdpServer->SetOnError(UdpError);
        io_service.run();
/*
        thread_group group;
        for (unsigned i = 0; i < thread::hardware_concurrency(); ++i)
            group.create_thread(bind(&asio::io_service::run, ref(io_service)));

        group.join_all();
*/
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
