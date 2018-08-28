#ifndef OT_STATIC_LOGGER_HPP_
#define OT_STATIC_LOGGER_HPP_

#include <ot/utility/logger.hpp>
#include <ot/policy.hpp>

namespace ot {

// Global declaration and macro usage.
inline Logger logger(env::log_file());

}; // end of namespace ot. ------------------------------------------------------------------------


#define OT_LOG_REMOVE_FIRST_HELPER(N, ...) __VA_ARGS__
#define OT_LOG_GET_FIRST_HELPER(N, ...) N
#define OT_LOG_GET_FIRST(...) OT_LOG_GET_FIRST_HELPER(__VA_ARGS__)
#define OT_LOG_REMOVE_FIRST(...) OT_LOG_REMOVE_FIRST_HELPER(__VA_ARGS__)

#define OT_LOGTO(...) ot::logger.redir  (__VA_ARGS__)
#define OT_LOG(...)   ot::logger.raw    (__VA_ARGS__)
#define OT_LOGD(...)  ot::logger.debug  (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define OT_LOGI(...)  ot::logger.info   (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define OT_LOGW(...)  ot::logger.warning(__FILE__, __LINE__, __VA_ARGS__, '\n')
#define OT_LOGE(...)  ot::logger.error  (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define OT_LOGF(...)  ot::logger.fatal  (__FILE__, __LINE__, __VA_ARGS__, '\n')

#define OT_LOG_IF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {          \
                         OT_LOG(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                       }

#define OT_LOGD_IF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          OT_LOGD(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define OT_LOGI_IF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          OT_LOGI(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define OT_LOGW_IF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          OT_LOGW(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define OT_LOGE_IF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          OT_LOGE(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define OT_LOGF_IF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                          OT_LOGF(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                        }

#define OT_LOGW_RIF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                           OT_LOGW(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                           return;                                     \
                         }

#define OT_LOGE_RIF(...) if(OT_LOG_GET_FIRST(__VA_ARGS__)) {           \
                           OT_LOGE(OT_LOG_REMOVE_FIRST(__VA_ARGS__));  \
                           return;                                     \
                         }

#endif






