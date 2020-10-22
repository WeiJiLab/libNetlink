#include <iostream>
#include <string.h>
#include <unistd.h>
#include "netlink_socket.h"
#include "netlink_msg.h"
#include <linux/netlink.h>
#include "netlink_handler.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <thread>

DISABLE_WARNING_PUSH
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_IMPLICIT_INT_CONVERSION
DISABLE_WARNING_SHORTEN_64_TO_32
DISABLE_WARNING_SIGN_CONVERSION
DISABLE_WARNING_SIGN_COMPARE
DISABLE_WARNING_OLD_STYLE_CAST
DISABLE_WARNING_CAST_ALIGN
DISABLE_WARNING_SIGN_CONVERSION

int main(/*int argc, char* argv[]*/) {
    libNetlink::netlink_handler handler;
    std::thread nlmanager([&]{
        handler.create(NETLINK_ROUTE);
        handler.start();
    });

    libNetlink::netlink_socket netLinker;
    netLinker.create(NETLINK_ROUTE); 
    libNetlink::netlink_msg nlmsg(NLM_F_CREATE | NLM_F_EXCL, 1024);
    struct fib_rule_hdr msg;
    msg.action = FR_ACT_TO_TBL;
    msg.family = AF_INET;
    nlmsg.addRule(RTM_NEWRULE, msg);
    nlmsg.addAttr32(FRA_FWMARK, 0xFF);
    nlmsg.addAttr32(FRA_FWMASK, 0xFFFF);
    msg.table = RT_TABLE_UNSPEC;
    nlmsg.addAttr32(FRA_TABLE, 200);
    
    netLinker.sendNetlinkMsgToKernel(nlmsg.getNetLinkMessage());  
    
    std::cout<<"ip rule add fwmark 0xFF/0xFFFF lookup 200"<<std::endl;

    nlmanager.join();
    return 0;
}


DISABLE_WARNING_POP