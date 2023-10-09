
/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_uart.h"
#include "ql_gpio.h"
#include "ql_pin_cfg.h"
#include "ql_usb.h"

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

#define QL_USB_PRINTER_ENABLE	0


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

/*===========================================================================
                             Functions
 ===========================================================================*/

const uint8_t g_polynom = 0x31;

static uint8_t crc8 (uint8_t data[], int len)
{
    // initialization value
    uint8_t crc = 0xff;
    
    // iterate over all bytes
    for (int i = 0; i < len; i++)
    {
        crc ^= data[i];  
    
        for (int i = 0; i < 8; i++)
        {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}

void ql_uart_notify_cb(unsigned int ind_type, ql_uart_port_number_e port, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_UART_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    int read_len = 0;


    QL_UART_DEMO_LOG("UART port %d receive ind type:0x%x, receive data size:%d", port, ind_type, size);
    switch(ind_type)
    {
        case QUEC_UART_RX_OVERFLOW_IND:  //rx buffer overflow
    
        case QUEC_UART_RX_RECV_DATA_IND:
        {	
            while(size > 0)
            {
                memset(recv_buff, 0, QL_UART_RX_BUFF_SIZE + 1);
                real_size= MIN(size, QL_UART_RX_BUFF_SIZE);
                read_len = ql_uart_read(port, recv_buff, real_size);
                 
                QL_UART_DEMO_LOG("read_len=%d, recv_data=%s", read_len, recv_buff);

                if(recv_buff[5] == crc8(recv_buff, 5))
                {   
                    if(recv_buff[0] == HEADER_HIGH_BYTE_VERSION && recv_buff[1] == HEADER_LOW_BYTE_VERSION && recv_buff[2] == HEADER_BYTE_HOST)
                    {
                        if(recv_buff[3] == VER_CMD)
                        {
                            switch (recv_buff[4])
                            {
                            case 0x01:
                                QL_UART_DEMO_LOG("CASE 1\n");
                                break;
                            case 0x02:
                                QL_UART_DEMO_LOG("CASE 2\n");
                                break;
                            case 0x03:
                                QL_UART_DEMO_LOG("CASE 3\n");
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
				
                if((read_len > 0) && (size >= read_len))
                {
                    size -= read_len;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        case QUEC_UART_TX_FIFO_COMPLETE_IND: 
        {
            QL_UART_DEMO_LOG("tx fifo complete");
            break;
        }
    }
    free(recv_buff);
    recv_buff = NULL;
}

static void ql_uart_recv_task(void *param)
{
    ql_uart_register_cb(QL_UART_PORT_1, ql_uart_notify_cb);  
    // ql_rtos_task_sleep_ms(1000);
    for(;;)
    {
        ql_rtos_task_sleep_ms(1000);

    }
}

static void ql_uart_send_task(void *param)
{	
	ql_uart_tx_status_e tx_status;
	int write_len = 0;

	unsigned char data_send[9] = {HEADER_HIGH_BYTE_VERSION, HEADER_LOW_BYTE_VERSION, HEADER_BYTE_SLAVE, 
                                  VER_DATA, ID_DATA, DATA_LEN, HIGH_BYTE_DATA, LOW_BYTE_DATA, CRC};

    uint8_t data_example = 19;   //example
    char data_hex[3];

    snprintf(data_hex, sizeof(data_hex), "%02X", data_example);
    data_send[7] = strtol(data_hex, NULL, 16);
    data_send[8] = crc8(data_send, 8);

    unsigned char cmd[6] = {HEADER_HIGH_BYTE_VERSION, HEADER_LOW_BYTE_VERSION, HEADER_BYTE_HOST, VER_CMD, ID_CMD, CRC};
    cmd[5] = crc8(cmd, 5);  

	for(;;)
	{   
		write_len = ql_uart_write(QL_UART_PORT_1, data_send, sizeof(data_send));
	    QL_UART_DEMO_LOG("write_len:%d", write_len);

        // write_len = ql_uart_write(QL_UART_PORT_1, cmd, sizeof(cmd));
	    // QL_UART_DEMO_LOG("write_len:%d", write_len);

        ql_uart_get_tx_fifo_status(QL_UART_PORT_1, &tx_status);
        QL_UART_DEMO_LOG("tx_status:%d", tx_status);

	    ql_rtos_task_sleep_ms(10000);
	}
}

void ql_uart_init()
{
	int ret = 0;
	QlOSStatus err = 0;
	ql_uart_config_s uart_cfg = {0};

	uart_cfg.baudrate = QL_UART_BAUD_115200;
    uart_cfg.flow_ctrl = QL_FC_NONE;
    uart_cfg.data_bit = QL_UART_DATABIT_8;
    uart_cfg.stop_bit = QL_UART_STOP_1;
    uart_cfg.parity_bit = QL_UART_PARITY_NONE;

	ret = ql_uart_set_dcbconfig(QL_UART_PORT_1, &uart_cfg);
    QL_UART_DEMO_LOG("ret: 0x%x", ret);

    memset(&uart_cfg, 0, sizeof(ql_uart_config_s));
	ql_uart_get_dcbconfig(QL_UART_PORT_1, &uart_cfg);

    QL_UART_DEMO_LOG("baudrate=%d, flow_ctrl=%d, data_bit=%d, stop_bit=%d, parity_bit=%d",uart_cfg.baudrate, uart_cfg.flow_ctrl, uart_cfg.data_bit, uart_cfg.stop_bit, uart_cfg.parity_bit);

	if(QL_UART_SUCCESS != ret)
	{
	    err = ql_rtos_task_delete(NULL);
	    if(err != QL_OSI_SUCCESS)
	    {
	    	QL_UART_DEMO_LOG("task deleted failed");
	    }
	}
	ret = ql_uart_open(QL_UART_PORT_1);
    QL_UART_DEMO_LOG("ret: 0x%x", ret);
}

void ql_uart_app_init(void)
{
    QuecOSStatus err;
    QuecOSStatus er;

	ql_task_t uart_send_task = NULL;
    ql_task_t uart_recv_task = NULL;

	ql_uart_init();
    
	
    err = ql_rtos_task_create(&uart_send_task, 1024 * 5, 13, "QUARTDE", ql_uart_send_task, NULL, 7);
    if (err != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("demo task created failed");
        return;
	}
    else
    {
        QL_UART_DEMO_LOG("demo task created successful");
    }

    er = ql_rtos_task_create(&uart_recv_task, 16 * 1024, 14, "QUARTDEMO", ql_uart_recv_task, NULL, 6);
    if (er != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("demo task created failed");
        return;
	}
    else
    {
        QL_UART_DEMO_LOG("demo task created successful");
    }

}


