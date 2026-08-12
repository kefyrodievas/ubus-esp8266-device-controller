#include <stdio.h>
#include "libserialport.h"
#include "device_handler.h"

int main() {
    // printf("Hello world!\n");
    struct serial_port_list *list = get_device_port_list();
    int i = 0;
    for (struct serial_port_list *node = list; node != NULL; node = node->next) {
        printf("%d\n", send_packet_to_device(node->port, control_packet(GET, 14, "dht", "dht11")));
        printf("Device %d, port: %s\n", i, sp_get_port_name(node->port));
        i++;
    }

}