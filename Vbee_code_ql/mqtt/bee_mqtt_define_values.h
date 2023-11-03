#ifndef BEE_MQTT_DEFINE_VALUES_H
#define BEE_MQTT_DEFINE_VALUES_H
   
#include "ql_log.h"

#define QL_MQTT_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_MQTT_LOG(msg, ...)			QL_LOG(QL_MQTT_LOG_LEVEL, "ql_MQTT", msg, ##__VA_ARGS__)
#define QL_MQTT_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_MQTT", msg, ##__VA_ARGS__)

 
#define FLAG_TEM 0x01
#define FLAG_HUM 0x02

#endif