#include "server/acceptor.h"

#include <iostream>

#include "server/service.h"

namespace net {

void Acceptor::Start() {
  m_acceptor_.listen();
  InitAccept();
}

void Acceptor::Stop() { m_is_stopped_.store(true); }

void Acceptor::InitAccept() {
  std::shared_ptr<tcp::socket> sock(new tcp::socket(m_ios_));

  m_acceptor_.async_accept(*sock.get(), [this, sock](const error_code& error) {
    OnAccept(error, sock);
  });
}

void Acceptor::OnAccept(const error_code& ec,
                        std::shared_ptr<tcp::socket> sock) {
  if (ec.value() == 0) {
    (new Service(sock))->StartHandling();
  } else {
    std::cout << "Error occured! Error code = " << ec.value()
              << ". Message: " << ec.message();
  }
  // Init next async accept operation if
  // acceptor has not been stopped yet.
  if (!m_is_stopped_.load()) {
    InitAccept();
  } else {
    // Stop accepting incoming connections
    // and free allocated resources.
    m_acceptor_.close();
  }
}

}  // namespace net
