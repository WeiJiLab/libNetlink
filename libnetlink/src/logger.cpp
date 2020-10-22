#include "logger.h"
#include <fstream>
#include <iostream>
#include <time.h>

libNetlink::common::logger *libNetlink::common::logger::logger_ = nullptr;
libNetlink::common::log_config libNetlink::common::logger::config_;

const char *g_log_level_name[8] = {
        "Info", "Debug", "Error", "Warn", "Fatal"};
libNetlink::common::logger::logger() {
}

libNetlink::common::logger::~logger() {
}

void libNetlink::common::logger::writeTime() {
    char logStr[2048] = {'\0'};
    time_t t = time(0);
    char date[32] = {'\0'};
    strftime(date, sizeof(date), "[%Y-%m-%d %H:%M:%S]", localtime(&t));
    sprintf(logStr, "%s [%s]: ", date, g_log_level_name[this->level_]);
    std::cout << logStr;
}

void libNetlink::common::logger::write(std::string log) {
    std::cout << log;
}