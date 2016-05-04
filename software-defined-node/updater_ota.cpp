#include "updater_ota.h"

static updater_ota* updater_ota_instance=nullptr;

updater_ota::updater_ota(): use_serial(true), run_default_handlers(true)
{
    if(updater_ota_instance==nullptr)
        updater_ota_instance=this;
}

void updater_ota::setup()
{
    ArduinoOTA.onStart([]()
    {
        updater_ota_instance->handle_on_start();
    });
    ArduinoOTA.onEnd([]()
    {
        updater_ota_instance->handle_on_end();
    });
    ArduinoOTA.onProgress([](unsigned int p, unsigned int t)
    {
        updater_ota_instance->handle_on_progress(p,t);
    });
    ArduinoOTA.onError([](ota_error_t e)
    {
        updater_ota_instance->handle_on_error(e);
    });
};

void updater_ota::set_hostname(const String& name, bool append_chip_id)
{
    if(name!="")
    {
        String hostname=name;
        if(append_chip_id)
        {
            hostname+=" esp8266-"+String(ESP.getChipId(), HEX);
        }
        ArduinoOTA.setHostname(hostname.c_str());
    }
}

void updater_ota::set_password(const String& password)
{
    ArduinoOTA.setPassword(password.c_str());
}

void updater_ota::begin(bool use_serial, bool run_default_handlers)
{
    setup();
    this->use_serial=use_serial;
    this->run_default_handlers=run_default_handlers;
    ArduinoOTA.begin();
}

void updater_ota::update()
{
    ArduinoOTA.handle();
}

void updater_ota::handle_on_start()
{
    if(!on_start || run_default_handlers)
    {
        if(use_serial)
        {
            Serial.println("[OTA] Start");
        }
    }
    if(on_start)
        on_start();
}

void updater_ota::handle_on_end()
{
    if(!on_end || run_default_handlers)
    {
        if(use_serial)
        {
            Serial.println("\n[OTA] End");
        }
    }
    if(on_end)
        on_end();
}

void updater_ota::handle_on_error(ota_error_t error)
{
    if(!on_error || run_default_handlers)
    {
        if(use_serial)
        {
            Serial.printf("[OTA] Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        }
    }
    if(on_error)
        on_error(error);
}

void updater_ota::handle_on_progress(unsigned int current, unsigned int total)
{
    unsigned int progress=current/(total/100);
    if(!on_progress || run_default_handlers)
    {
        if(use_serial)
        {
            Serial.printf("[OTA] Progress: %u%%\r", progress);
        }
    }
    if(on_progress)
        on_progress(progress);
}
