#include <stdio.h>
#include "libserialport.h"
#include "device_handler.h"

enum pins {
    D0 = 16,
    D1 = 5,
    D2 = 4,
    D3 = 0,
    D4 = 2,
    D5 = 14,
    D6 = 12,
    D7 = 13,
    D8 = 15
};

int main() {
    // printf("Hello world!\n");
    struct serial_port_list *list = get_device_port_list();
    int i = 0;
    for (struct serial_port_list *node = list; node != NULL; node = node->next) {

        printf("%d\n", send_packet_to_device(node->port, control_packet(ON, D4, "dht", "dht11")));
        printf("%d\n", send_packet_to_device(node->port, control_packet(ON, D6, "dht", "dht11")));

        printf("Device %d, port: %s\n", i, sp_get_port_name(node->port));
        i++;
    }

}