led_pin=4
timer_interval=0.05 -- s -> 5ms
dtsum=0

require('helpers')

function init_wifi()
    print("Initializing WiFi")
    wifi.sta.connect()
    tmr.alarm(0, 1000, 1, function()
        if wifi.sta.getip()==nil then
            print("Connecting to AP...")
        else
            print("Connected to AP. IP:",wifi.sta.getip())
            tmr.stop(0)
            --init_mqtt()
        end
    end)
end

function init_strip(pin,strip_len)
    return {
        pin=pin,
        cur=make_single_color_strip_state(strip_len,color(0,0,0)),
        len=strip_len
    }
end

function make_single_color_strip_state(strip_len, initial_color)
    local res={
        red={},
        green={},
        blue={}
    }
    for i=1, strip_len do
        res.red[i]=initial_color.r
        res.green[i]=initial_color.g
        res.blue[i]=initial_color.b
    end
    return res
end

function make_random_strip_state(strip_len)
    local res=make_single_color_strip_state(strip_len,color())
    for i=1,strip_len do
        res.red[i]=math.random(0,255)
        res.green[i]=math.random(0,255)
        res.blue[i]=math.random(0,255)
    end
    return res
end

function make_fade_strip_animation(new_color, speed)
    return {
        t=0,
        speed=speed,
        start=strip.cur,
        stop=make_single_color_strip_state(strip.len,new_color)
    }
end

function make_fade_strip_to_random_colors_animaion(speed)
    local res=make_fade_strip_animation(color(0,0,0),speed)
    res.stop=make_random_strip_state(strip.len)
    return res
end

function set_strip(state)
    local s={}
    strip.cur=state
    for i=strip.len,1,-1 do
        strip.cur.red[i]=clamp(0,trunc(strip.cur.red[i]),255)
        strip.cur.green[i]=clamp(0,trunc(strip.cur.green[i]),255)
        strip.cur.blue[i]=clamp(0,trunc(strip.cur.blue[i]),255)
        s[#s+1]=tostring(strip.cur.blue[i])
	s[#s+1]=tostring(strip.cur.red[i])
	s[#s+1]=tostring(strip.cur.green[i])
    end    
    ws2812.write(strip.pin,table.concat(s,''))
end

strip=init_strip(led_pin,63)
fade_config=nil

tmr.alarm(1,5,1,function()
    local dt=timer_interval
    if f_timer and f_timer ~= nil then
        f_timer(dt)
    end
end)

function fade_strip()
    f_timer=function(dt) -- tmr.alarm
        fade_config.t=fade_config.t+(dt/fade_config.speed)
        if fade_config.t>=1 then
            f_timer=nil -- tmr.stop
            fade_config=nil
            return
        end
        local new_state=make_single_color_strip_state(strip.len,color(0,0,0))
        for i=1,strip.len do
            new_state.red[i]=lerp(fade_config.start.red[i],fade_config.stop.red[i],fade_config.t)
            new_state.green[i]=lerp(fade_config.start.green[i],fade_config.stop.green[i],fade_config.t)
            new_state.blue[i]=lerp(fade_config.start.blue[i],fade_config.stop.blue[i],fade_config.t)
        end
        set_strip(new_state)
    end
end

function animate_strip(pattern)
    anim_config=pattern
    f_timer=function(dt)
        if anim_config and anim_config ~= nil then
            set_strip(anim_config:update(dt))
        end
    end
end

function fade_strip_to_color(r,g,b,speed)
    fade_config=make_fade_strip_animation(color(r,g,b),speed)
    fade_strip()
end

function set_strip_to_color(r,g,b)
    set_strip(make_single_color_strip_state(strip.len,color(r,g,b)))
end

function fade_strip_to_random_colors(speed)
    fade_config=make_fade_strip_to_random_colors_animaion(0.5)
    fade_strip()
end

function make_animation_pattern()
    return {
        stage=0,
        stage_t=0,
        update=nil
    }
end

function make_pingpong_pattern(color_on,color_off,speed,trail)
    local pattern=make_animation_pattern()
    pattern.stage=0 -- 0 -> left to right, 1 -> right to left
    pattern.current_pos=1
    pattern.current_led_t=0
    pattern.color_on=color_on
    pattern.color_off=color_off
    pattern.speed=speed -- time per led on in seconds
    pattern.trail_factor={0.75,0.5,0.25}
    local trail=trail or 0
    pattern.trail=math.min(trail or 0,3) -- #leds to the left and side
    pattern.update=function(self,dt)
        self.current_led_t=self.current_led_t+dt
        if self.current_led_t>=self.speed then
            if self.stage==0 then
                self.current_pos=self.current_pos+1
                if self.current_pos>=strip.len then
                    self.stage=1
                end
            elseif self.stage==1 then
                self.current_pos=self.current_pos-1
                if self.current_pos<=1 then
                    self.stage=0
                end
            end
            self.current_led_t = self.current_led_t-self.speed
        end
        local res=make_single_color_strip_state(strip.len,color_off)
        res.red[self.current_pos]=self.color_on.r
        res.green[self.current_pos]=self.color_on.g
        res.blue[self.current_pos]=self.color_on.b
        if self.trail~=0 then
            for i=1,self.trail do
                local factor=self.trail_factor[i]               
                if self.stage==0 and self.current_pos-i>=1 then
                    local np=self.current_pos-i
                    res.red[np]=trunc(self.color_on.r*factor)
                    res.green[np]=trunc(self.color_on.g*factor)
                    res.blue[np]=trunc(self.color_on.b*factor)
                elseif self.stage==1 and self.current_pos+i<strip.len then
                    local np=self.current_pos+i
                    res.red[np]=trunc(self.color_on.r*factor)
                    res.green[np]=trunc(self.color_on.g*factor)
                    res.blue[np]=trunc(self.color_on.b*factor)
                end
            end
        end
        return res
    end
    return pattern
end

function animate_strip_pingpong(color_on,color_off,speed)
    animate_strip(make_pingpong_pattern(color_on,color_off,speed,3))    
end
