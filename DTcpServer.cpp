#include "DTcpServer.h"
#include <iostream>

#define DEFAULT_BUFF_LEN 1024
#define DEFAULT_TIMEOUT_SEC TIMEOUT_DISABLE

///******************************************************************************************************************
///******************************                   DTcpServer class                   ******************************
///******************************************************************************************************************
//! DConnection contructor
/**
* @param server         ->  reference of a DTcpServer that handle connection
* @param ConnectionID   ->  id assigned to this connection (default = 0)
**/
DTcpServer::Connection::Connection(DTcpServer& server, size_t ConnectionID) : server_(server), read_timer(server.io_service_), write_timer(server.io_service_), Socket(server.io_service_), ID(ConnectionID)
{
    BuffLen=DEFAULT_BUFF_LEN;
    RecvBuff=new uint8_t[BuffLen];
}

//! DConnection destructor
DTcpServer::Connection::~Connection()
{
    server_.OnClosed(ID);
    delete RecvBuff;
}

//! Start connection session
void DTcpServer::Connection::Start() {
    // Set socket options
    Socket.set_option(asio::ip::tcp::no_delay(true));
    // Start Reading
    DoRead();
}

//! Start an async reading loop
/**
* Remark:
* After a successful read do the (@ref)DTcpServer::OnDataRecv callback.
* If an error occurs, (@ref)DTcpServer::OnError callback is called.
**/
void DTcpServer::Connection::DoRead() {
    // Look for timeout
    if (server_.ReadTimeoutSec > 0) {
        // Setup expire timer
        read_timer.expires_from_now(std::chrono::seconds(server_.ReadTimeoutSec));
        read_timer.async_wait([this](std::error_code ec) {
            if (!ec) {
                std::cout << "Read timeout\n";
                Shutdown();
            }
        });
    }

    // Really start reading
    Socket.async_read_some(asio::buffer(RecvBuff,BuffLen), [this, self = shared_from_this()](std::error_code ec, std::size_t length) {
        if (!ec) {
            //std::cout << "DoReadCmd()->FROM IP\t:\t" << Socket.remote_endpoint().address().to_string() << "\t:\t" << "\t:\tLength=" << length << "\t:\t" << std::string((char*)RecvBuff,length) << "\n";
            // Do the OnDataRecvd callback
            server_.OnDataRecvd(shared_from_this(),std::ref(*RecvBuff),length);
            DoRead();
        }
        else {
            //std::cout << "DoReadCmd()->READ IP\t:\t" << Socket.remote_endpoint().address().to_string() << "\tERROR:\t" << ec.message() << "\n";
            // Do the OnError callback
            server_.OnError(shared_from_this(),ec);
            Close();
        }
    });
}

//! Start an async writing sequence
void DTcpServer::Connection::DoWrite() {
    std::cout << "in DoWrite()\n";
    active_buffer_ ^= 1; // switch buffers
    for (const auto& data : buffers_[active_buffer_]) {
        buffer_seq_.push_back(asio::buffer(data));
    }

    // Look for timeout
    if (server_.WriteTimeoutSec > 0) {
        write_timer.expires_from_now(std::chrono::seconds(server_.WriteTimeoutSec));
        write_timer.async_wait([this](std::error_code ec) {
            if (!ec) {
                std::cout << "Write timeout\n";

           Shutdown();
            }
        });
    }

    // Start write
    std::cout << "Active buffer: " << active_buffer_ << "\tWrite buffers count: " << buffer_seq_.size() << "\n";
    asio::async_write(Socket, buffer_seq_, [this, self = shared_from_this()](const std::error_code& ec, size_t bytes_transferred) {
        write_timer.cancel();
        std::lock_guard<std::mutex> lock(buffers_mtx_);
        buffers_[active_buffer_].clear();
        buffer_seq_.clear();
        if (!ec) {
            std::cout << "Wrote " << bytes_transferred << " bytes\n";
            if (!buffers_[active_buffer_ ^ 1].empty()) // have more work
                DoWrite();
            }
            else {
                server_.OnError(shared_from_this(),ec);
                Close();
            }
        }
      );

    std::cout << "out DoWrite()\n";
}

//! Send a string
/**
* @param data   ->  string to send
**/
void DTcpServer::Connection::Write(std::string str) {
    std::lock_guard<std::mutex> lock(buffers_mtx_);
    //std::cout << "pushing " << std::string(std::ref(data)) << " in " << active_buffer_ << "\n";
    std::vector<uint8_t> Data(data.begin(),str.end());
    buffers_[active_buffer_ ^ 1].push_back(Data); // move input data to the inactive buffer
    if (!IsWriting()) {
        //std::cout << "before DoWrite()\n";
        DoWrite();
        //std::cout << "after DoWrite()\n";
    }
}

