/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**In picow_bt_example_freertos.c, modify the packet handler to log when the number of connections has changed.
Hint: The different types of events and helpers to extract event data from the packet are located in bstack_config.h
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "btstack.h"
#include "btstack_event.h"
#include "pico/cyw43_arch.h"
#include "picow_bt_example_common.h"

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

int btstack_main(int argc, const char * argv[]);
static btstack_packet_callback_registration_t hci_event_callback_registration;

static uint8_t connection_count = 0; // Track the number of active connections

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
            break;

        case HCI_EVENT_CONNECTION_COMPLETE:
            if (packet[2] == 0) { // Check status (0 indicates success)
                connection_count++;
                printf("Connection established. Total connections: %d\n", connection_count);
            } else {
                printf("Connection failed with status: 0x%02x\n", packet[2]);
            }
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            if (packet[2] == 0) { // Check status (0 indicates success)
                if (connection_count > 0) {
                    connection_count--;
                }
                printf("Connection terminated. Total connections: %d\n", connection_count);
            } else {
                printf("Disconnection failed with status: 0x%02x\n", packet[2]);
            }
            break;
        // this is working as well 
        
        // case BTSTACK_EVENT_NR_CONNECTIONS_CHANGED:
        // printf("Total connections: %d\n",  btstack_event_nr_connections_changed_get_number_connections(packet));
        // break;

        default:
            break;
    }
}


// static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
//     UNUSED(size);
//     UNUSED(channel);
//     bd_addr_t local_addr;
//     if (packet_type != HCI_EVENT_PACKET) return;
//     switch(hci_event_packet_get_type(packet)){
//         case BTSTACK_EVENT_STATE:
//             if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
//             gap_local_bd_addr(local_addr);
//             printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
//             break;
//         default:
//             break;
//     }
// }

void main_task(__unused void *params)
{
    // initialize CYW43 driver architecture
    // (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
    } else {
        // inform about BTstack state
        hci_event_callback_registration.callback = &packet_handler;
        hci_add_event_handler(&hci_event_callback_registration);
        btstack_main(0, NULL);
    }

    while(true) {
        vTaskDelay(1000);
    }
}

int main()
{
    stdio_init_all();
    TaskHandle_t task;
    xTaskCreate(main_task, "TestMainThread", 1024, NULL, TEST_TASK_PRIORITY, &task);
    vTaskStartScheduler();
    return 0;
}
