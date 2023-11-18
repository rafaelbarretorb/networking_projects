/*
  Run async tcp client

  ./async_tcp_client <ip-address-name> <port-number> <client-name>
  <sleep-time-in-secs>

  Ex
  ./async_tcp_client 127.0.0.1 3333 Ted 5
  ./async_tcp_client 127.0.0.1 3333 Barney 2
  ./async_tcp_client 127.0.0.1 3333 Marshall
*/

#include <boost/system/error_code.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include "client/client.h"
#include "client/session.h"

using boost::asio::ip::address;
using boost::asio::ip::tcp;
using boost::system::error_code;

// /*
// argv[1] IP
// argv[2] Port
// argv[3] Client Name
// argv[4] Sleep Time
// */

void printClientInfo(char *info[]) {
  std::cout << "Client Info:\n";
  std::cout << "IP:             " << info[1] << "\n";
  std::cout << "Port:           " << info[2] << "\n";
  std::cout << "Name:           " << info[3] << "\n";
  std::cout << "Sleep Time:     " << info[4] << "\n";
}

void printClientMsg(const std::string &name, const std::string &msg) {
  std::cout << "Client : " << name << "\n";
  std::cout << "Message : " << msg << "\n";
  std::cout << "\n";
}

void handler(const std::string &response, const error_code &ec) {
  if (ec.value() == 0) {
    std::cout << "[Client] Request has completed. Server said: '" << response
              << "'\n\n";
  } else {
    std::cout << "Request"
              << " failed! Error code = " << ec.value()
              << ". Error message = " << ec.message() << std::endl;
  }

  return;
}

int main(int argc, char *argv[]) {
  // if (argc == 5) {
  //   printClientInfo(argv);
  // } else {
  //   std::cout << "Error: missing command Line arguments\n";
  // }

  const std::string msg{"Hello Server!"};
  const uint32_t time{10};
  const std::string raw_ip_address{"192.168.15.5"};
  uint16_t port{3333};

  try {
    net::Client client;
    // Here we emulate the user's behavior.
    // User initiates a request with id 1.
    client.sendMsg(msg, time, raw_ip_address, port, handler);

    // while (true) {
    //   printClientMsg(client_name, msg);

    //   client.sendMsg(msg, server_ip_address, server_port_num, handler);
    //   std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
    // }

    client.close();
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();

    return e.code().value();
  }

  return 0;
}
