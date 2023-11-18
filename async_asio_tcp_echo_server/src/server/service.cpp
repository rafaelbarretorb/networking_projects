#include "server/service.h"

#include <iostream>

namespace net {

void Service::StartHandling() {
  boost::asio::async_read_until(
      *m_sock.get(), m_request, '\n',
      [this](const error_code& ec, std::size_t bytes_transferred) {
        onRequestReceived(ec, bytes_transferred);
      });
}

void Service::onRequestReceived(const error_code& ec,
                                std::size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "Error occured! Error code = " << ec.value()
              << ". Message: " << ec.message();
    onFinish();
    return;
  }

  // Process the request.
  m_response = ProcessMsg(m_request);

  // Initiate asynchronous write operation.
  boost::asio::async_write(
      *m_sock.get(), boost::asio::buffer(m_response),
      [this](const error_code& ec, std::size_t bytes_transferred) {
        onResponseSent(ec, bytes_transferred);
      });
}

void Service::onResponseSent(const error_code& ec,
                             std::size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "Error occured! Error code = " << ec.value()
              << ". Message: " << ec.message();
  }

  onFinish();
}

void Service::onFinish() { delete this; }

std::string Service::ProcessMsg(boost::asio::streambuf& stream_buf_msg) {
  // get buffer size
  size_t nBufferSize{boost::asio::buffer_size(stream_buf_msg.data())};

  // get const buffer
  std::stringstream msg_stream;
  boost::asio::streambuf::const_buffers_type constBuffer{stream_buf_msg.data()};

  // copy const buffer to stringstream, then output
  std::copy(boost::asio::buffers_begin(constBuffer),
            boost::asio::buffers_begin(constBuffer) + nBufferSize,
            std::ostream_iterator<char>(msg_stream));

  // remove last char '\n'
  std::string message{msg_stream.str()};
  message.pop_back();

  std::cout << "[Server] Client said: '" << message << "'\n";
  std::string response{"Hello Client!\n"};
  return response;
}

}  // namespace net
