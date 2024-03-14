#include "ThreadPool.hpp"
#include "ThreadPool.cc"
#include "Option.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <netinet/in.h>
#include <bits/types/time_t.h>

#define LOCALPORT 6666

int epfd;
Redis redis;
using namespace std;

int main(){
    // 连接redis服务端
    struct timeval timeout = {1, 500000};
    redis.connect(timeout);    //超时连接
    // 将每个账号的在线状态改为-1
    int num = redis.scard("用户uid集合");
    redisReply **allAccounts = redis.smembers("用户uid集合");
    for(int i = 0; i < num; i++){
        redis.hsetValue(allAccounts[i]->str, "在线状态", "-1");
    }

    ThreadPool<Argc_func> pool(2,10); // 创建一个线程池类
    TcpServer sfd_class;                             // 创建服务器的socket
    map<string, int> uid_cfd;                        // 一个uid对应一个cfd的表
    int ret;                                         // 检测返回值
    ret = sfd_class.setListen(LOCALPORT);            // 设置监听返回监听符.内部报错
    if(ret == -1) {exit(1);}

    // 创建epoll实例，并把listenfd加进去，监视可读事件
    epfd = epoll_create(5);
    if(epfd == -1) { exit(1); }
    struct epoll_event temp,ep[1024];
    temp.data.fd = sfd_class.getfd();
    temp.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd_class.getfd(), &temp);
    if(ret == -1) {
        my_error("epoll_ctl() failed.");
    }
    
    // 循环监听自己的符看是否有连接请求，监听客户端的符看是否有消息需要处理
    while(true) {
        int readyNum = epoll_wait(epfd, ep, 1024, -1);  // 有几个符就绪了
        for (int i = 0; i < readyNum; i++){  // 对于ep中每个就绪的符
            // 如果是服务器的符，说明新客户端的交互/通知套接字连接，接入连接并把符扔进epoll,并在fd-uid表里加上该符，对应uid先为-1，在登录时在获得并写入uid
            if (ep[i].data.fd == sfd_class.getfd()) { 
                TcpSocket* cfd_class = sfd_class.acceptConn(NULL);
                temp.data.fd = cfd_class->getfd();
                temp.events = EPOLLIN;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd_class->getfd(),&temp);
                redis.hsetValue("fd-uid对应表", to_string(ep[i].data.fd), "-1");
                cout << "客户端套接字连接成功，套接字为：" << ep[i].data.fd << endl;
            }
            // 如果是客户端的符，就接收消息，并处理
            else {
                TcpSocket cfd_class(ep[i].data.fd);   // 用这个符创一个类来交互信息
                string command_string = cfd_class.recvMsg(); // 接收命令json字符串
                cout << "接收到的命令字符串为：" << command_string << endl;

                // 如果命令字符串是说客户端挂了，socket类里关fd，并修改用户信息，摘符
                if(command_string == "close" || command_string == "-1" || command_string == "quit"){
                    if(!redis.hashexists("fd-uid对应表", to_string(ep[i].data.fd))){
                        break;
                    }    
                    string cuid = redis.gethash("fd-uid对应表", to_string(ep[i].data.fd));
                    cout << "退出的客户端的uid为：" << cuid << endl;
                    if(cuid.size() == 4){
                        cout << "cuid : " << cuid << endl;
                        redis.hsetValue(cuid, "在线状态", "-1");
                        redis.hsetValue(cuid, "通知套接字", "-1");
                    }
                    epoll_ctl(epfd,EPOLL_CTL_DEL,cfd_class.getfd(),&temp);
                    redis.hsetValue("fd-uid对应表", to_string(ep[i].data.fd), "-1");
                    cout << "客户端断开连接" << endl;
                    continue;
                }
                else{
                    // 命令类将sring格式的字符串转为josn格式的字符串
                    Command command;
                    command.From_Json(command_string);
                    // 如果是通知套接字来消息，说明是告诉服务器该通知套接字属于哪个账号，更改这个账号的通知套接字并加在fd-uid对应表里，不运行任务函数
                    if(command.m_flag == SETRECVFD){
                        redis.hsetValue(command.m_uid, "通知套接字", to_string(ep[i].data.fd));
                        redis.hsetValue("fd-uid对应表", to_string(ep[i].data.fd), command.m_uid + "(通)");
                    }
                    // 不是通知套接字消息，说明是用户的命令，把命令和客户端套接字传进任务函数进行处理
                    else{
                        Argc_func *argc_func = new Argc_func(cfd_class, command_string);   
                        // 摘符
                        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd_class.getfd(), &temp);
                        // 调用任务函数，传发过来的json字符串格式过去
                        pool.addTask(Task<Argc_func>(&taskfunc,static_cast<void*>(argc_func)));
                        // 上符
                        temp.data.fd = cfd_class.getfd();
                        temp.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, temp.data.fd, &temp);
                    }
                }
            }
        }
    }

    return 0;
}
