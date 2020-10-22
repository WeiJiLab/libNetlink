#ifndef __COMMON_LOG_H__
#define __COMMON_LOG_H__

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

static const std::string endl = "\n";

namespace libNetlink {
    namespace common {

        typedef enum log_level {
            INFO = 0,
            DEBUG,
            ERROR,
            WARN,
            FATAL
        } log_level;

        typedef struct log_config {
            std::string path;
        } log_config;

        class logger {
        private:
            std::string getLogFileName();
            std::ofstream *osWrite_;

            log_level level_;

            static logger *logger_;
            static log_config config_;

        public:
            logger();
            ~logger();

            void writeTime();
            void write(std::string log);

            static void config(log_config config) {
                config_ = config;
            }

            static logger *instance(log_level level) {
                if (!logger_) {
                    logger_ = new logger();
                    logger_->level_ = level;
                    return logger_;
                } else {
                    logger_->level_ = level;
                    return logger_;
                }
            }

            static logger &info() {
                logger &log = *instance(INFO);
                log.writeTime();
                return log;
            }

            static logger &debug() {
                logger &log = *instance(DEBUG);
                log.writeTime();
                return log;
            }

            static logger &error() {
                logger &log = *instance(ERROR);
                log.writeTime();
                return log;
            }

            static logger &warn() {
                logger &log = *instance(WARN);
                log.writeTime();
                return log;
            }

            static logger &fatal() {
                logger &log = *instance(FATAL);
                log.writeTime();
                return log;
            }

            std::ostringstream os;
        };

        template<typename T>
        logger &operator<<(libNetlink::common::logger &log, T const &value) {
            log.os.str(std::string());
            log.os << value;
            log.write(log.os.str());
            return log;
        }

    }// namespace common
}// namespace libNetlink

#endif//!__COMMON_LOG_H__