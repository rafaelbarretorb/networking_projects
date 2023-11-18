/*
  NO copyright
  page 148 - Implementing an asynchronous TCP server

  g++ async_tcp_server.cpp -std=c++17 -o async_tcp_server -lboost_system -lboost_thread -pthread
*/

#include <boost/system/system_error.hpp>
#include <iostream>
#include <thread>

#include "Server.h"

// #include <boost/system/system_error.hpp>
// #include <boost/asio.hpp>
// #include <boost/core/noncopyable.hpp>

// using boost::asio::ip::tcp;
// using boost::asio::ip::address_v4;
// using boost::system::error_code;

// class Service {
//  public:
//   explicit Service(std::shared_ptr<tcp::socket> sock) :
//     m_sock(sock) {}

//   void StartHandling() {
//     boost::asio::async_read_until(*m_sock.get(),
//                                   m_request,
//                                   '\n',
//                                   [this](const error_code& ec,
//                                          std::size_t bytes_transferred) {
//       onRequestReceived(ec, bytes_transferred);
//     });
//   }

//  private:
//   void onRequestReceived(const error_code& ec,
//                          std::size_t bytes_transferred) {

//     if (ec.value() != 0) {
//       std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
//       onFinish();
//       return;
//     }

//     // Process the request.
//     m_response = ProcessRequest(m_request);

//     // Initiate asynchronous write operation.
//     boost::asio::async_write(*m_sock.get(),
//                              boost::asio::buffer(m_response),
//                              [this](const error_code& ec,
//                                     std::size_t bytes_transferred) {
//       onResponseSent(ec, bytes_transferred);
//     });
//   }

//   void onResponseSent(const error_code& ec,
//     std::size_t bytes_transferred) {

//     if (ec.value() != 0) {
//       std::cout << "Error occured! Error code = "
//       << ec.value()
//       << ". Message: " << ec.message();
//     }

//     onFinish();
//   }

//   // Here we perform the cleanup.
//   void onFinish() {
//     delete this;
//   }

//   std::string ProcessRequest(boost::asio::streambuf& request) {
//     // get buffer size
//     size_t nBufferSize = boost::asio::buffer_size(request.data());

//     // get const buffer
//     std::stringstream response;
//     boost::asio::streambuf::const_buffers_type constBuffer = request.data();

//     // copy const buffer to stringstream, then output
//     std::copy(
//         boost::asio::buffers_begin(constBuffer),
//         boost::asio::buffers_begin(constBuffer) + nBufferSize,
//         std::ostream_iterator<char>(response)
//     );

//     std::cout << "[DEBUG] Test reading buffer" << "\n";
//     std::cout << response.str() << "\n";

//     return response.str();
//   }

//  private:
//   std::shared_ptr<tcp::socket> m_sock;
//   std::string m_response;
//   boost::asio::streambuf m_request;
// };


// class Acceptor {
//  public:
//   Acceptor(boost::asio::io_service &ios,  // NOLINT
//            uint16_t port_num) :
//            m_ios(ios),
//            m_acceptor(m_ios,
//                       tcp::endpoint(address_v4::any(), port_num)),
//            m_isStopped(false) {}

//   // Start accepting incoming connection requests.
//   void Start() {
//     m_acceptor.listen();
//     InitAccept();
//   }

//   // Stop accepting incoming connection requests.
//   void Stop() {
//     m_isStopped.store(true);
//   }

//  private:
//   void InitAccept() {
//     std::shared_ptr<tcp::socket> sock(new tcp::socket(m_ios));

//     m_acceptor.async_accept(*sock.get(),
//                             [this, sock](const error_code& error) {
//       onAccept(error, sock);
//     });
//   }

//   void onAccept(const error_code&ec,
//                 std::shared_ptr<tcp::socket> sock) {
//     if (ec.value() == 0) {
//       (new Service(sock))->StartHandling();
//     } else {
//       std::cout<< "Error occured! Error code = "
//       <<ec.value()
//       << ". Message: " <<ec.message();
//     }
//     // Init next async accept operation if
//     // acceptor has not been stopped yet.
//     if (!m_isStopped.load()) {
//       InitAccept();
//     } else {
//       // Stop accepting incoming connections
//       // and free allocated resources.
//       m_acceptor.close();
//     }
//   }

//   boost::asio::io_service &m_ios;
//   tcp::acceptor m_acceptor;
//   std::atomic<bool> m_isStopped;
// };

// class Server {
//  public:
//   Server() {
//     m_work.reset(new boost::asio::io_service::work(m_ios));
//   }

//   // Start the server.
//   void Start(uint16_t port_num, unsigned int thread_pool_size) {
//     assert(thread_pool_size > 0);

//     // Create and start Acceptor.
//     acc.reset(new Acceptor(m_ios, port_num));
//     acc->Start();

//     // Create specified number of threads and
//     // add them to the pool.
//     for (unsigned int i = 0; i < thread_pool_size; i++) {
//       std::unique_ptr<std::thread> th(new std::thread([this]() {
//           m_ios.run();
//       }));

//       m_thread_pool.push_back(std::move(th));
//     }
//   }

//   // Stop the server.
//   void Stop() {
//     acc->Stop();

//     m_ios.stop();

//     for (auto& th : m_thread_pool) {
//       th->join();
//     }
//   }

//  private:
//   boost::asio::io_service m_ios;
//   std::unique_ptr<boost::asio::io_service::work> m_work;
//   std::unique_ptr<Acceptor> acc;
//   std::vector<std::unique_ptr<std::thread>> m_thread_pool;
// };

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main() {
  const uint16_t port_num = 3333;

  try {
    net::Server srv;
    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0)
      thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    srv.Start(port_num, thread_pool_size);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    srv.Stop();
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
    << ". Message: " << e.what();

    return e.code().value();
  }

  return 0;
}
