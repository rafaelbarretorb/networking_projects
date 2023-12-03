/*
  Run async tcp client

  ./run_tcp_client <ip> <port> <client> <sleep-time>
  - ip: IP address
  - port: port number
  - client: client name
  - sleep-time: sleep time in seconds

  Ex:
  ./run_tcp_client 127.0.0.1 3333 Ted 2
  ./run_tcp_client 127.0.0.1 3333 Barney 1
  ./run_tcp_client 127.0.0.1 3333 Marshall 3
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

void printClientInfo(char *info[]) {
  std::cout << "Client Info:\n";
  std::cout << "IP:             " << info[1] << "\n";
  std::cout << "Port:           " << info[2] << "\n";
  std::cout << "Name:           " << info[3] << "\n";
  std::cout << "Sleep Time:     " << info[4] << "\n";
}

void printClientMsg(const std::string &name, const std::string &msg) {
  std::cout << "Message  : " << name << " said: " << msg << "\n";
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
  if (argc == 5) {
    printClientInfo(argv);
  } else {
    std::cout << "Error: missing command Line arguments\n";
    std::cout << "Usage:\n";
    std::cout
        << "./run_tcp_client <ip-address-name> <port-number> <client-name>\n";
    return 0;
  }

  const std::string raw_ip_address{argv[1]};
  uint16_t port{(uint16_t)std::atoi(argv[2])};
  const std::string client_name{argv[3]};
  uint32_t time{(uint32_t)std::atoi(argv[4])};

  const std::string msg{"Hello Server!"};

  try {
    net::Client client;

    do {
      printClientMsg(client_name, msg);
      client.SendMsg(client_name, msg, time, raw_ip_address, port, handler);
      std::this_thread::sleep_for(std::chrono::seconds(time));
    } while (client.IsConnected());

    client.Close();
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();

    return e.code().value();
  }

  return 0;
}
