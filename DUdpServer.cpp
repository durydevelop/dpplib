#include "DUdpServer.hpp"
#include <iostream>

DUdpServer::DUdpServer(asio::io_context& Context, short Port) : Socket(Context, asio::ip::udp::endpoint(asio::ip::udp::v4(), Port)) {
    do_receive();
}

void DUdpServer::do_receive() {
    Socket.async_receive_from(asio::buffer(Data, MAX_LENGTH), Endpoint, [this](std::error_code ec, std::size_t BytesRecvd) {
        if (!ec) {
            if (BytesRecvd > 0) {
                std::cout << "FROM IP\t:\t" << Endpoint.address().to_string() << "\t:\t" << Data << "\n";
                do_send(BytesRecvd);
            }
            else {
                std::cout << "FROM IP\t:\t" << Endpoint.address().to_string() << "\t0 Byte received" << "\n";
            }
        }
        else {
            std::cout << "IP\t:\t" << Endpoint.address().to_string() << "\tERROR:\t" << ec.message() << "\n";
            do_receive();
        }
    });
}

void DUdpServer::do_send(std::size_t Length) {
    Socket.async_send_to(asio::buffer(Data, Length), Endpoint, [this](std::error_code ec, std::size_t BytesSent) {
        std::cout << "TO IP\t:\t" << Endpoint.address().to_string() << "\t:\t" << Data << "\n";
        do_receive();
    });
}
