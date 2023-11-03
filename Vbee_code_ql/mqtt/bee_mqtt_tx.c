#include "cJSON.h"
#include "bee_mqtt_tx.h"
#include "bee_mqtt_define_values.h"


uint8_t u8Trans_code = 0;
extern char cMAC_address[13];

char* create_json_data_pub(uint8_t u8Flag_data, uint8_t u8Values)
{
    cJSON *root;
	root = cJSON_CreateObject();
	
	cJSON_AddItemToObject(root, "thing_token", cJSON_CreateString(cMAC_address));
	cJSON_AddItemToObject(root, "cmd_name", cJSON_CreateString("Bee.data"));
	if ( u8Flag_data == FLAG_TEM)
    {
        cJSON_AddItemToObject(root, "object_type", cJSON_CreateString("temperature"));
    }
    else
    {
        cJSON_AddItemToObject(root, "object_type", cJSON_CreateString("humidity"));
    }

    cJSON_AddItemToObject(root, "values", cJSON_CreateNumber(u8Values));
    cJSON_AddItemToObject(root, "trans_code", cJSON_CreateNumber(++u8Trans_code));

	char *message_json_publish = cJSON_Print(root);
	 
    cJSON_Delete(root);

    return message_json_publish;
}
