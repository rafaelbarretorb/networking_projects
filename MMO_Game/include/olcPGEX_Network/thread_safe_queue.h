/*
  Source code modified by Rafael Barreto 2023

  ASIO Based Networking olcPixelGameEngine Extension v1.0

  Videos:
  Part #1: https://youtu.be/2hNdkYInj4g
  Part #2: https://youtu.be/UbjxGvrDrbw
  Part #3: https://youtu.be/hHowZ3bWsio
  Part #4: https://youtu.be/f_1lt9pfaEo

  License (OLC-3)
  ~~~~~~~~~~~~~~~

  Copyright 2018 - 2021 OneLoneCoder.com

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions or derivations of source code must retain the above
  copyright notice, this list of conditions and the following disclaimer.

  2. Redistributions or derivative works in binary form must reproduce
  the above copyright notice. This list of conditions and the following
  disclaimer must be reproduced in the documentation and/or other
  materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Links
  ~~~~~
  YouTube:	https://www.youtube.com/javidx9
  Discord:	https://discord.gg/WhwHUMV
  Twitter:	https://www.twitter.com/javidx9
  Twitch:		https://www.twitch.tv/javidx9
  GitHub:		https://www.github.com/onelonecoder
  Homepage:	https://www.onelonecoder.com

  Author
  ~~~~~~
  David Barr, aka javidx9, �OneLoneCoder 2019, 2020, 2021

*/

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>
#include <thread>

namespace olc {

namespace net {

// Thread safe Queue
template<typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() = default;
  ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete;
  virtual ~ThreadSafeQueue() { Clear(); }

 public:
    // Returns and maintains item at front of Queue
    const T& Front() {
      std::scoped_lock lock(muxQueue);
      return deqQueue.front();
    }

    // Returns and maintains item at back of Queue
    const T& Back() {
      std::scoped_lock lock(muxQueue);
      return deqQueue.back();
    }

    // Removes and returns item from front of Queue
    T PopFront() {
      std::scoped_lock lock(muxQueue);
      auto t = std::move(deqQueue.front());
      deqQueue.pop_front();
      return t;
    }

    // Removes and returns item from back of Queue
    T pop_back() {
      std::scoped_lock lock(muxQueue);
      auto t = std::move(deqQueue.back());
      deqQueue.pop_back();
      return t;
    }

    // Adds an item to back of Queue
    void PushBack(const T& item) {
      std::scoped_lock lock(muxQueue);
      deqQueue.emplace_back(std::move(item));

      std::unique_lock<std::mutex> ul(muxBlocking);
      cvBlocking.notify_one();
    }

    // Adds an item to front of Queue
    void PushFront(const T& item) {
      std::scoped_lock lock(muxQueue);
      deqQueue.emplace_front(std::move(item));

      std::unique_lock<std::mutex> ul(muxBlocking);
      cvBlocking.notify_one();
    }

    // Returns true if Queue has no items
    bool Empty() {
      std::scoped_lock lock(muxQueue);
      return deqQueue.empty();
    }

    // Returns number of items in Queue
    size_t Count() {
      std::scoped_lock lock(muxQueue);
      return deqQueue.size();
    }

    // Clears Queue
    void Clear() {
      std::scoped_lock lock(muxQueue);
      deqQueue.clear();
    }

    void Wait() {
      while (Empty()) {
        std::unique_lock<std::mutex> ul(muxBlocking);
        cvBlocking.wait(ul);
      }
    }

 protected:
  std::mutex muxQueue;
  std::deque<T> deqQueue;
  std::condition_variable cvBlocking;
  std::mutex muxBlocking;
};

}  // namespace net
}  // namespace olc
