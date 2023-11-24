#ifndef ASIO_ASYNC_TCP_ECHO_SERVER_CLIENT_H
#define ASIO_ASYNC_TCP_ECHO_SERVER_CLIENT_H

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <memory>
#include <mutex>
#include <thread>

#include "client/session.h"

namespace net {

class Client : public boost::noncopyable {
 public:
  Client();

  void sendMsg(const std::string& message, unsigned int duration_sec,
               const std::string& raw_ip_address, uint16_t port_num,
               Callback callback);

  void close();

  bool IsConnected() { return is_connected_; };

 private:
  void onRequestComplete(std::shared_ptr<Session> session);

 private:
  boost::asio::io_context m_ioc;
  std::mutex m_active_sessions_guard;
  std::unique_ptr<boost::asio::io_service::work> m_work;
  std::unique_ptr<std::thread> m_thread;
  bool is_connected_{false};
};

}  // namespace net
#endif  // ASIO_ASYNC_TCP_ECHO_SERVER_CLIENT_H
