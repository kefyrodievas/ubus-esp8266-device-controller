#include <stdio.h>
#include "libserialport.h"
#include "device_handler.h"
#include "ubus_func.h"
#include "libubus.h"

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

struct serial_port_list *list;

int main() {
    list = get_device_port_list();
    // int i = 0;
    // for (struct serial_port_list *node = list; node != NULL; node = node->next) {
    //     char *rc;
    //     send_packet_to_device(node->port, control_packet(OFF, D4, "dht", "dht11"), &rc);
    //     printf("%s", rc);
    //     i++;
    // }


    struct ubus_context *ctx;

    uloop_init();

    ctx = ubus_connect(NULL);
    if (!ctx) {
        fprintf(stderr, "Failed to connect to ubus\n");
        return -1;
    }

    ubus_add_uloop(ctx);
    ubus_add_object(ctx, &controller_object);
    uloop_run();

    ubus_free(ctx);
    uloop_done();

    free_serial_port_list(list);

    return 0;
}