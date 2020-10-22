 #include "netlink_handler.h"
 #include "logger.h"
 #include <memory>

libNetlink::netlink_handler::netlink_handler(){
    this->setOnEventHandler(std::bind(&netlink_handler::onEvent,this,std::placeholders::_1));
}

libNetlink::netlink_handler::~netlink_handler(){}


void libNetlink::netlink_handler::onEvent(std::shared_ptr<netlink_event> evt){
    // yep, we got the netlink message action, so let's notify the user
    switch (evt->getAction()){
        case Action::Unknown:
            break;
        case Action::Add:
            break;
        case Action::Remove:
            break;
        case Action::Change:
            break;
        case Action::LinkUp:
            break;
        case Action::LinkDown:
            break;
        case Action::AddressUpdated:
            break;
        case Action::AddressRemoved:
            break;
        case Action::Rdnss:
            break;
        case Action::RouteUpdated:
            break;
        case Action::RouteRemoved:
            break;
        
        default:
            break;
    }
}

void libNetlink::netlink_handler::notifyInterfaceAdded(const std::string& ){

}

void libNetlink::netlink_handler::notifyInterfaceRemoved(const std::string& ){

}

void libNetlink::netlink_handler::notifyInterfaceChanged(const std::string& , bool ){

}

void libNetlink::netlink_handler::notifyInterfaceLinkChanged(const std::string& , bool ){

}

void libNetlink::netlink_handler::notifyAddressUpdated(const std::string& , const std::string& , int , int ){

}

void libNetlink::netlink_handler::notifyAddressRemoved(const std::string& , const std::string& , int , int ){

}
        
void libNetlink::netlink_handler::notifyInterfaceDnsServers(const std::string& , int64_t , const std::vector<std::string>& ){

}

void libNetlink::netlink_handler::notifyRouteChange(bool , const std::string& , const std::string& , const std::string& ){

}

void libNetlink::netlink_handler::start(){
    common::logger::info() << "[NetLinkHandler] start." << endl; 
    this->listen();
}

void libNetlink::netlink_handler::stop(){
    this->stopListen();
}