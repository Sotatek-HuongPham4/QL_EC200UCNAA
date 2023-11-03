#include "bee_mqtt.h"
#include "bee_uart.h"

#include "ql_api_osi.h"
#include "ql_api_nw.h"
#include "ql_api_dev.h"

#include "ql_log.h"
#include "ql_api_datacall.h"

ql_task_t uart_send_task = NULL;
ql_task_t uart_recv_task = NULL;

ql_task_t mqtt_task_1 = NULL;
ql_task_t mqtt_task_2 = NULL;

int bee_main(){
    
    printf("Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello");
    // ql_uart_init(); 
    // ql_rtos_task_create(&uart_send_task, 1024 * 4, APP_PRIORITY_NORMAL, "UART_SEND", ql_uart_send_task, NULL, 7);
    // ql_rtos_task_create(&uart_recv_task, 5 * 1024, APP_PRIORITY_ABOVE_NORMAL, "UART_RECV", ql_uart_recv_task, NULL, 8);
    printf("==========Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello==========");
    ql_mqtt_init();
    ql_rtos_task_create(&mqtt_task_1, 16 * 1024, APP_PRIORITY_ABOVE_NORMAL, "MQTT_PUB", ql_mqtt_pub_data_task, NULL, 7);
    ql_rtos_task_create(&mqtt_task_2, 10 * 1024, APP_PRIORITY_NORMAL, "MQTT_SUB", ql_mqtt_sub_cmd_task, NULL, 10); 

    return 0;
}