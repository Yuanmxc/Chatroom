#include "TCPServer.hpp"
#include <asm-generic/socket.h>
#include <sys/socket.h>

TcpServer::TcpServer() {
  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  // 设置SO_REUSEADDR套接字选项，实现地址复用
  int optval = 1;
  setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

TcpServer::~TcpServer() { close(m_fd); }

int TcpServer::setListen(unsigned short port) {
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY; // 0 = 0.0.0.0
  int ret = bind(m_fd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }
  cout << "套接字绑定成功, ip: " << inet_ntoa(saddr.sin_addr)
       << ", port: " << port << endl;

  ret = listen(m_fd, 128);
  if (ret == -1) {
    perror("listen");
    return -1;
  }
  cout << "设置监听成功..." << endl;

  return ret;
}

TcpSocket *TcpServer::acceptConn(sockaddr_in *addr) {
  socklen_t addrlen = sizeof(struct sockaddr_in);
  int cfd = accept(m_fd, (struct sockaddr *)addr, &addrlen);
  if (cfd == -1) {
    perror("accept");
    return nullptr;
  }
  // cout << "成功和客户端建立连接..." << endl;
  return new TcpSocket(cfd);
}