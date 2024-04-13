#include "TCPSocket.hpp"
#include <asm-generic/errno-base.h>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

TcpSocket::TcpSocket() { m_fd = socket(AF_INET, SOCK_STREAM, 0); }

TcpSocket::TcpSocket(int socket) { m_fd = socket; }

TcpSocket::TcpSocket(string recv) {
  if (recv == "recv") {
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    recv_fd = socket(AF_INET, SOCK_STREAM, 0);
  }
}

TcpSocket::~TcpSocket() {}
int TcpSocket::connectToHost(string ip, unsigned short port) {
  // 连接服务器IP port
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &saddr.sin_addr.s_addr);
  int ret = connect(m_fd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret == -1) {
    perror("connect");
    return -1;
  }

  return ret;
}

int TcpSocket::sendMsg(string msg) {
  // 申请内存空间: 数据长度 + 包头4字节(存储数据长度)
  char *data = new char[msg.size() + 4];
  int bigLen = htonl(msg.size());
  memcpy(data, &bigLen, 4);
  memcpy(data + 4, msg.data(), msg.size());
  // 发送数据
  int ret = writen(data, msg.size() + 4);
  // cout << "msg :" << msg << endl;
  // cout << "ret : " << ret << endl;
  delete[] data;
  // cout << "delete OK" << endl;
  return ret;
}

string TcpSocket::recvMsg() {
  // 接收数据
  // 1. 读数据头
  int len = 0;
  int ret = readn((char *)&len, 4);
  if (ret == 0) {
    return "close";
  }
  len = ntohl(len);
  // cout << "数据块大小: " << len << endl;
  // 根据读出的长度分配内存
  char *buf = new char[len + 1];
  ret = readn(buf, len);
  if (ret == 0) {
    delete[] buf;
    return "close";
  } else if (ret != len) {
    delete[] buf;
    return to_string(len);
  }
  buf[len] = '\0';
  string retStr(buf);
  delete[] buf;

  return retStr;
}

int TcpSocket::readn(char *buf, int size) {
  int nread = 0;
  int left = size;
  char *p = buf;

  while (left > 0) {
    if ((nread = read(m_fd, p, left)) > 0) {
      p += nread;
      left -= nread;
    } else if (nread == -1) {
      if (errno == EINTR)
        continue;
      else {
        perror("read: ");
        close(m_fd);
        return -1;
      }
    } else if (nread == 0) {
      cout << "对端已关闭" << endl;
      close(m_fd);
      return 0;
    }
  }
  return size;
}

int TcpSocket::writen(const char *msg, int size) {
  int left = size;
  int nwrite = 0;
  const char *p = msg;

  while (left > 0) {
    if ((nwrite = write(m_fd, msg, left)) > 0) {
      p += nwrite;
      left -= nwrite;
    } else if (nwrite == -1) {
      if (errno == EINTR) {
        perror("write:");
        close(m_fd);
        return -1;
      } else
        continue;
    } else if (nwrite == 0) {
      cout << "对端已关闭" << endl;
      close(m_fd);
      return 0;
    }
  }
  return size;
}
