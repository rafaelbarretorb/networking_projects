/*
  NO copyright
  Implementing an asynchronous TCP client

  ./async_tcp_client <ip-address-name> <port-number> <client-name> <sleep-time-in-secs>

  Ex
  ./async_tcp_client 127.0.0.1 3333 Rafael 5
  ./async_tcp_client 127.0.0.1 3333 Felipe 2
*/


#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>

#include "Session.h"

using boost::asio::ip::tcp;
using boost::asio::ip::address;
using boost::system::error_code;


// class AsyncTCPClient : public boost::noncopyable {
//  public:
//   explicit AsyncTCPClient(const std::string name) : client_name{name} {
//     m_work.reset(new boost::asio::io_service::work(m_ioc));
//     m_thread.reset(new std::thread([this](){
//       m_ioc.run();
//     }));
//   }

//   void sendMsg(const std::string& message,
//                                 const std::string& raw_ip_address,
//                                 uint16_t port_num,
//                                 Callback callback) {
//     std::cout << "[DEBUG] Test 1 " << std::endl;

//     std::string request = message + "\n";


//     std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session(m_ioc,
//                                                                             raw_ip_address,
//                                                                             port_num,
//                                                                             request,
//                                                                             callback));

//     session->m_sock.open(session->m_ep.protocol());

//     std::unique_lock<std::mutex> lock(m_active_sessions_guard);
//     m_active_sessions.push(session);
//     lock.unlock();

//     std::cout << "[DEBUG] Test 2 " << std::endl;

//     session->m_sock.async_connect(session->m_ep,
//                                   [this, session](const error_code& ec) {
//       if (ec.value() != 0) {
//         std::cout << "[DEBUG] Test 12" << std::endl;
//         session->m_ec = ec;
//         onRequestComplete(session);
//         return;
//       }

//       boost::asio::async_write(session->m_sock,
//                               boost::asio::buffer(session->m_request),
//                               [this, session](const error_code& ec,
//                                               std::size_t bytes_transferred) {
//         if (ec.value() != 0) {
//           std::cout << "[DEBUG] Test 13" << std::endl;
//           session->m_ec = ec;
//           onRequestComplete(session);
//           return;
//         }

//         boost::asio::async_read_until(session->m_sock,
//                                       session->m_response_buf,
//                                       '\n',
//                                       [this, session](const error_code& ec,
//                                                       std::size_t bytes_transferred) {
//           if (ec.value() != 0) {
//             std::cout << "[DEBUG] Test 14" << std::endl;
//             session->m_ec = ec;
//           } else {
//             std::istream strm(&session->m_response_buf);
//             std::getline(strm, session->m_response);
//           }

//           onRequestComplete(session);
//         });  // async_read_until
//       });  // async_write
//     });  // async_connect

//     std::cout << "[DEBUG] Test 3 " << std::endl;
//   }

//   void close() {
//     // Destroy work object. This allows the I/O thread to
//     // exits the event loop when there are no more pending
//     // asynchronous operations.
//     m_work.reset(NULL);

//     // Wait for the I/O thread to exit.
//     m_thread->join();
//   }

//  private:
//   void onRequestComplete(std::shared_ptr<Session> session) {
//     std::cout << "[DEBUG] Test 20" << std::endl;
//     // Shutting down the connection. This method may
//     // fail in case socket is not connected. We don’t care
//     // about the error code if this function fails.
//     boost::system::error_code ignored_ec;

//     session->m_sock.shutdown(tcp::socket::shutdown_both, ignored_ec);

//     // Remove session from the queue
//     std::unique_lock<std::mutex> lock(m_active_sessions_guard);
//     m_active_sessions.pop();
//     lock.unlock();

//     boost::system::error_code ec;

//     if (session->m_ec.value() == 0) {
//       ec = boost::asio::error::operation_aborted;
//     } else {
//       ec = session->m_ec;
//     }
//     // Call the callback provided by the user.
//     session->m_callback(session->m_response, ec);
//   }

//  private:
//   boost::asio::io_context m_ioc;
//   std::queue<std::shared_ptr<Session>> m_active_sessions;
//   std::mutex m_active_sessions_guard;
//   std::unique_ptr<boost::asio::io_service::work> m_work;
//   std::unique_ptr<std::thread> m_thread;

//   const std::string client_name;
// };

// void handler(const std::string& response,
//              const boost::system::error_code& ec) {
//   if (ec.value() == 0) {
//     std::cout << "Response: " << response << std::endl;
//   } else {
//     std::cout << "Request failed! Error code = " << ec.value()
//     << ". Error message = " << ec.message()
//     << std::endl;
//   }

//   return;
// }

// /*
// argv[1] IP
// argv[2] Port
// argv[3] Client Name
// argv[4] Sleep Time
// */

// void printClientInfo(char *info[]) {
//   std::cout << "Client Info:\n";
//   std::cout << "IP:             " << info[1] << "\n";
//   std::cout << "Port:           " << info[2] << "\n";
//   std::cout << "Name:           " << info[3] << "\n";
//   std::cout << "Sleep Time:     " << info[4] << "\n";
// }

