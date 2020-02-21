#include "DUdpServer.hpp"

//#include <cstdlib>
#include <iostream>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>

DUdpServer::DUdpServer(asio::io_context& io_context, short port) : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {
    do_receive();
}

void DUdpServer::do_receive() {
    socket_.async_receive_from(asio::buffer(data_, max_length), sender_endpoint_, [this](std::error_code ec, std::size_t bytes_recvd) {
        if (!ec) {
            if (bytes_recvd > 0) {
                std::cout << "FROM IP\t:\t" << sender_endpoint_.address().to_string() << "\t:\t" << data_ << "\n";
                do_send(bytes_recvd);
            }
            else {
                std::cout << "FROM IP\t:\t" << sender_endpoint_.address().to_string() << "\t0 Byte received" << "\n";
            }
        }
        else {
            std::cout << "IP\t:\t" << sender_endpoint_.address().to_string() << "\tERROR:\t" << ec.message() << "\n";
            do_receive();
        }
    });
}

void DUdpServer::do_send(std::size_t length) {
    socket_.async_send_to(asio::buffer(data_, length), sender_endpoint_, [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/) {
        std::cout << "TO IP\t:\t" << sender_endpoint_.address().to_string() << "\t:\t" << data_ << "\n";
        do_receive();
    });
}
