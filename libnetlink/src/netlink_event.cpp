#include <netlink_event.h>
#include <iostream>
#include <string.h>
#include <warning_disable.h>
#include <linux/if.h>
// #include <net/if.h>
#include <linux/in6.h>
#include <logger.h>
#include <arpa/inet.h>

DISABLE_WARNING_PUSH
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_IMPLICIT_INT_CONVERSION
DISABLE_WARNING_SHORTEN_64_TO_32
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_SIGN_COMPARE
DISABLE_WARNING_OLD_STYLE_CAST
DISABLE_WARNING_CAST_ALIGN
DISABLE_WARNING_SIGN_CONVERSION

libNetlink::netlink_event::netlink_event(){
}
    
libNetlink::netlink_event::~netlink_event(){
}

const char* rtMessageName(int type) {
    switch (type){
    case RTM_NEWLINK:
        return "RTM_NEWLINK";
    case RTM_DELLINK:
        return "RTM_DELLINK";
    case RTM_NEWADDR:
        return "RTM_NEWADDR";
    case RTM_DELADDR:
        return "RTM_DELADDR";
    case RTM_NEWROUTE:
        return "RTM_NEWROUTE";
    case RTM_DELROUTE:
        return "RTM_DELROUTE";
    case RTM_NEWRULE:
        return "RTM_NEWRULE";
    case RTM_DELRULE:
        return "RTM_DELRULE";
    case RTM_NEWNDUSEROPT:
        return "RTM_NEWNDUSEROPT";
    default:
        return nullptr;
    }
}

bool libNetlink::netlink_event::parseInterfaceInfoInfoMessage(struct nlmsghdr *hdr){
    struct ifinfomsg *interfaceInfo = (struct ifinfomsg *) NLMSG_DATA(hdr);
    struct rtattr *rta;

    if(hdr->nlmsg_len < NLMSG_LENGTH(sizeof(*interfaceInfo))){
        return false;
    }

    int len = IFLA_PAYLOAD(hdr);
    for (rta = IFLA_RTA(interfaceInfo); RTA_OK(rta, len); rta = RTA_NEXT(rta, len)) {
        switch(rta->rta_type) {
            case IFLA_IFNAME:
                asprintf(&this->params_[0], "INTERFACE=%s", (char *) RTA_DATA(rta));
                asprintf(&this->params_[1], "IFINDEX=%d", interfaceInfo->ifi_index);
                this->action_ = (interfaceInfo->ifi_flags & IFF_LOWER_UP) ? Action::LinkUp : Action::LinkDown;
                return true;
        }
    }
    return false;
}

