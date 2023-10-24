
#ifndef BEE_UART_H
#define BEE_UART_H

/*===========================================================================
 *Definition
 ===========================================================================*/
#define QL_UART_DEMO_LOG_LEVEL			QL_LOG_LEVEL_INFO
#define QL_UART_DEMO_LOG(msg, ...)		QL_LOG(QL_UART_DEMO_LOG_LEVEL, "ql_uart_demo", msg, ##__VA_ARGS__)


#define QL_UART_TASK_STACK_SIZE     		4096
#define QL_UART_TASK_PRIO          	 	    APP_PRIORITY_NORMAL
#define QL_UART_TASK_EVENT_CNT      		5


#define QL_UART_RX_BUFF_SIZE                2048
#define QL_UART_TX_BUFF_SIZE                2048

#define MIN(a,b) ((a) < (b) ? (a) : (b))


/*===========================================================================
 * Variate
 ===========================================================================*/
#define HEADER_HIGH_BYTE_VERSION 0x55
#define HEADER_LOW_BYTE_VERSION  0xaa
#define CRC                      0x00
/****************************SEND DATA**************************************/  

#define HEADER_BYTE_SLAVE        0x01
#define VER_DATA                 0x02   // can change
#define ID_DATA                  0x03   // can change
#define DATA_LEN                 0x02   // can change
#define HIGH_BYTE_DATA           0x00   // can change
#define LOW_BYTE_DATA            0x00   // can change
/****************************RECEIVE CMD**************************************/  
#define HEADER_BYTE_HOST         0x02
#define VER_CMD                  0x01   // can change
#define ID_CMD                   0x03   // can change

/****************************FRAME_DATA_RULE_UART****************************/ 
/*

	    Header_high_byte | Header_low_byte	| Header_byte_slave | Version_data	| ID_data | Data_length | High_byte_data | Low_byte_data | CRC
send_data	0x55	            0xAA	               0x01	            0x02	    0x03	    0x02	    0x00	            0x13	   0xBD


         Header_high_byte	Header_low_byte	   Header_byte_host	    Version_cmd	    ID_cmd	    CRC
recv_cmd	0x55	            0xAA	            0x02	            0x03	    0x03	    0x8A
        	0x55	            0xAA	            0x02	            0x01	    0x01	    31
	        0x55	            0xAA	            0x02	            0x01	    0x02	    62
	        0x55	            0xAA	            0x02	            0x01	    0x03	    53

*/
// ql_task_t uart_send_task = NULL;
// ql_task_t uart_recv_task = NULL;


void ql_uart_recv_task(void *param);
void ql_uart_send_task(void *param);
void ql_uart_init();
// void bee_ql_uart_app_init(void);


#endif /* UART_DEMO_H */


