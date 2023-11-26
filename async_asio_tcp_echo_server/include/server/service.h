#ifndef ASIO_ASYNC_TCP_ECHO_SERVER_SERVICE_H
#define ASIO_ASYNC_TCP_ECHO_SERVER_SERVICE_H

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <string>

using boost::asio::ip::address_v4;
using boost::asio::ip::tcp;
using boost::system::error_code;

namespace net {

class Service {
 public:
  explicit Service(std::shared_ptr<tcp::socket> sock) : m_sock(sock) {}

  void StartHandling();

 private:
  void OnRequestReceived(const error_code& ec, std::size_t bytes_transferred);

  void OnResponseSent(const error_code& ec, std::size_t bytes_transferred);

  // Here we perform the cleanup.
  void OnFinish();

  std::string ProcessMsg(boost::asio::streambuf& stream_buf_msg);

 private:
  std::shared_ptr<tcp::socket> m_sock;
  std::string m_response;
  boost::asio::streambuf m_request;
};

}  // namespace net

#endif  // ASIO_ASYNC_TCP_ECHO_SERVER_SERVICE_H
