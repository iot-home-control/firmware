#include "led_strip.h"
#include "util.h"

led_strip::led_strip(size_t length): animator(length, NEO_MILLISECONDS), strip_length(length),
    animation_playing(false), repeat_animation(false), current_animation_setup(nullptr), leds(length,3)
{
}

void led_strip::begin()
{
    leds.Begin();
    leds.ClearTo(RgbColor(0,0,0));
    leds.Show();
    _is_on=false;
}

void led_strip::update()
{
    if(animator.IsAnimating())
    {
        animator.UpdateAnimations();
        leds.Show();
    }
    else
    {
        // Animation was playing but it is done now.
        if(animation_playing)
        {
            if(repeat_animation)
            {
                if(current_animation_setup)
                {
                    current_animation_setup();
                }
            }
            else
            {
                current_animation_setup=nullptr;
            }
        }
        else
        {
            if(current_animation_setup)
            {
                current_animation_setup();
                animation_playing=true;
            }
        }
    }
}

bool led_strip::is_on() const
{
    return _is_on;
}

void led_strip::fade_to_color(const HslColor &color, size_t fade_duration_ms)
{
    for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
    {
        HslColor color_start=leds.GetPixelColor(pixel_index);
        AnimUpdateCallback cb_update=[=](AnimationParam param) mutable
        {
            // progress will start at 0.0 and end at 1.0
            auto set_color = //gamma.Correct(
                        RgbColor(
                            HslColor::LinearBlend<NeoHueBlendShortestDistance>(
                                color_start,
                                color,
                                param.progress)
                          //  )
                        );
            leds.SetPixelColor(pixel_index, set_color);
        };
        animator.StartAnimation(pixel_index,fade_duration_ms,cb_update);
    }
}

void led_strip::set_color_to(const HslColor &color)
{
    leds.ClearTo(color);
    leds.Show();
}

void led_strip::turn_on(const HslColor &color, size_t fade_duration_ms)
{
    _is_on=true;
    if(fade_duration_ms!=0)
    {
        HslColor off_color=color;
        off_color.L=0;
        leds.ClearTo(off_color);
        leds.Show();
        fade_to_color(color, fade_duration_ms);
    }
    else
    {
        leds.ClearTo(color);
        leds.Show();
    }
}

void led_strip::turn_off(size_t fade_duration_ms)
{
    _is_on=false;
    for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
    {
        HslColor color_start=leds.GetPixelColor(pixel_index);
        HslColor color_stop=color_start;
        color_stop.L=0;
        AnimUpdateCallback cb_update=[=](AnimationParam param)
        {
            // progress will start at 0.0 and end at 1.0
            auto set_color = //gamma.Correct(
                        RgbColor(
                            HslColor::LinearBlend<NeoHueBlendShortestDistance>(
                                color_start,
                                color_stop,
                                param.progress)
                            //)
                        );
            leds.SetPixelColor(pixel_index, set_color);
        };
        animator.StartAnimation(pixel_index,fade_duration_ms,cb_update);
    }
}

void led_strip::play_animation_rainbow(uint16_t duration_ms, bool repeat)
{
    current_animation_setup=[this,duration_ms]
    {
        for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
        {
            float s=1.0, l=0.5;
            float h=pixel_index/(float)(strip_length+1);
            HslColor color_start(h,s,l);
            bool wrapped=false;
            AnimUpdateCallback cb_update=[=](AnimationParam param) mutable
            {
                // progress will start at 0.0 and end at 1.0
                float h_new=color_start.H+param.progress;
                if(h_new>1.0 || wrapped)
                {
                    wrapped=true;
                    h_new-=1.0;
                }
                HslColor color_new(h_new, color_start.S, color_start.L);
                leds.SetPixelColor(pixel_index, color_new);
            };
            animator.StartAnimation(pixel_index,duration_ms,cb_update);
        }
    };
    animation_playing=false;
    repeat_animation=repeat;
}