bool libNetlink::netlink_event::parseInterafaceAddressMessage(struct nlmsghdr *hdr){
    struct ifaddrmsg *interfaceAddress = (struct ifaddrmsg *) NLMSG_DATA(hdr);
    struct ifa_cacheinfo *cacheinfo = nullptr;
    // char ifname[IFNAMSIZ] = "";
    char addrstr[46] = "";
    uint32_t flags;

    if(hdr->nlmsg_len < NLMSG_LENGTH(sizeof(*interfaceAddress))){
        return false;
    }

    uint8_t type = hdr->nlmsg_type;
    if((type != RTM_NEWADDR) && (type !=RTM_DELADDR)){
        return false;
    }

    const char *msgtype = rtMessageName(type);
    struct rtattr *rta;
    int len = IFA_PAYLOAD(hdr);
    for (rta = IFA_RTA(interfaceAddress); RTA_OK(rta, len); rta = RTA_NEXT(rta, len)) {
         if (rta->rta_type == IFA_ADDRESS) {
            if (interfaceAddress->ifa_family == AF_INET) {
                struct in_addr *addr4 = (struct in_addr *) RTA_DATA(rta);
                if (RTA_PAYLOAD(rta) < sizeof(*addr4)) {
                    common::logger::error() << "[NetlinkEvent] Short IPv4 address  ("<<RTA_PAYLOAD(rta)<< " bytes) in "<< msgtype << endl;
                    continue;
                }
                inet_ntop(AF_INET, addr4, addrstr, sizeof(addrstr));
            } else if (interfaceAddress->ifa_family == AF_INET6) {
                struct in6_addr *addr6 = (struct in6_addr *) RTA_DATA(rta);
                if (RTA_PAYLOAD(rta) < sizeof(*addr6)) {
                    common::logger::error() << "[NetlinkEvent] Short IPv6 address  ("<<RTA_PAYLOAD(rta)<< " bytes) in "<< msgtype << endl;
                    continue;
                }
                inet_ntop(AF_INET6, addr6, addrstr, sizeof(addrstr));
            } else {
                common::logger::error()<<"[NetlinkEvent] Unknown address family "<< interfaceAddress->ifa_family<< endl;
                continue;
            }

            // if (!if_indextoname(interfaceAddress->ifa_index, ifname)) {
            //     common::logger::error() << "[NetlinkEvent] Unknown ifindex "<< interfaceAddress->ifa_index << " in " << msgtype << endl;
            // }
         } else if (rta->rta_type == IFA_CACHEINFO) {
            if (RTA_PAYLOAD(rta) < sizeof(*cacheinfo)) {
                common::logger::error() << "[NetlinkEvent] Short IFA_CACHEINFO ("<<RTA_PAYLOAD(rta)<<" vs."<<sizeof(cacheinfo)<<" bytes) in " << msgtype <<endl;
                continue;
            }
            cacheinfo = (struct ifa_cacheinfo *) RTA_DATA(rta);
        } else if (rta->rta_type == IFA_FLAGS) {
            flags = *(uint32_t*)RTA_DATA(rta);
        }
    }

    this->action_ = (type == RTM_NEWADDR) ? Action::AddressUpdated : Action::AddressRemoved;
    asprintf(&this->params_[0], "ADDRESS=%s/%d", addrstr, interfaceAddress->ifa_prefixlen);
    // asprintf(&this->params_[1], "INTERFACE=%s", ifname);
    asprintf(&this->params_[2], "FLAGS=%u", flags);
    asprintf(&this->params_[3], "SCOPE=%u", interfaceAddress->ifa_scope);
    asprintf(&this->params_[4], "IFINDEX=%u", interfaceAddress->ifa_index);

    if (cacheinfo) {
        asprintf(&this->params_[5], "PREFERRED=%u", cacheinfo->ifa_prefered);
        asprintf(&this->params_[6], "VALID=%u", cacheinfo->ifa_valid);
        asprintf(&this->params_[7], "CSTAMP=%u", cacheinfo->cstamp);
        asprintf(&this->params_[8], "TSTAMP=%u", cacheinfo->tstamp);
    }
    return true;
}

bool libNetlink::netlink_event::parseRuleMessage(struct nlmsghdr *){
    return false;
}

