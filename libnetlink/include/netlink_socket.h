#ifndef __INCLUDE_NETLINK_SOCKET_H__
#define __INCLUDE_NETLINK_SOCKET_H__

#include <functional>
#include <memory>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace libNetlink
{
    class netlink_socket
    {
    private:
        std::function<void(int, char *,ssize_t)> handler_;
        int socketFd_;
    public:
        netlink_socket();
        virtual ~netlink_socket();

        void setSock(int sock){this->socketFd_ = sock;}
        
        int create(int protocol);
        int binding();

        int acceptAndListen();

        int sendNetlinkMsgToKernel(struct nlmsghdr* msg);

        ssize_t receive(void* buf);

        int shutdown();

        void setOnDataReceiveHandler(std::function<void(int,char*,ssize_t)> handler);

    private:
          /**
         * Link layer: RTM_NEWLINK, RTM_DELLINK, RTM_GETLINK, RTM_SETLINK
         * Address settings: RTM_NEWADDR, RTM_DELADDR, RTM_GETADDR
         * Routing tables: RTM_NEWROUTE, RTM_DELROUTE, RTM_GETROUTE
         * Neighbor cache: RTM_NEWNEIGH, RTM_DELNEIGH, RTM_GETNEIGH
         * Routing rules: RTM_NEWRULE, RTM_DELRULE, RTM_GETRULE
         * Queuing discipline settings: RTM_NEWQDISC, RTM_DELQDISC, RTM_GETQDISC
         * Traffic classes used with queues: RTM_NEWTCLASS, RTM_DELTCLASS, RTM_GETTCLASS
         * Traffic filters: RTM_NEWTFILTER, RTM_DELTFILTER, RTM_GETTFILTER
         * Others: RTM_NEWACTION, RTM_DELACTION, RTM_GETACTION, RTM_NEWPREFIX, RTM_GETPREFIX, RTM_GETMULTICAST, RTM_GETANYCAST, RTM_NEWNEIGHTBL, RTM_GETNEIGHTBL, RTM_SETNEIGHTBL
         */
        int send(unsigned short action,char *buffer,size_t size, unsigned short rtaType, char* attrBuf, size_t attrBufLen);
    };
} // namespace libNetlink

#endif //!__INCLUDE_NETLINK_SOCKET_H__