// void printClientMsg(const std::string &name, const std::string &msg) {
//   std::cout << "Client : " << name << "\n";
//   std::cout << "Message : " << msg << "\n";
//   std::cout << "\n";
// }

// int main(int argc, char *argv[]) {
//   if (argc == 5) {
//     printClientInfo(argv);
//   } else {
//     std::cout << "Error: missing command Line arguments\n";
//   }

//   const std::string server_ip_address = std::string(argv[1]);
//   const uint16_t server_port_num = std::stoi(argv[2]);
//   const std::string client_name = std::string(argv[3]);
//   const uint16_t sleep_time = std::stoi(argv[4]);

//   const std::string msg {"Hello\n"};

//   try {
//     AsyncTCPClient client{client_name};

//     //while (true) {
//       //printClientMsg(client_name, msg);
      
//       // client.sendMsg(msg, server_ip_address, server_port_num, handler);
//       //std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
//     // }
//     // client.sendMsg(msg, server_ip_address, server_port_num, handler);
//     client.sendMsg(msg, "127.0.0.1", 3333, handler);
    
//     // Then does nothing for 5 seconds.
//     std::this_thread::sleep_for(std::chrono::seconds(5));
    
//     // Decides to exit the application.
//     client.close();
//   } catch (boost::system::system_error &e) {
//     std::cout << "Error occured! Error code = " << e.code()
//     << ". Message: " << e.what();

//     return e.code().value();
//   }

//   return 0;
// }

class AsyncTCPClient : public boost::noncopyable {
 public:
  AsyncTCPClient() {
    m_work.reset(new boost::asio::io_service::work(m_ioc));
    m_thread.reset(new std::thread([this](){
      m_ioc.run();
    }));
}

  void sendMsg(const std::string& message,
  unsigned int duration_sec,
                                const std::string& raw_ip_address,
                                uint16_t port_num,
                                Callback callback) {
    // Preparing the request string.
    std::string request = message + "\n";

    std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session(m_ioc,
                                                                            raw_ip_address,
                                                                            port_num,
                                                                            request,
                                                                            callback));

    session->m_sock.open(session->m_ep.protocol());

    session->m_sock.async_connect(session->m_ep,
                                  [this, session](const boost::system::error_code& ec) {
      if (ec.value() != 0) {
        session->m_ec = ec;
        onRequestComplete(session);
        return;
      }

      boost::asio::async_write(session->m_sock,
                              boost::asio::buffer(session->m_request),
                              [this, session](const boost::system::error_code& ec,
                                              std::size_t bytes_transferred) {
        if (ec.value() != 0) {
          session->m_ec = ec;
          onRequestComplete(session);
          return;
        }

        boost::asio::async_read_until(session->m_sock,
                                      session->m_response_buf,
                                      '\n',
                                      [this, session](const boost::system::error_code& ec,
                                                      std::size_t bytes_transferred) {
          if (ec.value() != 0) {
            session->m_ec = ec;
          } else {
            std::istream strm(&session->m_response_buf);
            std::getline(strm, session->m_response);
          }

          onRequestComplete(session);
        });  // async_read_until
      });  // async_write
    });  // async_connect
  }

  void close() {
    // Destroy work object. This allows the I/O thread to
    // exits the event loop when there are no more pending
    // asynchronous operations.
    m_work.reset(NULL);

    // Wait for the I/O thread to exit.
    m_thread->join();
  }

 private:
  void onRequestComplete(std::shared_ptr<Session> session) {
    // Shutting down the connection. This method may
    // fail in case socket is not connected. We don’t care
    // about the error code if this function fails.
    boost::system::error_code ignored_ec;

    session->m_sock.shutdown(tcp::socket::shutdown_both, ignored_ec);

    boost::system::error_code ec;

    if (session->m_ec.value() != 0) {
      ec = session->m_ec;
    }

    // Call the callback provided by the user.
    session->m_callback(session->m_response, ec);
  }

 private:
  boost::asio::io_context m_ioc;
  std::mutex m_active_sessions_guard;
  std::unique_ptr<boost::asio::io_service::work> m_work;
  std::unique_ptr<std::thread> m_thread;
};

void handler(const std::string& response,
             const boost::system::error_code& ec) {
  if (ec.value() == 0) {
    std::cout << "Request has completed. Response: "
    << response << std::endl;
  } else {
    std::cout << "Request"
    << " failed! Error code = " << ec.value()
    << ". Error message = " << ec.message()
    << std::endl;
  }

  return;
}

int main() {
  const std::string msg {"Hello Server"};
  try {
    AsyncTCPClient client;
    // Here we emulate the user's behavior.
    // User initiates a request with id 1.
    client.sendMsg(msg, 10, "127.0.0.1", 3333, handler);

    client.close();
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
    << ". Message: " << e.what();

    return e.code().value();
  }

  return 0;
}
