
#include "client/client.h"

using boost::system::error_code;

namespace net {

Client::Client() {
  m_work_.reset(new boost::asio::io_service::work(m_ioc_));
  m_thread_.reset(new std::thread([this]() { m_ioc_.run(); }));
}

void Client::SendMsg(const std::string& client_name, const std::string& message,
                     uint32_t duration_sec, const std::string& raw_ip_address,
                     uint16_t port_num, Callback callback) {
  // Preparing the request string.
  std::string request = client_name + ";" + message + "\n";

  std::shared_ptr<Session> session = std::shared_ptr<Session>(
      new Session(m_ioc_, raw_ip_address, port_num, request, callback));

  session->m_sock_.open(session->m_ep_.protocol());

  // tcp::socket::async_connect()
  session->m_sock_.async_connect(session->m_ep_, [this, session](
                                                     const error_code& ec) {
    if (ec.value() != 0) {
      session->m_ec_ = ec;
      OnRequestComplete(session);
      is_connected_ = false;
      return;
    } else {
      is_connected_ = true;
    }

    // tcp::socket::async_write_some(buffer, handler);
    boost::asio::async_write(
        session->m_sock_, boost::asio::buffer(session->m_request_),
        [this, session](const error_code& ec, std::size_t bytes_transferred) {
          if (ec.value() != 0) {
            session->m_ec_ = ec;
            OnRequestComplete(session);
            return;
          }

          boost::asio::async_read_until(
              session->m_sock_, session->m_response_buf_, '\n',
              [this, session](const error_code& ec,
                              std::size_t bytes_transferred) {
                if (ec.value() != 0) {
                  session->m_ec_ = ec;
                } else {
                  std::istream strm(&session->m_response_buf_);
                  std::getline(strm, session->m_response_);
                }

                OnRequestComplete(session);
              });  // async_read_until
        });        // async_write
  });              // async_connect
}

void Client::Close() {
  // Destroy work object. This allows the I/O thread to
  // exits the event loop when there are no more pending
  // asynchronous operations.
  m_work_.reset(NULL);

  // Wait for the I/O thread to exit.
  m_thread_->join();
}

void Client::OnRequestComplete(std::shared_ptr<Session> session) {
  // Shutting down the connection. This method may
  // fail in case socket is not connected. We don’t care
  // about the error code if this function fails.
  error_code ignored_ec;

  session->m_sock_.shutdown(tcp::socket::shutdown_both, ignored_ec);

  error_code ec;

  if (session->m_ec_.value() != 0) {
    ec = session->m_ec_;
  }

  // Call the callback provided by the user.
  session->m_callback_(session->m_response_, ec);
}
}  // namespace net
