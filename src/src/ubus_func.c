#include "libubus.h"
#include "ubus_func.h"
#include "device_handler.h"
#include <libubox/blobmsg_json.h>

extern struct serial_port_list *list;

static int controller_on(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg);

static int controller_off(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg);


static int controller_get(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg);

static int controller_devices(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg);


enum {
    PORT_VALUE,
    PIN_VALUE,
    SENSOR_VALUE,
    MODEL_VALUE,
    __GET_MODE_MAX,
    __SET_MODE_MAX = SENSOR_VALUE
};



static const struct blobmsg_policy set_mode_policy[] = {
    [PORT_VALUE] = {.name = "port", .type = BLOBMSG_TYPE_STRING},
    [PIN_VALUE] = {.name = "pin", .type = BLOBMSG_TYPE_INT32},
};

static const struct blobmsg_policy get_policy[] = {
    [PORT_VALUE] = {.name = "port", .type = BLOBMSG_TYPE_STRING},
    [PIN_VALUE] = {.name = "pin", .type = BLOBMSG_TYPE_INT32},
    [SENSOR_VALUE] = {.name = "sensor", .type = BLOBMSG_TYPE_STRING},
    [MODEL_VALUE] = {.name = "model", .type = BLOBMSG_TYPE_STRING}
};

static const struct ubus_method controller_methods[] = {
    UBUS_METHOD("on", controller_on, set_mode_policy),
    UBUS_METHOD("off", controller_off, set_mode_policy),
    UBUS_METHOD("get", controller_get, get_policy),
    UBUS_METHOD_NOARG("devices", controller_devices),
};

static struct ubus_object_type controller_object_type = UBUS_OBJECT_TYPE("esp_controller", controller_methods);

struct ubus_object controller_object = {
    .name = "esp_controller",
    .type = &controller_object_type,
    .methods = controller_methods,
    .n_methods = ARRAY_SIZE(controller_methods),
};


static int controller_on(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method, struct blob_attr *msg) {

    struct blob_attr *tb[__SET_MODE_MAX];



    blobmsg_parse(set_mode_policy, __SET_MODE_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[PORT_VALUE] || !tb[PIN_VALUE]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }
    static struct blob_buf buffer;
    blob_buf_init(&buffer, 0);

    char *response;
    struct sp_port *port = NULL;
    for (struct serial_port_list *curr = list; curr != NULL; curr = curr->next) {
        if (!strcmp(blobmsg_get_string(tb[PORT_VALUE]), sp_get_port_name(curr->port))) {
            port = curr->port;
            break;
        }
    }

    if (port == NULL) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    int rc = send_packet_to_device(port, control_packet(ON, blobmsg_get_u32(tb[PIN_VALUE]), NULL, NULL), &response);
    blobmsg_add_json_from_string(&buffer, response);
    ubus_send_reply(ctx, req, buffer.head);
    blob_buf_free(&buffer);
    free(response);
    return 0;
}

static int controller_off(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method, struct blob_attr *msg) {

    struct blob_attr *tb[__SET_MODE_MAX];


    blobmsg_parse(set_mode_policy, __SET_MODE_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[PORT_VALUE] || !tb[PIN_VALUE]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }
    static struct blob_buf buffer;
    blob_buf_init(&buffer, 0);

    char *response;
    struct sp_port *port = NULL;
    for (struct serial_port_list *curr = list; curr != NULL; curr = curr->next) {
        if (!strcmp(blobmsg_get_string(tb[PORT_VALUE]), sp_get_port_name(curr->port))) {
            port = curr->port;
            break;
        }
    }

    if (port == NULL) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    int rc = send_packet_to_device(port, control_packet(OFF, blobmsg_get_u32(tb[PIN_VALUE]), NULL, NULL), &response);
    blobmsg_add_json_from_string(&buffer, response);
    ubus_send_reply(ctx, req, buffer.head);
    blob_buf_free(&buffer);
    free(response);
    return 0;
}

static int controller_get(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method, struct blob_attr *msg) {

    struct blob_attr *tb[__GET_MODE_MAX];


    blobmsg_parse(get_policy, __GET_MODE_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[PORT_VALUE] || !tb[PIN_VALUE] || !tb[SENSOR_VALUE] || !tb[MODEL_VALUE]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    static struct blob_buf buffer;
    blob_buf_init(&buffer, 0);
    char *response;
    struct sp_port *port = NULL;
    for (struct serial_port_list *curr = list; curr != NULL; curr = curr->next) {
        if (!strcmp(blobmsg_get_string(tb[PORT_VALUE]), sp_get_port_name(curr->port))) {
            port = curr->port;
            break;
        }
    }

    if (port == NULL) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    int rc = send_packet_to_device(port, control_packet(GET, blobmsg_get_u32(tb[PIN_VALUE]), blobmsg_get_string(tb[SENSOR_VALUE]), blobmsg_get_string(tb[MODEL_VALUE])), &response);
    blobmsg_add_json_from_string(&buffer, response);
    ubus_send_reply(ctx, req, buffer.head);

    blob_buf_free(&buffer);
    free(response);
    return 0;
}

static int controller_devices(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg) {
    static struct blob_buf buffer;
    void *tbl;
    void *elem;
    blob_buf_init(&buffer, 0);
    free_serial_port_list(list);
    list = get_device_port_list();
    tbl = blobmsg_open_array(&buffer, "list");
    for (struct serial_port_list *cur = list; cur != NULL; cur = cur->next) {
        elem = blobmsg_open_table(&buffer, "device");
        int vid, pid;
        sp_get_port_usb_vid_pid(cur->port, &vid, &pid);
        blobmsg_add_string(&buffer, "port", sp_get_port_name(cur->port));
        blobmsg_add_u16(&buffer, "vendor_id", vid);
        blobmsg_add_u16(&buffer, "product_id", pid);
        blobmsg_close_table(&buffer, elem);
    }
    blobmsg_close_array(&buffer, tbl);
    ubus_send_reply(ctx, req, buffer.head);
    blob_buf_free(&buffer);
    return 0;
}