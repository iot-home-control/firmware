#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <functional>

class ota_handler
{
private:
    bool use_serial, run_default_handlers;
    void setup();
    void handle_on_start();
    void handle_on_end();
    void handle_on_error(ota_error_t error);
    void handle_on_progress(unsigned int current, unsigned int total);

public:
    typedef std::function<void()> callback;
    typedef std::function<void(unsigned int)> callback_progress;
    typedef std::function<void(ota_error_t)> callback_error;
    
    ota_handler();
    void set_hostname(const String& name, bool append_chip_id=true);
    void set_password(const String& password);
    void begin(bool use_serial=true, bool run_default_handers=true);
    void update();

    callback on_start;
    callback on_end;
    callback_error on_error;
    callback_progress on_progress;
};

#endif // OTA_HANDLER_H