RgbColor sunrise_colors[]=
{
    {0, 0, 0},
    {8, 0, 0},
    {17, 0, 0},
    {26, 0, 0},
    {35, 0, 0},
    {43, 0, 0},
    {52, 0, 0},
    {61, 0, 0},
    {70, 0, 0},
    {79, 0, 0},
    {87, 0, 0},
    {96, 0, 0},
    {105, 0, 0},
    {114, 0, 0},
    {123, 0, 0},
    {131, 0, 0},
    {140, 0, 0},
    {149, 0, 0},
    {158, 0, 0},
    {167, 0, 0},
    {175, 0, 0},
    {184, 0, 0},
    {193, 0, 0},
    {202, 0, 0},
    {211, 0, 0},
    {219, 0, 0},
    {228, 0, 0},
    {237, 0, 0},
    {246, 0, 0},
    {255, 0, 0},
    {255, 0, 0},
    {255, 2, 0},
    {255, 4, 0},
    {255, 6, 0},
    {255, 8, 0},
    {255, 10, 0},
    {255, 13, 0},
    {255, 15, 0},
    {255, 17, 0},
    {255, 19, 0},
    {255, 21, 0},
    {255, 23, 0},
    {255, 26, 0},
    {255, 28, 0},
    {255, 30, 0},
    {255, 32, 0},
    {255, 34, 0},
    {255, 36, 0},
    {255, 39, 0},
    {255, 41, 0},
    {255, 43, 0},
    {255, 45, 0},
    {255, 47, 0},
    {255, 49, 0},
    {255, 52, 0},
    {255, 54, 0},
    {255, 56, 0},
    {255, 58, 0},
    {255, 60, 0},
    {255, 62, 0},
    {255, 65, 0},
    {255, 67, 0},
    {255, 69, 0},
    {255, 71, 0},
    {255, 73, 0},
    {255, 75, 0},
    {255, 78, 0},
    {255, 80, 0},
    {255, 82, 0},
    {255, 84, 0},
    {255, 86, 0},
    {255, 88, 0},
    {255, 91, 0},
    {255, 93, 0},
    {255, 95, 0},
    {255, 97, 0},
    {255, 99, 0},
    {255, 101, 0},
    {255, 104, 0},
    {255, 106, 0},
    {255, 108, 0},
    {255, 110, 0},
    {255, 112, 0},
    {255, 114, 0},
    {255, 117, 0},
    {255, 119, 0},
    {255, 121, 0},
    {255, 123, 0},
    {255, 125, 0},
    {255, 128, 0},
    {255, 128, 0},
    {255, 130, 0},
    {255, 132, 0},
    {255, 135, 0},
    {255, 137, 0},
    {255, 140, 0},
    {255, 142, 0},
    {255, 145, 0},
    {255, 147, 0},
    {255, 150, 0},
    {254, 150, 1},
    {253, 150, 2},
    {253, 150, 3},
    {252, 150, 5},
    {252, 150, 6},
    {251, 150, 7},
    {250, 150, 9},
    {250, 150, 10},
    {249, 150, 11},
    {249, 150, 12},
    {248, 150, 14},
    {247, 150, 15},
    {247, 150, 16},
    {246, 150, 18},
    {246, 150, 19},
    {245, 150, 20},
    {245, 150, 22},
    {244, 150, 23},
    {243, 150, 24},
    {243, 150, 25},
    {242, 150, 27},
    {242, 150, 28},
    {241, 150, 29},
    {240, 150, 31},
    {240, 150, 32},
    {239, 150, 33},
    {239, 150, 34},
    {238, 150, 36},
    {237, 150, 37},
    {237, 150, 38},
    {236, 150, 40},
    {236, 150, 41},
    {235, 150, 42},
    {235, 150, 44},
    {234, 150, 45},
    {233, 150, 46},
    {233, 150, 47},
    {232, 150, 49},
    {232, 150, 50},
    {231, 150, 51},
    {230, 150, 53},
    {230, 150, 54},
    {229, 150, 55},
    {229, 150, 57},
    {228, 150, 58},
    {228, 150, 59},
    {227, 150, 60},
    {226, 150, 62},
    {226, 150, 63},
    {225, 150, 64},
    {225, 150, 66},
    {224, 150, 67},
    {223, 150, 68},
    {223, 150, 69},
    {222, 150, 71},
    {222, 150, 72},
    {221, 150, 73},
    {220, 150, 75},
    {220, 150, 76},
    {219, 150, 77},
    {219, 150, 79},
    {218, 150, 80},
    {218, 150, 81},
    {217, 150, 82},
    {216, 150, 84},
    {216, 150, 85},
    {215, 150, 86},
    {215, 150, 88},
    {214, 150, 89},
    {213, 150, 90},
    {213, 150, 92},
    {212, 150, 93},
    {212, 150, 94},
    {211, 150, 95},
    {211, 150, 97},
    {210, 150, 98},
    {209, 150, 99},
    {209, 150, 101},
    {208, 150, 102},
    {208, 150, 103},
    {207, 150, 104},
    {206, 150, 106},
    {206, 150, 107},
    {205, 150, 108},
    {205, 150, 110},
    {204, 150, 111},
    {203, 150, 112},
    {203, 150, 114},
    {202, 150, 115},
    {202, 150, 116},
    {201, 150, 117},
    {201, 150, 119},
    {200, 150, 120},
    {199, 150, 121},
    {199, 150, 123},
    {198, 150, 124},
    {198, 150, 125},
    {197, 150, 127},
    {196, 150, 128},
    {196, 150, 129},
    {195, 150, 130},
    {195, 150, 132},
    {194, 150, 133},
    {193, 150, 134},
    {193, 150, 136},
    {192, 150, 137},
    {192, 150, 138},
    {191, 150, 139},
    {191, 150, 141},
    {190, 150, 142},
    {189, 150, 143},
    {189, 150, 145},
    {188, 150, 146},
    {188, 150, 147},
    {187, 150, 149},
    {186, 150, 150},
    {186, 150, 151},
    {185, 150, 152},
    {185, 150, 154},
    {184, 150, 155},
    {184, 150, 156},
    {183, 150, 158},
    {182, 150, 159},
    {182, 150, 160},
    {181, 150, 162},
    {181, 150, 163},
    {180, 150, 164},
    {179, 150, 165},
    {179, 150, 167},
    {178, 150, 168},
    {178, 150, 169},
    {177, 150, 171},
    {176, 150, 172},
    {176, 150, 173},
    {175, 150, 174},
    {175, 150, 176},
    {174, 150, 177},
    {174, 150, 178},
    {173, 150, 180},
    {172, 150, 181},
    {172, 150, 182},
    {171, 150, 184},
    {171, 150, 185},
    {170, 150, 186},
    {169, 150, 187},
    {169, 150, 189},
    {168, 150, 190},
    {168, 150, 191},
    {167, 150, 193},
    {167, 150, 194},
    {166, 150, 195},
    {165, 150, 197},
    {165, 150, 198},
    {164, 150, 199},
    {164, 150, 200},
    {163, 150, 202},
    {162, 150, 203},
    {162, 150, 204},
    {161, 150, 206},
    {161, 150, 207},
    {160, 150, 208},
    {159, 150, 209},
    {159, 150, 211},
    {158, 150, 212},
    {158, 150, 213},
    {157, 150, 215},
    {157, 150, 216},
    {156, 150, 217},
    {155, 150, 219},
    {155, 150, 220},
    {154, 150, 221},
    {154, 150, 222},
    {153, 150, 224},
    {152, 150, 225},
    {152, 150, 226},
    {151, 150, 228},
    {151, 150, 229},
    {150, 150, 230},
    {150, 150, 232},
    {149, 149, 233},
    {148, 148, 234},
    {147, 147, 235},
    {146, 146, 236},
    {145, 145, 237},
    {144, 144, 238},
    {142, 142, 240},
    {141, 141, 241},
    {140, 140, 242},
    {139, 139, 243},
    {138, 138, 244},
    {137, 137, 245},
    {136, 136, 246},
    {135, 135, 247},
    {134, 134, 248},
    {133, 133, 249},
    {132, 132, 250},
    {131, 131, 251},
    {130, 130, 252},
    {129, 129, 253},
    {128, 128, 255},
};


