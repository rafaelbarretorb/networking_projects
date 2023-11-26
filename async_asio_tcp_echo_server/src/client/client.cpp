
#include "client/client.h"

using boost::system::error_code;

namespace net {

Client::Client() {
  m_work.reset(new boost::asio::io_service::work(m_ioc));
  m_thread.reset(new std::thread([this]() { m_ioc.run(); }));
}

void Client::SendMsg(const std::string& client_name, const std::string& message, unsigned int duration_sec,
                     const std::string& raw_ip_address, uint16_t port_num,
                     Callback callback) {
  // Preparing the request string.
  std::string request = client_name + ";" + message + "\n";

  std::shared_ptr<Session> session = std::shared_ptr<Session>(
      new Session(m_ioc, raw_ip_address, port_num, request, callback));

  session->m_sock.open(session->m_ep.protocol());

  session->m_sock.async_connect(
      session->m_ep, [this, session](const error_code& ec) {
        if (ec.value() != 0) {
          session->m_ec = ec;
          OnRequestComplete(session);
          is_connected_ = false;
          return;
        } else {
          is_connected_ = true;
        }

        boost::asio::async_write(
            session->m_sock, boost::asio::buffer(session->m_request),
            [this, session](const error_code& ec,
                            std::size_t bytes_transferred) {
              if (ec.value() != 0) {
                session->m_ec = ec;
                OnRequestComplete(session);
                return;
              }

              boost::asio::async_read_until(
                  session->m_sock, session->m_response_buf, '\n',
                  [this, session](const error_code& ec,
                                  std::size_t bytes_transferred) {
                    if (ec.value() != 0) {
                      session->m_ec = ec;
                    } else {
                      std::istream strm(&session->m_response_buf);
                      std::getline(strm, session->m_response);
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
  m_work.reset(NULL);

  // Wait for the I/O thread to exit.
  m_thread->join();
}

void Client::OnRequestComplete(std::shared_ptr<Session> session) {
  // Shutting down the connection. This method may
  // fail in case socket is not connected. We don’t care
  // about the error code if this function fails.
  error_code ignored_ec;

  session->m_sock.shutdown(tcp::socket::shutdown_both, ignored_ec);

  error_code ec;

  if (session->m_ec.value() != 0) {
    ec = session->m_ec;
  }

  // Call the callback provided by the user.
  session->m_callback(session->m_response, ec);
}
}  // namespace net
