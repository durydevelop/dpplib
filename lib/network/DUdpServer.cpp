#include "libdpp/network/DUdpServer.h"
#include <iostream>
#include <functional>

namespace asio  =   boost::asio;
namespace err   =   boost::system;

DUdpSession::DUdpSession(DUdpServer* server) {
    RecvBuffer=new uint8_t[100];
    SendBuffer=new uint8_t[100];
    Server=server;
    BufferSize=100;
}

void DUdpSession::handle_request(const boost::system::error_code& ec, std::size_t BytesRecvd)
{
    //std::cout << "SERVER: FROM IP\t:\t" << RemoteEndpoint.address().to_string() << "\t:\t" << std::string((char*)RecvBuffer,BytesRecvd) << "\n";
    if (!ec || ec == asio::error::message_size) {
        Server->DoCallbackDataRecvd(shared_from_this(),RecvBuffer,BytesRecvd);
    }
    else {
        //std::cout << "SERVER: ERROR IP\t:\t" << RemoteEndpoint.address().to_string() << "\t:\t" << ec.message() << "\n";
        Server->DoCallbackError(shared_from_this(),ec.message());
    }
}

void DUdpSession::handle_sent(const boost::system::error_code& ec, std::size_t BytesSent) {
    //std::cout << "SERVER: TO IP\t:\t" << RemoteEndpoint.address().to_string() << "\t:\t" << std::string((char*)SendBuffer,BytesSent) << "\n";
    Server->DoCallbackDataSent(shared_from_this(),SendBuffer,BytesSent);
    if (ec) {
        //std::cout << "SERVER: ERROR IP\t:\t" << RemoteEndpoint.address().to_string() << "\t:\t" << ec.message() << "\n";
        Server->DoCallbackError(shared_from_this(),ec.message());
    }
}

DUdpServer::DUdpServer(asio::io_service& io_service) : socket_(io_service, asio::ip::udp::endpoint(asio::ip::udp::v4(), 666)), strand_(io_service) {
    OnDataRecvd=0;
    OnDataSent=0;
    OnError=0;
    ioservice=&io_service;
}

void DUdpServer::Run() {
    StartReceiveSession();
    ioservice->run();
}

void DUdpServer::StartReceiveSession() {
    auto session = std::make_shared<DUdpSession>(this);

    socket_.async_receive_from(
            asio::buffer(session->RecvBuffer,session->BufferSize),
            session->RemoteEndpoint,
            strand_.wrap(
                bind(&DUdpServer::handle_receive, this,
                    session, // keep-alive of buffer/endpoint
                    std::placeholders::_1,
                    std::placeholders::_2)));
}

void DUdpServer::handle_receive(shared_session session, const boost::system::error_code& ec, std::size_t BytesRecvd) {
    //std::cout << "handle_receive " << std::string(session->recv_buffer_.data(),bytes_recvd) << "\n";
    post(socket_.get_executor(),bind(&DUdpSession::handle_request,session,ec,BytesRecvd));

    // immediately accept new datagrams
    StartReceiveSession();
}

void DUdpServer::PostResponse(shared_session const& session, size_t len) {
    socket_.async_send_to(
        asio::buffer(session->SendBuffer,len),
        session->RemoteEndpoint,
        strand_.wrap(bind(&DUdpServer::handle_response, this,
            session, // keep-alive of buffer/endpoint
            std::placeholders::_1,
            std::placeholders::_2)));
}

void DUdpServer::handle_response(shared_session session, const boost::system::error_code& ec, std::size_t BytesRecvd) {
    post(socket_.get_executor(),bind(&DUdpSession::handle_sent,session,ec,BytesRecvd));
}

//! Registra la callback per dati ricevuti
void DUdpServer::SetOnDataRecvd(DCallbackData callback) {
    OnDataRecvd=callback;
}
//! Registra la callback per dati inviati
void DUdpServer::SetOnDataSent(DCallbackData callback) {
    OnDataSent=callback;
}

//! Registra la callback per gli errori
void DUdpServer::SetOnError(DCallbackErr callback) {
    OnError=callback;
}

//! Esegue la chiamata di callback per dati ricevuti
void DUdpServer::DoCallbackDataRecvd(shared_session session, uint8_t *data, size_t len) {
    if (OnDataRecvd) {
        OnDataRecvd(session,data,len);
    }
}

//! Esegue la chiamata di callback per dati inviati
void DUdpServer::DoCallbackDataSent(shared_session session, uint8_t *data, size_t len) {
    if (OnDataSent) {
        OnDataSent(session,data,len);
    }
}

//! Esegue la chiamata di callback per gli errori
void DUdpServer::DoCallbackError(shared_session session, std::string Error) {
    if (OnError) {
        OnError(session,Error);
    }
}

//! Esegue la chiamata di callback per dati ricevuti
void DUdpServer::Send(shared_session session, uint8_t *data, size_t len) {
    session->SendBuffer=data;
    PostResponse(session,len);
}
