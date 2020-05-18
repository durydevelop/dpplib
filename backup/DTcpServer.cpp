#include "DTcpServer.hpp"
//#include <cstdlib>
#include <iostream>
//#include <memory>
//#include <utility>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>


DTcpSession::DTcpSession(asio::ip::tcp::socket socket) : socket_(std::move(socket)) {

}

void DTcpSession::start() {
    do_read();
}

void DTcpSession::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, max_length), [this, self](std::error_code ec, std::size_t length) {
        if (!ec) {
            std::cout << "FROM IP\t:\t" << socket_.remote_endpoint().address().to_string() << "\t:\t" << data_ << "\n";
            do_write(length);
        }
        else {
            std::cout << "READ IP\t:\t" << socket_.remote_endpoint().address().to_string() << "\tERROR:\t" << ec.message() << "\n";
        }
    });
}

void DTcpSession::do_write(std::size_t length) {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data_, length), [this, self](std::error_code ec, std::size_t /*length*/) {
        if (!ec) {
            std::cout << "TO IP\t:\t" << socket_.remote_endpoint().address().to_string() << "\t:\t" << data_ << "\n";
            do_read();
        }
        else {
            std::cout << "WRITE IP\t:\t" << socket_.remote_endpoint().address().to_string() << "\tERROR:\t" << ec.message() << "\n";
        }
    });
}

DTcpServer::DTcpServer(asio::io_context& io_context, short port) : acceptor_(io_context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(),port)),socket_(io_context) {
    do_accept();
}

void DTcpServer::do_accept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
        if (!ec) {
            std::cout << "CONNECTED IP\t:\t" << socket_.remote_endpoint().address().to_string() << "\n";
            std::make_shared<DTcpSession>(std::move(socket_))->start();
        }
        else {
            std::cout << "ACCEPT IP\t:\t" << socket_.remote_endpoint().address().to_string() << "\tERROR:\t" << ec.message() << "\n";
        }

        do_accept();
    });
}
