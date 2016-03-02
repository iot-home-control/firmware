#pragma once
#include <HslColor.h>

struct wifi_info
{
	const char* ssid;
	const char* password;
};

enum rotary_mode
{
  mode_h,
  mode_s,
  mode_l,
};

wifi_info wifi_config[]={
	{0,0},
	{"ThatsNoWiFi","J2UYpGaNUN2gh7nb"},
	{"Freifunk",""},
	{"1084059","2415872658287010"}
};

HslColor presets[]={
  {0.0833, 1.0, 0.580}, //Candle (1900K)
  {0.0805, 1.0, 0.780}, //40W Tungsten (2600K)
  {0.1388, 1.0, 0.833}, //100W Tungsten (2850K)
  {0.0916, 1.0, 0.939}, //Halogen (3200K)
  {0.0916, 1.0, 0.978}, //Carbon Arc (5200K)
  {0.1666, 1.0, 0.992}, //High Noon Sun (5400K)
  {0.0000, 1.0, 1.000}, //Direct Sunlight (6000K)
  {0.4944, 1.0, 0.894}, //Overcast Sky (7000K)
  {0.5861, 1.0, 0.625}, //Clear Blue Sky (20000K)
};