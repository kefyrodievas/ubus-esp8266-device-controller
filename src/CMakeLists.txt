set(APP_NAME "esp_controller")

add_executable(
    ${APP_NAME}
        "src/main.c"
        src/device_handler.c
        src/ubus_func.c
)

target_link_libraries(
    ${APP_NAME}
    PUBLIC
        blobmsg_json
        serialport
        ubox
        ubus   
)

target_include_directories(
    ${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

install(TARGETS ${APP_NAME} DESTINATION /usr/bin)