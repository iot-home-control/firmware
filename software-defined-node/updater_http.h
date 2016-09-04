#ifndef UPDATER_HTTP_H
#define UPDATER_HTTP_H

#include <Arduino.h>
#include <functional>

class updater_http
{
private:
    String url, version;
public:
    typedef std::function<void()> callback;
    typedef std::function<void(const String& s)> callback_string;
    
    updater_http();
    void begin(const String& url, const String& version);
    void check();
    
    callback before_update;
    callback on_update_ok;
    callback_string on_update_error;
    callback on_update_up_to_date;
};

#endif
