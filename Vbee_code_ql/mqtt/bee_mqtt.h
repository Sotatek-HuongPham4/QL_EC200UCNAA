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
#define MQTT_CLIENT_IDENTITY_TEST        	"quectel_test"
#define MQTT_CLIENT_USER_TEST            	"VBeeHome"
#define MQTT_CLIENT_PASS_TESS            	"123abcA@!"
#define MQTT_CLIENT_QUECTEL_URL_TEST 	  	"mqtt://61.28.238.97:1993"
#define MQTT_SUB_LEN						          1024


#define FLAG_TEM 0x01
#define FLAG_HUM 0x02

/*========================================================================
 *	function Definition
 *========================================================================*/	


void ql_mqtt_init();

void ql_mqtt_pub_data_task(void * arg);

void ql_mqtt_sub_cmd(void *arg);
		
#endif   /*DATACALL_DEMO_H*/

