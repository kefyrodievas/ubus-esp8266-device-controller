#include "libserialport.h"
#include "device_handler.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <libubox/blobmsg_json.h>



struct serial_port_list *get_device_port_list() {
    struct serial_port_list *head = NULL, *tail = NULL;
    struct sp_port **port_list;
    enum sp_return result = sp_list_ports(&port_list);

    if (result != SP_OK) {
        printf("sp_list_ports() failed!\n");
        return NULL;
    }

    for (int i = 0; port_list[i] != NULL; i++) {
        struct sp_port *port = port_list[i];
        int vid, pid;
        char *name = sp_get_port_name(port);
        sp_get_port_usb_vid_pid(port, &vid, &pid);
        enum sp_transport transport = sp_get_port_transport(port);
        if (transport == SP_TRANSPORT_USB) {
            if (vid == DEVICE_VID && pid == DEVICE_PID) {
                struct sp_port *port_copy;
                sp_copy_port(port, &port_copy);
                if (head == NULL) {
                    head = (struct serial_port_list *)malloc(sizeof(struct serial_port_list));

                    head->port = port_copy;
                    head->next = NULL;
                    tail = head;
                }
                else {
                    tail->next = (struct serial_port_list *)malloc(sizeof(struct serial_port_list));
                    tail = tail->next;
                    tail->port = port_copy;
                    tail->next = NULL;
                }
            }
        }
    }
    sp_free_port_list(port_list);
    return head;
}

int send_packet_to_device(struct sp_port *port, struct control_packet packet, char **response) {
    // port setup
    if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK) {
        return -1;
    }
    if (sp_set_baudrate(port, 9600) != SP_OK) {
        return -1;
    }
    if (sp_set_bits(port, 8) != SP_OK) {
        return -1;
    }
    if (sp_set_parity(port, SP_PARITY_NONE) != SP_OK) {
        return -1;
    }
    if (sp_set_stopbits(port, 1) != SP_OK) {
        return -1;
    }
    if (sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE) != SP_OK) {
        return -1;
    }

    char *types[] = {
        "on", "off", "get"
    };
    // char message[80];
    static struct blob_buf buffer;
    blobmsg_buf_init(&buffer);
    switch (packet.type) {
    case GET:
        blobmsg_add_string(&buffer, "sensor", packet.sensor);
        blobmsg_add_string(&buffer, "model", packet.model);
    case ON:
    case OFF:
        blobmsg_add_string(&buffer, "action", types[packet.type]);
        blobmsg_add_u16(&buffer, "pin", packet.pin);
        break;
    default:
        sp_close(port);
        blob_buf_free(&buffer);
        return -2;
        break;
    }

    char *input = blobmsg_format_json(buffer.head, true);
    sp_nonblocking_write(port, input, buffer.buflen);
    free(input);
    char *buf = malloc(256 * sizeof(char));

    sp_blocking_read(port, buf, 256, 3000);
    // printf("%s", res);
    *response = buf;
    blob_buf_free(&buffer);
    sp_close(port);
    return 0;
}

struct control_packet control_packet(enum packet_type type, uint8_t pin, char *sensor, char *model) {
    struct control_packet packet = { .type = type, .pin = pin, .sensor = sensor, .model = model };
    return packet;
}

void free_serial_port_list(struct serial_port_list *head) {
    if (head == NULL) return;
    struct serial_port_list *curr = head;
    while (1) {
        curr = curr->next;
        sp_free_port(head->port);
        free(head);
        head = curr;
        if (head == NULL && curr == NULL) return;
    }
}