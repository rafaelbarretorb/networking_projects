#ifndef ASIO_ASYNC_TCP_ECHO_SERVER_SESSION_H
#define ASIO_ASYNC_TCP_ECHO_SERVER_SESSION_H

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

namespace net {
using boost::asio::ip::address;
using boost::asio::ip::tcp;

// Function pointer type that points to the callback function which is called
// when a request is complete.
using Callback = void (*)(const std::string& response,
                          const boost::system::error_code& ec);

// Data structure whose purpose is to keep the data related to a
// particular request while it is being executed. Let's name it Session.
// Structure represents a context of a single request.
struct Session {
  Session(boost::asio::io_context& ioc, const std::string& raw_ip_address,
          uint16_t port_num, const std::string& request, Callback callback)
      : m_sock(ioc),
        m_ep(address::from_string(raw_ip_address), port_num),
        m_request(request),
        m_callback(callback) {}

  tcp::socket m_sock;     // Socket used for communication
  tcp::endpoint m_ep;     // Remote endpoint
  std::string m_request;  // Request string

  // streambuf where the response will be stored
  boost::asio::streambuf m_response_buf;

  std::string m_response;  // Response represented as a string

  // Contains the description of an error if one occurs during
  // the request life cycle.
  boost::system::error_code m_ec;

  // Pointer to the function to be called when the request completes
  Callback m_callback;
};

}  // namespace net
#endif  // ASIO_ASYNC_TCP_ECHO_SERVER_SESSION_H
