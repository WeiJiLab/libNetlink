# libNetlink
this is a library for operating linux netlink, and subscibe netlink events.

### how to use
if you want to net package with special fwmark lookup some route table, like below:

for example : `ip rule add fwmark 0xFF/0xFFFF lookup 200`

```cpp
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
```
