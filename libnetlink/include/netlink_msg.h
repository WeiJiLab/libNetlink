#ifndef __INCLUDE_NETLINK_MSG_H__
#define __INCLUDE_NETLINK_MSG_H__

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
#include <linux/fib_rules.h>

namespace libNetlink
{

    class netlink_msg
    {
    private:
        struct nlmsghdr* netlinkMessage_;
        size_t maxBufLen_;
    public:
        netlink_msg(uint16_t flags, size_t maxBufLen);

        void  addRoute(unsigned short action, struct rtmsg msg);
        void  addRule(unsigned short action, struct fib_rule_hdr msg);
        void  addRouteNextHop(unsigned short action, struct rtnexthop msg);
        void  addRouteAttributeCacheInfo(unsigned short action, struct rta_cacheinfo msg);
        void  addInterfaceAddress(unsigned short action, struct ifaddrmsg msg);
        void  addInterfaceAddressCacheInfo(unsigned short action, struct ifa_cacheinfo msg);
        void  addNeighborDiscovery(unsigned short action, struct ndmsg msg);
        void  addNeighborDiscoveryAttributeCacheInfo(unsigned short action, struct nda_cacheinfo msg);
        void  addInterfaceInfo(unsigned short action, struct ifinfomsg msg);
        void  addTrafficControl(unsigned short action, struct tcmsg );

        void addAttr(unsigned int rtaType, void* buf, size_t bufLen);
        void addAttr16(unsigned int rtaType, uint16_t data);
        void addAttr32(unsigned int rtaType,uint32_t data);

        struct nlmsghdr* getNetLinkMessage();
        ~netlink_msg();
    };
} // namespace libNetlink


#endif //!__INCLUDE_NETLINK_MSG_H__