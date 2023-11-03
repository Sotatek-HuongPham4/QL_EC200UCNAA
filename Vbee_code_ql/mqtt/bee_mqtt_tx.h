
#ifndef BEE_MQTT_TX_H
#define BEE_MQTT_TX_H

#include <stdint.h>

char* create_json_data_pub(uint8_t u8Flag_data, uint8_t u8Values);

char* create_json_control_pub(uint8_t u8Flag_control, uint8_t u8Value);

#endif