//! Send a buffer
/**
* @param data   ->  pointer to uint8_t buffer
* @param len    ->  length of buffer
**/
void DTcpServer::Connection::Write(const uint8_t data[], const size_t len) {
    std::lock_guard<std::mutex> lock(buffers_mtx_);
    //std::cout << "pushing " << std::string(std::ref(data)) << " in " << active_buffer_ << "\n";
    std::vector<uint8_t> Data(data,data+len);
    buffers_[active_buffer_ ^ 1].push_back(Data); // move input data to the inactive buffer
    if (!IsWriting()) {
        //std::cout << "before DoWrite()\n";
        DoWrite();
        //std::cout << "after DoWrite()\n";
    }
}

//! @return true if an async writing operation is still running
bool DTcpServer::Connection::IsWriting() const
{
    return(!buffer_seq_.empty());
}

//! Perform a safe close of the connection
void DTcpServer::Connection::Close() {
    //std::cout << "Closing... \n";
    closing_ = true;
    if (!IsWriting()) {
        Shutdown();
    }
}

//! Shutdown the socket and remove this connection from server list
void DTcpServer::Connection::Shutdown() {
    if (!closed_) {
        closing_ = closed_ = true;
        std::error_code ec;
        Socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            server_.OnError(shared_from_this(),ec);
        }
        Socket.close();
        server_.active_connections_.erase(shared_from_this());
    }
}

//! @return the remote endpoint connected to the socket
asio::ip::tcp::endpoint DTcpServer::Connection::RemoteEndpoint() const {
    return(Socket.remote_endpoint());
}

//! @return the connection ID
size_t DTcpServer::Connection::GetID() {
    return(ID);
}

///******************************************************************************************************************
///******************************                   DTcpServer class                   ******************************
///******************************************************************************************************************

DTcpServer::DTcpServer(int port, uint8_t timeout_read, uint8_t timeout_write) : acceptor_(io_service_, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port), true) {
    ReadTimeoutSec=timeout_read;
    WriteTimeoutSec=timeout_write;
    LastConnectionID=0;
    OnError=0;
    OnAccept=0;
    OnDataRecvd=0;
}

void DTcpServer::Run() {
    std::cout << "Listening on " << acceptor_.local_endpoint() << "\n";
    DoAccept();
    io_service_.run();
}

void DTcpServer::Stop() {
    acceptor_.close();
    {
        std::vector<std::shared_ptr<Connection>> sessionsToClose;
        copy(active_connections_.begin(), active_connections_.end(), back_inserter(sessionsToClose));
        for (auto& s : sessionsToClose)
        s->Shutdown();
    }
    active_connections_.clear();
    io_service_.stop();
}

void DTcpServer::DoAccept() {
    if (acceptor_.is_open()) {
        // Create a shared session
        auto session = std::make_shared<Connection>(*this,++LastConnectionID);
        // Try to accept connection
        acceptor_.async_accept(session->Socket, [this, session](std::error_code ec) {
            if (!ec) {
                // Accepted
                DoCallbackAccept(session);
                // Add session
                active_connections_.insert(session);
                // Start it
                session->Start();
            }
            else {
                // Error accepting
                DoCallbackError(session,ec);
            }
            DoAccept();
        });
    }
    else {
        // TODO: Error?
    }
}

void DTcpServer::SetReadTimeout(uint8_t TimeoutSec) {
    ReadTimeoutSec=TimeoutSec;
}

void DTcpServer::SetWriteTimeout(uint8_t TimeoutSec) {
    WriteTimeoutSec=TimeoutSec;
}

void DTcpServer::SetTimeout(uint8_t TimeoutSec) {
    ReadTimeoutSec=TimeoutSec;
    WriteTimeoutSec=TimeoutSec;
}

void DTcpServer::SetOnError(DCallbackErr Callback) {
    OnError=Callback;
}

void DTcpServer::SetOnAccept(DCallbackConn Callback) {
    OnAccept=Callback;
}

void DTcpServer::SetOnDataRecvd(DCallbackData Callback) {
    OnDataRecvd=Callback;
}

void DTcpServer::SetOnClosed(DCallbackClosed Callback) {
    OnClosed=Callback;
}

void DTcpServer::DoCallbackError(const std::shared_ptr<Connection> connection, const std::error_code& ec) {
    if (OnError) {
        OnError(connection,ec);
    }
}

void DTcpServer::DoCallbackAccept(const std::shared_ptr<DTcpServer::Connection> connection) {
    if (OnAccept) {
        OnAccept(connection);
    }
}
