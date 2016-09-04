#include "mqtt_handler.h"
#include <algorithm>

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

mqtt_handler::mqtt_handler(): client(wifi_client), connecting(false), connected(false), connection_retry_interval(1000), subscription_check_interval(2500)
{
}

void mqtt_handler::begin(const String& host, const String& client_id, const String& user, const String& pass)
{
    IPAddress ip;
    ip.fromString(host);
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
        if(!can_connect)
            return;
        
        if(connected)
        {
            connected=false;
            
            for(auto& hp:handlers)
                hp.subscribed=false;
            
            if(on_disconnected)
                on_disconnected();
        }
        if(!connecting)
        {
            connecting=true;
            connecting_start_time=now;
            client.connect(mqtt_id.c_str(), mqtt_user.c_str(), mqtt_pass.c_str());
        }
        else if(now-connecting_start_time>connection_retry_interval)
        {
            // retry
            connecting=false;
        }
    }
    else
    {
        if(connecting)
        {
            connecting=false;
            connected=true;
            
            check_subscriptions();

            if(on_connected)
                on_connected();
        }
        if(now-last_subscription_check>subscription_check_interval)
        {
            check_subscriptions();
        }
        client.loop();
    }
}
void mqtt_handler::handle_topic(const String& name, topic_handler handler)
{
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
