/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
=================================================================*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "bee_mqtt.h"
#include "bee_mqtt_tx.h"
#include "bee_mqtt_rx.h"
#include "bee_mqtt_define_values.h"


/******************************************************************/

#include "ql_api_osi.h"
#include "ql_api_nw.h"
#include "ql_api_dev.h"


#include "ql_api_datacall.h"
#include "ql_mqttclient.h"



/******************************************************************/
mqtt_client_t  		mqtt_cli;
static ql_sem_t		mqtt_semp = NULL;
static ql_queue_t 	mqtt_queue = NULL;

static char cTopic_pub[64] = "VB/DMP/VBEEON/CUSTOM/SMH/DeviceID/Telemetry";
static char cTopic_sub[64] = "VB/DMP/VBEEON/CUSTOM/SMH/DeviceID/Command";
static char u8mac[6];
char cMAC_address[13];

static uint8_t mqtt_connected = 0;

/******************************************************************/
// void ql_sub_cmd();
// void cmd_recv();

static void mqtt_state_exception_cb(mqtt_client_t *client)
{
	QL_MQTT_LOG("mqtt session abnormal disconnect");
	mqtt_connected = 0;
}

static void mqtt_connect_result_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_e status)
{
	QL_MQTT_LOG("status: %d", status);
	if(status == 0){
		mqtt_connected = 1;
	}
	ql_rtos_semaphore_release(mqtt_semp);
}

static void mqtt_requst_result_cb(mqtt_client_t *client, void *arg, int err)
{
	QL_MQTT_LOG("err: %d", err);
	ql_rtos_semaphore_release(mqtt_semp);
}

static void mqtt_inpub_data_cb(mqtt_client_t *client, void *arg, int pkt_id, const char *topic, const unsigned char *payload, unsigned short payload_len)
{
	QL_MQTT_LOG("=================topic=================: %s", topic);
	QL_MQTT_LOG("===================payload==============: %s", payload);
	// char arr_send_err[100];?
	if(payload != NULL)
	{
		if(QL_OSI_SUCCESS != ql_rtos_queue_release(mqtt_queue, sizeof(payload), (uint8 *)payload, 0))
		{
			QL_MQTT_LOG("Queue send err");
		}
		else 
		{	
			QL_MQTT_LOG("Queue send successful");
			QL_MQTT_LOG("+++++++++++++%s+++++++++++++", payload);
		}
	}

}
/******************************************************************/
static void ql_get_mac_device()
{
	ql_dev_get_mac_address(u8mac, 6);
	snprintf(cMAC_address, sizeof(cMAC_address), "%02X%02X%02X%02X%02X%02X", u8mac[0], u8mac[1], u8mac[2], u8mac[3], u8mac[4], u8mac[5]);
	snprintf(cTopic_pub, sizeof(cTopic_pub), "VB/DMP/VBEEON/CUSTOM/SMH/%s/telemetry", cMAC_address);
    snprintf(cTopic_sub, sizeof(cTopic_sub),"VB/DMP/VBEEON/CUSTOM/SMH/%s/Command", cMAC_address);
}


static void ql_sim_init()
{
	int ret = 0;
	int profile_idx = 1;
	ql_data_call_info_s info;
	uint8_t nSim = 0;
	uint16_t sim_cid;
	char ip4_addr_str[16] = {0};

	ql_rtos_semaphore_create(&mqtt_semp, 0);
	ql_rtos_task_sleep_s(10);
	while((ret = ql_network_register_wait(nSim, 120)) != 0){
		ql_rtos_task_sleep_s(1);
	}

	if(ret == 0){
		QL_MQTT_LOG("====network registered!!!!====");
	}else{
		QL_MQTT_LOG("====network register failure!!!!!====");
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_MQTT_LOG("===start data call====");
	ret = ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_MQTT_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_MQTT_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_MQTT_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
	}
		
    QL_MQTT_LOG("info->profile_idx: %d", info.profile_idx);
	QL_MQTT_LOG("info->ip_version: %d", info.ip_version);
            
	QL_MQTT_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MQTT_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MQTT_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MQTT_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);

	if(QL_DATACALL_SUCCESS != ql_bind_sim_and_profile(nSim, profile_idx, &sim_cid))
	{
		QL_MQTT_LOG("nSim or profile_idx is invalid!!!!");
	}
	QL_MQTT_LOG("%d",sim_cid);
	if(ql_mqtt_client_init(&mqtt_cli, sim_cid) != MQTTCLIENT_SUCCESS){
		QL_MQTT_LOG("mqtt client init failed!!!!");
	}
	QL_MQTT_LOG("mqtt_cli:%d", mqtt_cli);
}

