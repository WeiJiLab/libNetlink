#include "netlink_msg.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_IMPLICIT_INT_CONVERSION
DISABLE_WARNING_SHORTEN_64_TO_32
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_SIGN_COMPARE
DISABLE_WARNING_OLD_STYLE_CAST
DISABLE_WARNING_CAST_ALIGN
DISABLE_WARNING_SIGN_CONVERSION

libNetlink::netlink_msg::netlink_msg(uint16_t flags, size_t maxBufLen){
    this->maxBufLen_ = maxBufLen;
    this->netlinkMessage_ = reinterpret_cast<struct nlmsghdr *>(malloc(NLMSG_SPACE(maxBufLen)));
    this->netlinkMessage_->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | flags;
    this->netlinkMessage_->nlmsg_pid = getpid();
    this->netlinkMessage_->nlmsg_seq = 1;
}
    
libNetlink::netlink_msg::~netlink_msg(){
    delete this->netlinkMessage_;
}

void  libNetlink::netlink_msg::addRoute(unsigned short action, struct rtmsg msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct rtmsg));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
}

 void  libNetlink::netlink_msg::addRule(unsigned short action, struct fib_rule_hdr msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct fib_rule_hdr));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct fib_rule_hdr));
 }

void  libNetlink::netlink_msg::addRouteNextHop(unsigned short action, struct rtnexthop msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct rtnexthop));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtnexthop));
}

void  libNetlink::netlink_msg::addRouteAttributeCacheInfo(unsigned short action, struct rta_cacheinfo msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct rta_cacheinfo));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rta_cacheinfo));
}

void  libNetlink::netlink_msg::addInterfaceAddress(unsigned short action, struct ifaddrmsg msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct ifaddrmsg));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
}

void  libNetlink::netlink_msg::addInterfaceAddressCacheInfo(unsigned short action, struct ifa_cacheinfo msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct rta_cacheinfo));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rta_cacheinfo));
}

void  libNetlink::netlink_msg::addNeighborDiscovery(unsigned short action, struct ndmsg msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct ndmsg));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct ndmsg));
}

void  libNetlink::netlink_msg::addNeighborDiscoveryAttributeCacheInfo(unsigned short action, struct nda_cacheinfo msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct nda_cacheinfo));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct nda_cacheinfo));
}

void  libNetlink::netlink_msg::addInterfaceInfo(unsigned short action, struct ifinfomsg msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct ifinfomsg));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
}

void  libNetlink::netlink_msg::addTrafficControl(unsigned short action, struct tcmsg msg){
    this->netlinkMessage_->nlmsg_type = action;
    memcpy(NLMSG_DATA(this->netlinkMessage_), &msg, sizeof(struct tcmsg));
    this->netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
}


void libNetlink::netlink_msg::addAttr(unsigned int type, void *data, size_t alen){
	int len;
	struct rtattr *rta;

	len = RTA_LENGTH(alen);

	if (NLMSG_ALIGN(this->netlinkMessage_->nlmsg_len) + RTA_ALIGN(len) > this->maxBufLen_){
		return;
    }

	rta = (struct rtattr *)(((char *)this->netlinkMessage_) + NLMSG_ALIGN(this->netlinkMessage_->nlmsg_len));
	rta->rta_type = type;
	rta->rta_len = len;

	if (data){
		memcpy(RTA_DATA(rta), data, alen);
    }

	this->netlinkMessage_->nlmsg_len = NLMSG_ALIGN(this->netlinkMessage_->nlmsg_len) + RTA_ALIGN(len);
	return;
}

void libNetlink::netlink_msg::addAttr16(unsigned int type, uint16_t data){
	return this->addAttr(type, &data, sizeof(uint16_t));
}

void libNetlink::netlink_msg::addAttr32(unsigned int type,uint32_t data){
	return this->addAttr(type, &data, sizeof(uint32_t));
}

nlmsghdr* libNetlink::netlink_msg::getNetLinkMessage(){
    return this->netlinkMessage_;
}

DISABLE_WARNING_POP