#pragma once
#include "device_handler.h"
#include "libserialport.h"
#include <stdint.h>

struct serial_port_list {
    struct sp_port *port;
    struct serial_port_list *next;
};

enum packet_type {
    ON,
    OFF,
    GET
};

struct control_packet {
    enum packet_type type;
    uint8_t pin;
    char *sensor;
    char *model;
};

struct serial_port_list *get_device_port_list();

int send_packet_to_device(struct sp_port *port, struct control_packet packet, char **response);
struct control_packet control_packet(enum packet_type type, uint8_t pin, char *sensor, char *model);
void free_serial_port_list(struct serial_port_list *head);