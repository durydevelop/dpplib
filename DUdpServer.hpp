#ifndef DUdpServerH
#define DUdpServerH

//#include <cstdlib>
//#include <iostream>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>
#include <asio.hpp>
#include "DCallback.hpp"

class DUdpServer {
    public:
        DUdpServer(asio::io_context& Context, short Port);
        void do_receive();
        void do_send(std::size_t Length);
        //void SetRecvHandler()

		//! Registra la callback
		void SetCallback(DCallback::DCALLBACK_FUNC clbk) {
			callback=new DGlobalCallback(clbk);
		}

		//! Esegue la chiamata di callback
		void DoCallback(uint8_t Command, void* Param) {
			if (callback == NULL) {
				return;
			}
			callback->call(Command, Param);
		}

    private:
    	DGlobalCallback *callback;
        asio::ip::udp::socket Socket;
        asio::ip::udp::endpoint Endpoint;
        enum { MAX_LENGTH = 1024 };
        uint8_t Data[MAX_LENGTH];
};

#endif // DUdpServer
