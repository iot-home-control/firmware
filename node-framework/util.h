#pragma once

#define ARRAY_COUNT(arr) (sizeof((arr))/sizeof((arr)[0]))

template<typename T>
T clamp(T min, T value, T max)
{
    if(value<min)
        return min;
    if(value>max)
        return max;
    return value;
}
