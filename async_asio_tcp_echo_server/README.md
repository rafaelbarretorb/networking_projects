## async_asio_tcp_echo_server

Implementation of asynchronous TCP Server capable to handle multiple connections and process requests from multiple clients. This kind of "Hello World!" example of asynchronous server-client applications.
- C++ 17
- Boost.Asio
- Lambda functions (since C++11)
- Asynchronous TCP Server and Client

#### Build and Run
```
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

##### Run Server
```
$ ./run_tcp_server
```
##### Run Client

./run_tcp_client \<ip> \<port> \<client> \<sleep-time>
Example:
```
$ ./run_tcp_client 127.0.0.1 3333 Ted 2
```

##### Terminal Output

![terminal_output](images/terminal_output.png)

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