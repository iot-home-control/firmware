#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include <map>
#include <vector>


class ticker_component;

class config_loader
{
public:
    typedef std::function<ticker_component*(const JsonObject&/*obj*/, const int/*vnode_id*/)> FactoryFunction;
    void begin(std::vector<ticker_component*> &components, bool skip_components=false);
    String dump();
    // Returns true if reboot is required
    bool check(const String &device_id, const String &config_server="") const;
    bool install_factory(const char* type, int type_id, FactoryFunction creator);
private:
    struct factory
    {
        const char* type;
        const int type_id;
        FactoryFunction creator;
    };
    std::map<int, int> type_vnode_ids;
    std::vector<factory> factories;
    int current_config_version = -1;
    bool have_config_json = false;
};
