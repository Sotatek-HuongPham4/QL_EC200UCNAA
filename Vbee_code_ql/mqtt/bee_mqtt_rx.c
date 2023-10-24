
#include "cJSON.h"
#include "bee_mqtt_rx.h"
#include <stdio.h>
#include <stdint.h>

#include "ql_log.h"
#include "ql_api_datacall.h"

#define QL_MQTT_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_MQTT_LOG(msg, ...)			QL_LOG(QL_MQTT_LOG_LEVEL, "ql_MQTT", msg, ##__VA_ARGS__)
#define QL_MQTT_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_MQTT", msg, ##__VA_ARGS__)



void ql_mqtt_read_cmd(const char *cmd)
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


// if(payload == NULL) return;
	// else 
	// {	
	// 	const char *cmd = (char *)payload;
	// 	cJSON *root = cJSON_Parse(cmd);
		
	// 	if(root != NULL)
	// 	{
	// 		char* cThing_token = cJSON_GetObjectItemCaseSensitive(root, "thing_token")->valuestring;
    //         char* cCmd_name = cJSON_GetObjectItemCaseSensitive(root, "cmd_name")->valuestring;
    //         char* cObject_type = cJSON_GetObjectItemCaseSensitive(root, "object_type")->valuestring;
    //         int values = cJSON_GetObjectItemCaseSensitive(root, "values")->valueint;
    //         int trans_code = cJSON_GetObjectItemCaseSensitive(root, "trans_code")->valueint;

	// 		QL_MQTT_LOG("thing_token: %s", cThing_token);
	// 		QL_MQTT_LOG("cmd_name: %s", cCmd_name);
	// 		QL_MQTT_LOG("object_type: %s", cObject_type);
	// 		QL_MQTT_LOG("values: %d", values);
	// 		QL_MQTT_LOG("trans_code: %d", trans_code);

	// 		cJSON_Delete(root);
	// 	}
	// }