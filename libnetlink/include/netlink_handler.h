#ifndef __INCLUDE_NETLINK_HANDLER_H__
#define __INCLUDE_NETLINK_HANDLER_H__

#include "netlink_listener.h"
#include <vector>
#include "netlink_event.h"
#include <memory>


namespace libNetlink
{
    class netlink_handler: public netlink_listener
    {
    private:
        void onEvent(std::shared_ptr<netlink_event> evt);

        void notifyInterfaceAdded(const std::string& ifName);
        void notifyInterfaceRemoved(const std::string& ifName);
        void notifyInterfaceChanged(const std::string& ifName, bool isUp);
        void notifyInterfaceLinkChanged(const std::string& ifName, bool isUp);
        void notifyAddressUpdated(const std::string& addr, const std::string& ifName, int flags, int scope);
        void notifyAddressRemoved(const std::string& addr, const std::string& ifName, int flags, int scope);
        void notifyInterfaceDnsServers(const std::string& ifName, int64_t lifetime, const std::vector<std::string>& servers);
        void notifyRouteChange(bool updated, const std::string& route, const std::string& gateway, const std::string& ifName);
    public:
        void start();
        void stop();

        netlink_handler();
        ~netlink_handler();
    };
    
} // namespace libNetlink


#endif //!__INCLUDE_NETLINK_HANDLER_H__