## async_asio_tcp_echo_server

Implementation of asynchronous TCP Server capable to handle multiple connections and process requests from multiple clients. This the "Hello World!" example of server applications.
- C++ 17
- Boost.Asio
- Lambda functions (since C++11)
- Asynchronous TCP Server and Client

#### Folder Structure
<pre>
async_asio_tcp_echo_server
├── build
├── include
│   ├── client
│   |   ├── client.h
│   |   └── session.h
|   |
│   └── server
│       ├── acceptor.h
│       ├── server.h
│       └── service.h
│
├── src
│   ├── client
│   |   └── client.cpp
|   |
│   ├── server
│   |   ├── acceptor.cpp
│   |   ├── server.cpp
│   |   └── service.cpp
│   |
|   ├── run_tcp_client.cpp
|   └── run_tcp_server.cpp
|
├── CMakeLists.txt
└── README.md
</pre>