void led_strip::play_animation_sunrise(uint16_t duration_ms)
{
    current_animation_setup=[this,duration_ms]
    {
        uint16_t center_index=(strip_length-1)/2;
        float max_offset=0.33;
        for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
        {
            float progress_offset=abs(pixel_index-center_index)/(float)(center_index)*max_offset;
            Serial.print("pixel ");
            Serial.print(pixel_index);
            Serial.print(" offset ");
            Serial.println(progress_offset);
            AnimUpdateCallback cb_update=[=](AnimationParam param) mutable
            {
                float progress = clamp(0.0f, param.progress-progress_offset, 1.0f);

                RgbColor pixel_color=sunrise_colors[(int)(progress*(ARRAY_COUNT(sunrise_colors)-1))];
                leds.SetPixelColor(pixel_index, pixel_color);
            };
            animator.StartAnimation(pixel_index,duration_ms,cb_update);
        }
    };
    animation_playing=false;
    repeat_animation=false;
}

void led_strip::play_animation_warp_core(HslColor color, uint16_t duration_ms, bool repeat)
{
    current_animation_setup=[this,duration_ms,color]
    {
        float bump_percent=0.33;
        float bump_percent_2=bump_percent/2.0f;
        float bump_start_percent=0.5-bump_percent_2;
        float bump_stop_percent=0.5+bump_percent_2;
        uint16_t center_index=(strip_length-1)/2;
        for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
        {
            float p=1-abs(pixel_index-center_index)/(float)(center_index);
            AnimUpdateCallback cb_update=[=](AnimationParam param) mutable
            {
                float progress=param.progress;
                if(progress<=bump_start_percent)
                {
                    bool on=(progress/bump_start_percent)>p;
                    if(on)
                    {
                        leds.SetPixelColor(pixel_index, color);
                    }
                    else
                    {
                        leds.SetPixelColor(pixel_index, RgbColor(0,0,0));
                    }
                }
                else if(progress>bump_start_percent && progress <=bump_stop_percent)
                {
                    float bump=(progress-bump_start_percent)/bump_percent;
                    HslColor bump_color_low = color;
                    HslColor bump_color_high = bump_color_low;
                    bump_color_high.L*=1.5;
                    HslColor bump_color;

                    if(bump>=0.5)
                    {
                        float local_percent=bump/0.5;
                        local_percent=NeoEase::QuadraticIn(local_percent);
                        bump_color=bump_color_low;
                        bump_color.L*=(local_percent*1.5);
                    }
                    else
                    {
                        float local_percent=(bump-0.5)/0.5;
                        local_percent=NeoEase::QuadraticOut(local_percent);
                        bump_color=bump_color_high;
                        bump_color.L/=(local_percent*1.5);
                    }
                    leds.SetPixelColor(pixel_index, bump_color);
                }
                else //bump_stop_percent
                {
                    bool off=((progress-bump_stop_percent)/bump_start_percent)>p;
                    if(off)
                    {
                        leds.SetPixelColor(pixel_index, RgbColor(0,0,0));
                    }
                    else
                    {
                        leds.SetPixelColor(pixel_index, color);
                    }
                }
            };
            animator.StartAnimation(pixel_index,duration_ms,cb_update);
        }
    };
    animation_playing=false;
    repeat_animation=repeat;
}

void led_strip::stop_animation()
{
    for(uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
        animator.StopAnimation(pixel_index);
    repeat_animation=false;
}

bool led_strip::is_playing_animation()
{
    return animation_playing;
}
