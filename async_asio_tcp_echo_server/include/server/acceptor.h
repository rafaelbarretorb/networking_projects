#ifndef ASIO_ASYNC_TCP_ECHO_SERVER_ACCEPTOR_H
#define ASIO_ASYNC_TCP_ECHO_SERVER_ACCEPTOR_H

#include <atomic>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <mutex>
#include <thread>

namespace net {

using boost::asio::ip::address_v4;
using boost::asio::ip::tcp;
using boost::system::error_code;

class Acceptor {
 public:
  Acceptor(boost::asio::io_service &ios, uint16_t port_num)
      : m_ios_(ios),
        m_acceptor_(m_ios_, tcp::endpoint(address_v4::any(), port_num)),
        m_is_stopped_(false) {}

  // Start accepting incoming connection requests.
  void Start();

  // Stop accepting incoming connection requests.
  void Stop();

 private:
  void InitAccept();
  void OnAccept(const error_code &ec, std::shared_ptr<tcp::socket> sock);

  boost::asio::io_service &m_ios_;
  tcp::acceptor m_acceptor_;
  std::atomic<bool> m_is_stopped_;
};

}  // namespace net

#endif  // ASIO_ASYNC_TCP_ECHO_SERVER_ACCEPTOR_H
