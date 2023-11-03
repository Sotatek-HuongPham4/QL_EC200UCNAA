/**  
  @file
 
  @brief
  

*/
#ifndef BEE_MQTT_H
#define BEE_MQTT_H

#include <stdint.h>

/*========================================================================
 *	values Definition
 *========================================================================*/	
#define MQTT_CLIENT_IDENTITY             	"quectel_test"
#define MQTT_CLIENT_USER                 	"VBeeHome"
#define MQTT_CLIENT_PASS                	"123abcA@!"
#define MQTT_CLIENT_QUECTEL_URL      	  	"mqtt://61.28.238.97:1993"

/*========================================================================
 *	function Definition
 *========================================================================*/	

void ql_mqtt_init();

void ql_mqtt_pub_data_task(void * arg);

void ql_mqtt_sub_cmd_task(void *arg);
		
#endif   /*DATACALL_DEMO_H*/

