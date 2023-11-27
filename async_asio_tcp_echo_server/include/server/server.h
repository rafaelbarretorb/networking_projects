#ifndef ASIO_ASYNC_TCP_ECHO_SERVER_SERVER_H
#define ASIO_ASYNC_TCP_ECHO_SERVER_SERVER_H

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <thread>

#include "server/acceptor.h"

namespace net {

using boost::asio::ip::address_v4;
using boost::asio::ip::tcp;

class Server {
 public:
  Server();

  // Start the server.
  void Start(uint16_t port_num, unsigned int thread_pool_size);

  // Stop the server.
  void Stop();

 private:
  boost::asio::io_service m_ios_;
  std::unique_ptr<boost::asio::io_service::work> m_work_;
  std::unique_ptr<Acceptor> acc_;
  std::vector<std::unique_ptr<std::thread>> m_thread_pool_;
};

}  // namespace net

#endif  // ASIO_ASYNC_TCP_ECHO_SERVER_SERVER_H
