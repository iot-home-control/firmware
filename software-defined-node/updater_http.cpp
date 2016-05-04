#include "updater_http.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

updater_http::updater_http()
{
    ESPhttpUpdate.rebootOnUpdate(false);
}

void updater_http::begin(const String& url, const String& version)
{
    this->url=url;
    this->version=version;
}

void updater_http::check()
{
    if(before_update)
        before_update();
    
    HTTPUpdateResult result=ESPhttpUpdate.update(url,version);
    if(result==HTTP_UPDATE_FAILED)
    {
        Serial.println();
        Serial.println(ESPhttpUpdate.getLastErrorString());
        Serial.println();
        if(on_update_error)
            on_update_error();
    }
    else if(result==HTTP_UPDATE_NO_UPDATES)
    {
        if(on_update_up_to_date)
            on_update_up_to_date();
    }
    else
    {
        if(on_update_ok)
            on_update_ok();
        ESP.restart();
    }
}

