#include "wifi_connector.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>

wifi_connector::wifi_connector(): connecting(false), connected(false)
{
    WiFi.mode(WIFI_STA);
}

void wifi_connector::begin(const String& ssid, const String& password)
{
    this->ssid=ssid;
    this->password=password;
    connecting=true;
    WiFi.hostname("esp8266-"+String(ESP.getChipId(), HEX));
    WiFi.begin(this->ssid.c_str(), this->password.c_str());
}

void wifi_connector::update()
{
    auto status=WiFi.status();
    if(status==WL_CONNECTED)
    {
        if(connecting)
        {
            connecting=false;
            connected=true;
            if(on_connected)
                on_connected();
        }
    }
    else
    {
        if(connected)
        {
            // We were connected but aren't anymore
            connected=false;
            if(on_disconnected)
                on_disconnected();
        }
        if(!connecting)
        {
            // Start re-connecting
            connecting=true;
            WiFi.begin(this->ssid.c_str(), this->password.c_str());
        }
    }
}

bool wifi_connector::is_connected() const
{
    return connected;
}

bool wifi_connector::host_by_name(String hostname, IPAddress &ip)
{
    return WiFi.hostByName(hostname.c_str(), ip);
}
