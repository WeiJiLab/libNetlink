#include <netlink_listener.h>
#include <iostream>
#include "logger.h"

libNetlink::netlink_listener::netlink_listener(){
    this->setOnDataReceiveHandler(std::bind(&netlink_listener::onDataAvaliable, this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
}
    
libNetlink::netlink_listener::~netlink_listener(){
}

void libNetlink::netlink_listener::onDataAvaliable(int , char* buf, ssize_t size){
    common::logger::info()<<"[NetlinkListener] netlink message come." << endl;
    std::shared_ptr<netlink_event> event = std::make_shared<netlink_event>();

    if(!event->parseNetLinkMessage(buf, size)){
        common::logger::error()<<"[NetlinkListener] netlink message parse failed." << endl;
    }else{
        this->onEventHandler_(event);
    }
}


void libNetlink::netlink_listener::setOnEventHandler(std::function<void(std::shared_ptr<netlink_event>)> handler){
    this->onEventHandler_ = handler;
}

void libNetlink::netlink_listener::listen(){
    if(this->binding()==-1){
        exit(0);
    };
    common::logger::info()<<"[NetlinkListener] start listen."<<endl;
    this->running_ = true;
    while(this->running_){
        this->acceptAndListen();
    }
}

void libNetlink::netlink_listener::stopListen(){
    this->running_ = false;
}