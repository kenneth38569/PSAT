#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "gps_parser.h"

// Buffer to accumulate GPS sentences
char gps_buffer[512] = {0};
int buffer_pos = 0;

void app_main() {

// Setup UART buffered IO with event queue
const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;
// Install UART driver
ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

const uart_port_t uart_num = UART_NUM_0;
uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};
ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

GPS_Data gps_data = {0};

//Read data from UART
while(1) {
    uint8_t data[128];
    int length = 0;
    length = uart_read_bytes(uart_num, data, 128, 100);
    
    if(length > 0) {
        // Add data to buffer
        for(int i = 0; i < length; i++) {
            gps_buffer[buffer_pos++] = data[i];
            
            // Check if we have a complete sentence (ends with newline)
            if(data[i] == '\n' && buffer_pos > 0) {
                gps_buffer[buffer_pos] = '\0';  // Null terminate
                
                // Check if it's a GNGGA sentence
                if(strstr(gps_buffer, "$GNGGA") != NULL) {
                    parse_gngga(gps_buffer, &gps_data);
                    print_gps_data(&gps_data);
                }
                
                // Reset buffer
                buffer_pos = 0;
                memset(gps_buffer, 0, sizeof(gps_buffer));
            }
        }
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}