void DRESTClient::AsyncRun(void)
{
    ioc.run();
}

void DRESTClient::AsyncPoll(void)
{
    ioc.poll();
}

/**
 * @brief Send a POST request using current data stored.
 */
void DRESTClient::AsyncSendPOST(void)
{
    PrepareHttpRequest(REQ_POST);
    AsyncSend();
}

/**
 * @brief Send a PUT request using current data stored.
 */
void DRESTClient::AsyncSendPUT(void)
{
    PrepareHttpRequest(REQ_PUT);
    AsyncSend();
}

/**
 * @brief Send a GET request using current data stored.
 */
void DRESTClient::AsyncSendGET(void)
{
    PrepareHttpRequest(REQ_GET);
    AsyncSend();
}

void DRESTClient::AsyncSend(void)
{
    if (!Connected) {
        if (!Connect()) {
            DisconnectAfter=true;
            Error("Connection failed:");
            return;
        }
    }
    else if (dUri.Host.empty()) {
        Error("Missing host in url");
        return;
    }

    // Send request
    AsyncWrite();

}

/**
 * @brief Perform an async write of HttpRequest.
 * The function return immediatly and OnWrite funcion is called after data is sent.
 */
void DRESTClient::AsyncWrite(void)
{
    #ifdef DEBUG
        std::stringstream ss;
        ss << std::endl << "HttpRequest:" << std::endl;
        ss << HttpRequest << std::endl;
        Log(ss.str());
    #endif

    TcpStream.expires_after(std::chrono::seconds(TimeoutSec));
    http::async_write(TcpStream,HttpRequest,beast::bind_front_handler(&DRESTClient::OnWrite,shared_from_this()));
}

/**
 * @brief Callback fired on async_write data sent.
 * @param ec                ->  contain error_code on write error.
 * @param bytes_transferred ->  contain numer of byte sent.
 */
void DRESTClient::OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    #ifdef DEBUG
        Log("Wrote "+std::to_string(bytes_transferred)+" bytes");
    #endif

    boost::ignore_unused(bytes_transferred);
    if (ec) {
        Error("Error write: "+ec.message());
        return;
    }
    AsyncRead();
}

/**
 * @brief Perform an async read operation.
 * The function return immediatly and OnReade funcion is called after data is receved.
 * Responce wil be put in HttpResponse object.
 */
void DRESTClient::AsyncRead(void)
{
    #ifdef DEBUG
        Log("Async Reading...");
    #endif
    http::async_read(TcpStream,Buffer,HttpResponse,beast::bind_front_handler(&DRESTClient::OnRead,shared_from_this()));
}

/**
 * @brief Callback fired on async_read data received.
 * @param ec                ->  contain error_code on write error.
 * @param bytes_transferred ->  contain number of bytes received.
 */
void DRESTClient::OnRead(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    #ifdef DEBUG
        Log("Read "+std::to_string(bytes_transferred)+" bytes");
    #endif
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        Error("Error read: "+ec.message());
    }

    //std::cout << std::endl << "HttpResponse:" << std::endl;
    //std::cout << HttpResponse << std::endl << std::endl;

    DoResponseCallback();

    if (DisconnectAfter) {
        DisconnectAfter=false;
        Disconnect();
    }
}