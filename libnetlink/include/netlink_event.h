#ifndef __INCLUDE_NETLINK_EVENT_H__
#define __INCLUDE_NETLINK_EVENT_H__

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace libNetlink
{
    enum class Action {
        Unknown = 0,
        Add = 1,
        Remove = 2,
        Change = 3,
        LinkUp = 4,
        LinkDown = 5,
        AddressUpdated = 6,
        AddressRemoved = 7,
        Rdnss = 8,
        RouteUpdated = 9,
        RouteRemoved = 10,
    };
    class netlink_event
    {
    private:
        Action action_;
        char* params_[32];

        bool parseInterfaceInfoInfoMessage(struct nlmsghdr *hdr);
        bool parseInterafaceAddressMessage(struct nlmsghdr *hdr);   
        bool parseRouteMessage(struct nlmsghdr *hdr);
        bool parseRuleMessage(struct nlmsghdr *hdr);

    public:
        bool parseNetLinkMessage(char* buffer, ssize_t size);
        Action getAction(){return this->action_;}

        netlink_event();
        ~netlink_event();
    };    
} // namespace libNetlink


#endif //!__INCLUDE_NETLINK_EVENT_H__