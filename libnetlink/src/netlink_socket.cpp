#include "netlink_socket.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include "warning_disable.h"
#include <iostream>
#include <arpa/inet.h>
#include <logger.h>

DISABLE_WARNING_PUSH
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_IMPLICIT_INT_CONVERSION
DISABLE_WARNING_SHORTEN_64_TO_32
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_SIGN_COMPARE
DISABLE_WARNING_OLD_STYLE_CAST
DISABLE_WARNING_CAST_ALIGN
DISABLE_WARNING_SIGN_CONVERSION

#define EPOLL_SIZE 50

#define MAX_PAYLOAD 4096

libNetlink::netlink_socket::netlink_socket(){
}
    
libNetlink::netlink_socket::~netlink_socket(){
    close(this->socketFd_);
}

int libNetlink::netlink_socket::create(int protocol){
    this->socketFd_ = -1;
    if((this->socketFd_ = socket(AF_NETLINK, SOCK_RAW, protocol))==-1){
        common::logger::error()<<"[NetlinkSocket] create socket failed: "<<strerror(errno)<< endl;
        return -1;
    }
    return this->socketFd_;
}


int libNetlink::netlink_socket::binding(){
    struct sockaddr_nl local;
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid();
    // local.nl_groups = 0;
    local.nl_groups = RTMGRP_IPV4_ROUTE | RTMGRP_IPV4_RULE;
    int retval = bind(this->socketFd_, reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
    if(retval == -1){
        common::logger::error()<<"[NetlinkSocket] bind socket failed: "<<strerror(errno)<< endl;
        close(this->socketFd_);
        return -1;
    }
    return retval;
}



int libNetlink::netlink_socket::acceptAndListen(){
    char buffer[1024];
    struct iovec iov = {buffer, sizeof(buffer)};
    struct sockaddr_nl netlinkAddr;
    struct msghdr msg = {(void *) &netlinkAddr, sizeof(netlinkAddr), &iov, 1, ((void *) 0), 0, 0};
    
    ssize_t size = recvmsg(this->socketFd_, &msg, 0);
    if (size < 0) {
        // common::logger::error()<<"[NetlinkSocket] receive message failed."<<endl;
        return 0;
    }

    if (netlinkAddr.nl_pid != 0) {
        common::logger::error()<<"[NetlinkSocket] Ignore non kernel message from pid:"<< netlinkAddr.nl_pid<<endl;
        return 0;
    }

    if (size == 0) {
        common::logger::error()<<"[NetlinkSocket] EOF."<< endl;
        return -1;
    }


    if (msg.msg_namelen != sizeof(netlinkAddr)) {
        common::logger::error()<<"[NetlinkSocket] sender address length error."<<endl;
        return -1;
    }

    for (struct nlmsghdr *hdr = (struct nlmsghdr *) buffer; NLMSG_OK(hdr, size); hdr = NLMSG_NEXT(hdr, size)) {
        switch (hdr->nlmsg_type)
        {
        case NLMSG_DONE:
            return 0;
        case NLMSG_ERROR:{
            struct nlmsgerr *err = (struct nlmsgerr *) NLMSG_DATA(hdr);
            if (err->error == 0) {
                return 0;
            }
            if (hdr->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
                common::logger::error()<<"[NeetlinkSocket] Socket error: message truncated"<<endl;
                return -1;
            }
            common::logger::error()<<"[NeetlinkSocket] Socket error: "<<strerror(-err->error)<<", type="<< err->msg.nlmsg_type<<", seq="<<err->msg.nlmsg_seq<<", pid="<<err->msg.nlmsg_pid<<endl;
            return -1;
        }
        default:
            this->handler_(this->socketFd_, buffer, size);
            break;
        }
    }
    return 0;
}

int libNetlink::netlink_socket::sendNetlinkMsgToKernel(struct nlmsghdr* msg){
    if(!msg){
        common::logger::error()<<"[NeetlinkSocket] msg can not be null "<<endl;
        return -1;
    }
    struct iovec ioVector;
    ioVector.iov_base = msg;
    ioVector.iov_len = msg->nlmsg_len;

    struct msghdr msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));

    struct sockaddr_nl kernel;
    memset(&kernel,0,sizeof(kernel));
    kernel.nl_family = AF_NETLINK;
    kernel.nl_groups = 0;

    msgHeader.msg_name = &kernel;
    msgHeader.msg_namelen = sizeof(kernel);
    msgHeader.msg_iov = &ioVector;
    msgHeader.msg_iovlen = 1;

    long msgState = sendmsg(this->socketFd_,&msgHeader,0);
    if(msgState == -1){
        common::logger::error()<<"[NeetlinkSocket] msg send failed: "<<strerror(errno)<<endl;
       return -1;
    }
    return 1;
}

ssize_t libNetlink::netlink_socket::receive(void* buf){
    struct nlmsghdr* msg = reinterpret_cast<struct nlmsghdr *>(buf);
    memset(msg,0,NLMSG_SPACE(MAX_PAYLOAD));
    ssize_t size = recv(this->socketFd_,reinterpret_cast<char*>(msg),MAX_PAYLOAD,0);
    return size;
}

int libNetlink::netlink_socket::shutdown(){
    return close(this->socketFd_);
}

void libNetlink::netlink_socket::setOnDataReceiveHandler(std::function<void(int,char*,ssize_t)> handler){
    this->handler_ = handler;
}

DISABLE_WARNING_POP