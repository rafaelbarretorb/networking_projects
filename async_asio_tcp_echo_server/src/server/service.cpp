#include "server/service.h"

#include <iostream>

namespace net {

void Service::StartHandling() {
  boost::asio::async_read_until(
      *m_sock.get(), m_request, '\n',
      [this](const error_code& ec, std::size_t bytes_transferred) {
        OnRequestReceived(ec, bytes_transferred);
      });
}

void Service::OnRequestReceived(const error_code& ec,
                                std::size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "Error occured! Error code = " << ec.value()
              << ". Message: " << ec.message();
    OnFinish();
    return;
  }

  // Process the request.
  m_response = ProcessMsg(m_request);

  // Initiate asynchronous write operation.
  boost::asio::async_write(
      *m_sock.get(), boost::asio::buffer(m_response),
      [this](const error_code& ec, std::size_t bytes_transferred) {
        OnResponseSent(ec, bytes_transferred);
      });
}

void Service::OnResponseSent(const error_code& ec,
                             std::size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "Error occured! Error code = " << ec.value()
              << ". Message: " << ec.message();
  }

  OnFinish();
}

void Service::OnFinish() { delete this; }

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

  std::string delimiter = ";";
  std::string client_name = message.substr(0, message.find(delimiter));
  std::string client_message = message.substr(message.find(delimiter) + delimiter.size(), std::string::npos);

  std::cout << "[Server] Client " << client_name << " said: " << client_message << "'\n";
  std::string response{"Hello " + client_name + "!\n"};
  return response;
}

}  // namespace net
