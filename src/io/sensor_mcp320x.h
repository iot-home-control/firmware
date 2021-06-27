#pragma once

#include "../components/ticker_component.h"
#include <Mcp320x.h>
#include <functional>

template <int channels> struct mcp320x_channel_map {};

template <> struct mcp320x_channel_map<1> {
    typedef MCP320xTypes::MCP3201::Channel channel;
    static constexpr int mode_shift = 0;
};
template <> struct mcp320x_channel_map<2> {
    using channel = MCP320xTypes::MCP3202::Channel;
    static constexpr int mode_shift = 1;
};
template <> struct mcp320x_channel_map<4> {
    using channel = MCP320xTypes::MCP3204::Channel;
    static constexpr int mode_shift = 3;
};
template <> struct mcp320x_channel_map<8> {
    using channel = MCP320xTypes::MCP3208::Channel;
    static constexpr int mode_shift = 3;
};


template <int n>
class sensor_mcp320x: public ticker_component
{
private:
    template <typename channels>
    class wrapper
    {
    public:
        MCP320x<channels> mcp;
        wrapper(unsigned char cs, unsigned int vref): mcp(vref, cs){}
    };

    using map = mcp320x_channel_map<n>;
    using channel_type = typename map::channel;
    wrapper<channel_type> *sensor;
    unsigned char active_channels;

    unsigned long prev_millis, update_every_ms;

    uint16_t last_value[n];
    bool always_notify;
public:
    typedef std::function<void(const char, const uint16_t, const uint16_t)> callback;

    sensor_mcp320x<n>(): sensor(nullptr), prev_millis(0) { global_spi_init(); }
    void begin(unsigned char pin, uint16_t vref, unsigned char active_channels, unsigned long update_every_ms, const bool always_notify=false);
    void update() override;
    uint16_t get_value(const unsigned char idx);
    unsigned char get_active_channels() const;

    callback on_value_changed;

    static void global_spi_init();


};



