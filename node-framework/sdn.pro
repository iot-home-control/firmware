OTHER_FILES += \
    config.json \
    config.h.example \
    meson.build

HEADERS += \
    ticker_component.h \
    trampoline_magic.h \
    updater_http.h \
    updater_ota.h \
    wifi_connector.h \
    sensor_dht22.h \
    mqtt_handler.h \
    gpio_pin.h \
    config.h \
    led_strip.h \
    rotary_encoder.h \
    button.h

SOURCES += \
    node_led_strip.cpp \
    updater_http.cpp \
    updater_ota.cpp \
    wifi_connector.cpp \
    sensor_dht22.cpp \
    mqtt_handler.cpp \
    gpio_pin.cpp \
    led_strip.cpp \
    rotary_encoder.cpp \
    trampoline_magic.cpp \
    button.cpp
