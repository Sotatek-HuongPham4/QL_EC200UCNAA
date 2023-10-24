
#ifndef BEE_MQTT_TX_H
#define BEE_MQTT_TX_H

#include <stdint.h>

#define FLAG_TEM 0x01
#define FLAG_HUM 0x02

char* create_json_data_pub(uint8_t u8Flag_data, uint8_t u8Values);

#endif