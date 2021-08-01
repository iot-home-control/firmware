// SPDX-License-Identifier: GPL-3.0-or-later
/*
    Copyright (C) 2021  The Home Control Authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "mqtt_handler.h"
#include <algorithm>

#if 0
#define spl Serial.println
#define sp Serial.println
#else
#define spl(...)
#define sp(...)
#endif

int get_retry_delay(int retry_count, int retry_interval)
{
        if(retry_count < 5)
        {
            return retry_interval;
        } 
        else
        {
            return 2*retry_count*retry_interval;
        }
}

void mqtt_handler::mqtt_callback(char* topic, uint8_t* data, unsigned int length)
{
    String the_topic(topic);
    
    auto result=std::find_if(handlers.begin(),handlers.end(),[&](const handler_pair& hp){return hp.topic==the_topic;});
    if(result!=handlers.end())
    {
        (*result).handler(topic,data,length);
    }
}

void mqtt_handler::check_subscriptions()
{
    for(auto& hp:handlers)
    if(!hp.subscribed)
    {
        hp.subscribed=client.subscribe(hp.topic.c_str());
    }
    last_subscription_check=millis();
}

mqtt_handler::mqtt_handler(): client(wifi_client), connecting(false), connected(false), connection_retry_interval(1000), connection_retry_count(0), subscription_check_interval(2500)
{
}

void mqtt_handler::begin(IPAddress ip, const String& client_id, const String& user, const String& pass)
{
    client.setServer(ip, 1883);
    client.setCallback([this](char* t, uint8_t* d, unsigned int l){mqtt_callback(t,d,l);});
    mqtt_id=client_id;
    mqtt_user=user;
    mqtt_pass=pass;
    can_connect=true;
}

void mqtt_handler::update()
{
    unsigned long now=millis();
    if(!client.connected())
    {
        spl("Not connected");
        if(!can_connect)
            return;
        
        if(connected)
        {
            connected=false;
            
            for(auto& hp:handlers)
                hp.subscribed=false;
            
            if(on_disconnected)
                on_disconnected();
            spl("Marked as disconnected");
        }
        if(!connecting)
        {
            spl("Starting to connect");
            connecting=true;
            connecting_start_time=now;
            client.connect(mqtt_id.c_str(), mqtt_user.c_str(), mqtt_pass.c_str());
            spl("Connection request sent");
        }
        else if(now-connecting_start_time>(unsigned)get_retry_delay(connection_retry_count, connection_retry_interval))
        {
            connection_retry_count++;
            // retry
            spl("Retry");
            connecting=false;
        }
    }
    else
    {
        spl("Connected");
        if(connecting)
        {
            spl("Done connecting");
            connecting=false;
            connected=true;
            
            check_subscriptions();

            if(on_connected)
                on_connected();

            connection_retry_count = 0;
        }
        if(now-last_subscription_check>subscription_check_interval)
        {
            spl("Checking subscriptions");
            check_subscriptions();
        }
        client.loop();
    }
}
void mqtt_handler::handle_topic(const String& name, topic_handler handler)
{
    sp("Adding topic handler for ");
    spl(name);
    handlers.push_back({name,handler,false});
    if(client.connected())
    {
        client.subscribe(name.c_str());
        handlers.back().subscribed=true;
    }
}

void mqtt_handler::publish(const String& topic, const String& msg)
{
    if(!connected)
        return;
    
    client.publish(topic.c_str(),msg.c_str());
}
