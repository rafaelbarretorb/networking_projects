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

#include "olcPGEX_Network/connection.h"
#include "olcPGEX_Network/message.h"

#include <memory>
#include <ostream>

namespace olc {

namespace net {

template <typename T>
class Connection;

// An "owned" message is identical to a regular message, but it is associated with
// a connection. On a server, the owner would be the client that sent the message,
// on a client the owner would be the server.
template <typename T>
struct OwnedMessage {
  std::shared_ptr<Connection<T>> remote = nullptr;
  Message<T> msg;

  // Again, a friendly string maker
  friend std::ostream& operator<<(std::ostream& os, const OwnedMessage<T>& msg)  {
    os << msg.msg;
    return os;
  }
};

}  // namespace net

}  // namespace olc