// EXAMPLE TPYE DATA PUBLISH:
#define  DATA 	0x00
// CONTROL 	0x01
/******************************************************************/
static void ql_pub_data (uint8_t u8Type, uint8_t u8Flag_data, uint8_t u8Values)
{
	char *message_json_publish;
	switch (u8Type)
	{
	case 0x00:
		message_json_publish =  create_json_data_pub(u8Flag_data, u8Values);
		break;
	case 0x01: 
		break;
	default:
		break;
	}

	// ql_mqtt_publish(&mqtt_cli, cTopic_pub, message_json_publish, strlen(message_json_publish), 0, 0, mqtt_requst_result_cb, NULL);

    if(ql_mqtt_publish(&mqtt_cli, cTopic_pub, message_json_publish, strlen(message_json_publish), 0, 0, mqtt_requst_result_cb, NULL) == MQTTCLIENT_WOUNDBLOCK)
	{	
		QL_MQTT_LOG("MQTT_PUBLISH SUCCESSFUL");
		QL_MQTT_LOG("======wait publish result");
    	ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
		
	}
	else
	{
		QL_MQTT_LOG("MQTT_PUBLISH FALSE");
	}
}

/***************************FUNTIONS***************************************/

void ql_mqtt_init()
{	
	ql_sim_init();
	ql_get_mac_device();
	int ret = 0;

	struct mqtt_connect_client_info_t  client_info = {0};

	client_info.keep_alive = 10;
    client_info.pkt_timeout = 5;
    client_info.retry_times = 3;
    client_info.clean_session = 1;
    client_info.will_qos = 0;
    client_info.will_retain = 0;
    client_info.will_topic = NULL;
    client_info.will_msg = NULL;
    client_info.client_id = MQTT_CLIENT_IDENTITY;
    client_info.client_user = MQTT_CLIENT_USER;
    client_info.client_pass = MQTT_CLIENT_PASS;
	client_info.ssl_cfg = NULL;

	ret = ql_mqtt_connect(&mqtt_cli, MQTT_CLIENT_QUECTEL_URL, mqtt_connect_result_cb, NULL, (const struct mqtt_connect_client_info_t *)&client_info, mqtt_state_exception_cb);

	if(ret  == MQTTCLIENT_WOUNDBLOCK){
			QL_MQTT_LOG("====wait connect result ===");
			ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			if(mqtt_connected == 0){
				ql_mqtt_client_deinit(&mqtt_cli);
			}
	}else{
		QL_MQTT_LOG("===mqtt connect failed==== ,ret = %d",ret);
	
	}
	
	ql_rtos_queue_create(&mqtt_queue, 1024, 20);

	if(ql_mqtt_sub_unsub(&mqtt_cli, cTopic_sub, 0, mqtt_requst_result_cb, NULL , 1) == MQTTCLIENT_WOUNDBLOCK){
		QL_MQTT_LOG("==================wait subscrible result================================");
		ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
	}

	ql_mqtt_set_inpub_callback(&mqtt_cli, mqtt_inpub_data_cb, NULL);
	
}


/**************************TASK*************************************/

void ql_mqtt_pub_data_task(void * arg)
{	
	for(;;) 
	{
		if(mqtt_connected == 1)
		{
			ql_pub_data( DATA, FLAG_TEM, 30);
		}
		ql_rtos_task_sleep_ms(10000);
	}
}


void ql_mqtt_sub_cmd_task(void *arg)
{


	for(;;)
	{	
		char cmd_recv[100];

		if(QL_OSI_SUCCESS != ql_rtos_queue_wait(mqtt_queue,(uint8_t *) cmd_recv, 100, 500)) // QL_WAIT_FOREVER
		{
			QL_MQTT_LOG("Queue recv err");
		}
		else
		{
			QL_MQTT_LOG("Queue recv successful");
			if(cmd_recv != NULL)
			{
				QL_MQTT_LOG("+==========QueueRECV=====+++%s+++=======QueueRECV============+", cmd_recv);	
				ql_mqtt_parse_cmd((const char*) cmd_recv);
			}
			else
			{
				QL_MQTT_LOG("ERRROOORRRR");
			}
		}
		ql_rtos_task_sleep_ms(2000);
	}
	ql_rtos_queue_delete(mqtt_queue);	
}


