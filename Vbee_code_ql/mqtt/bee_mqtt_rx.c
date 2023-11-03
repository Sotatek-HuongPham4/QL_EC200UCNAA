#include <stdio.h>
#include <stdint.h>

#include "cJSON.h"
#include "bee_mqtt_rx.h"
#include "bee_mqtt_define_values.h"


void ql_mqtt_parse_cmd(const char *cmd)
{
    cJSON *root = cJSON_Parse(cmd);
    char* cThing_token = cJSON_GetObjectItemCaseSensitive(root, "thing_token")->valuestring;
    char* cCmd_name = cJSON_GetObjectItemCaseSensitive(root, "cmd_name")->valuestring;
    char* cObject_type = cJSON_GetObjectItemCaseSensitive(root, "object_type")->valuestring;
    int values = cJSON_GetObjectItemCaseSensitive(root, "values")->valueint;
    int trans_code = cJSON_GetObjectItemCaseSensitive(root, "trans_code")->valueint;
    
    QL_MQTT_LOG("thing_token: %s", cThing_token);
	QL_MQTT_LOG("cmd_name: %s", cCmd_name);
	QL_MQTT_LOG("object_type: %s", cObject_type);
	QL_MQTT_LOG("values: %d", values);
	QL_MQTT_LOG("trans_code: %d", trans_code);

    cJSON_Delete(root);
}