bool libNetlink::netlink_event::parseRouteMessage(struct nlmsghdr *hdr){
    uint8_t type = hdr->nlmsg_type;
    const char *msgname = rtMessageName(type);

    if (type != RTM_NEWROUTE && type != RTM_DELROUTE) {
        common::logger::error() << "[NetLinkEvent] incorrect message type "<< type <<":"<< msgname << endl;
        return false;
    }

    struct rtmsg *rtm = (struct rtmsg *) NLMSG_DATA(hdr);
    if(hdr->nlmsg_len < NLMSG_LENGTH(sizeof(*rtm))){
        common::logger::error() << "[NetLinkEvent] nl less than rt"<< endl;
        return false;
    }

    if (// Ignore static routes we've set up ourselves.
        (rtm->rtm_protocol != RTPROT_KERNEL &&
         rtm->rtm_protocol != RTPROT_RA) ||
        // We're only interested in global unicast routes.
        (rtm->rtm_scope != RT_SCOPE_UNIVERSE) ||
        (rtm->rtm_type != RTN_UNICAST) ||
        // We don't support source routing.
        (rtm->rtm_src_len != 0) ||
        // Cloned routes aren't real routes.
        (rtm->rtm_flags & RTM_F_CLONED)) {
        common::logger::warn() << "[NetLinkEvent] ignore some routes."<< endl;
        return false;
    }

    int family = rtm->rtm_family;
    int prefixLength = rtm->rtm_dst_len;

    // Currently we only support: destination, (one) next hop, ifindex.
    char dst[INET6_ADDRSTRLEN] = "";
    char gw[INET6_ADDRSTRLEN] = "";
    char dev[IFNAMSIZ] = "";

    size_t len = RTM_PAYLOAD(hdr);
    struct rtattr *rta;
    for (rta = RTM_RTA(rtm); RTA_OK(rta, len); rta = RTA_NEXT(rta, len)) {
        switch (rta->rta_type) {
            case RTA_DST:
                if (!inet_ntop(family, RTA_DATA(rta), dst, sizeof(dst))){
                    return false;
                }
                continue;
            case RTA_GATEWAY:
                if (!inet_ntop(family, RTA_DATA(rta), gw, sizeof(gw))){
                    return false;
                }
                continue;
            case RTA_OIF:
                // if (!if_indextoname(* (int *) RTA_DATA(rta), dev)){
                //     return false;
                // }
                continue;
            default:
                continue;
        }
    }

   // If there's no RTA_DST attribute, then:
   // - If the prefix length is zero, it's the default route.
   // - If the prefix length is nonzero, there's something we don't understand.
   //   Ignore the event.
   if (!*dst && !prefixLength) {
        if (family == AF_INET) {
            strncpy(dst, "0.0.0.0", sizeof(dst));
        } else if (family == AF_INET6) {
            strncpy(dst, "::", sizeof(dst));
        }
    }

    if (!*dst || (!*gw && !*dev)){
        return false;
    }

    this->action_ = (type == RTM_NEWROUTE) ? Action::RouteUpdated : Action::RouteRemoved;
    asprintf(&this->params_[0], "ROUTE=%s/%d", dst, prefixLength);
    asprintf(&this->params_[1], "GATEWAY=%s", (*gw) ? gw : "");
    asprintf(&this->params_[2], "INTERFACE=%s", (*dev) ? dev : "");

    return true;
}

bool libNetlink::netlink_event::parseNetLinkMessage(char* buffer, ssize_t size){
    struct nlmsghdr *nh;
     for (nh = (struct nlmsghdr *) buffer;
         NLMSG_OK(nh, (unsigned) size) && (nh->nlmsg_type != NLMSG_DONE);
         nh = NLMSG_NEXT(nh, size)) {
        
        const char *msgname = rtMessageName(nh->nlmsg_type);
        if (!msgname) {
            common::logger::error()<<"[NetlinkEvent] Unexpected netlink message type :"<< nh->nlmsg_type<< endl;
            continue;
        }
        common::logger::info()<<"[NetlinkEvent] got message: "<< msgname << endl;

        if (nh->nlmsg_type == RTM_NEWLINK) {
            if (parseInterfaceInfoInfoMessage(nh)){
                return true;
            }
        } else if (nh->nlmsg_type == RTM_NEWADDR || nh->nlmsg_type == RTM_DELADDR) {
            if (parseInterafaceAddressMessage(nh)){
                return true;
            }
        } else if (nh->nlmsg_type == RTM_NEWROUTE || nh->nlmsg_type == RTM_DELROUTE) {
            if (parseRouteMessage(nh)){
                return true;
            }
        } else if(nh->nlmsg_type == RTM_NEWRULE || nh->nlmsg_type == RTM_DELRULE){
            if (parseRuleMessage(nh)){
                return true;
            }
        } else {
            common::logger::info()<<"[NetlinkEvent] can not parse message type: "<< msgname << endl;
        }
    }
    return false;
}

DISABLE_WARNING_POP