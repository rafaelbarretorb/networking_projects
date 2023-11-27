
#include "server/server.h"

namespace net {
Server::Server() { m_work_.reset(new boost::asio::io_service::work(m_ios_)); }

void Server::Start(uint16_t port_num, unsigned int thread_pool_size) {
  assert(thread_pool_size > 0);

  // Create and start Acceptor.
  acc_.reset(new Acceptor(m_ios_, port_num));
  acc_->Start();

  // Create specified number of threads and
  // add them to the pool.
  for (unsigned int i = 0; i < thread_pool_size; i++) {
    std::unique_ptr<std::thread> th(
        new std::thread([this]() { m_ios_.run(); }));

    m_thread_pool_.push_back(std::move(th));
  }
}

void Server::Stop() {
  acc_->Stop();

  m_ios_.stop();

  for (auto& th : m_thread_pool_) {
    th->join();
  }
}

}  // namespace net
