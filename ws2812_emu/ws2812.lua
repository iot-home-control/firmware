ws2812 =
{
_state="",
_len=2.5*30,
writergb=function(pin, str) -- RGB, modifies buffer
end,
write=function(pin, str) -- GBR
	ws2812._state=str
end,
draw=function()
	
	for i=0,ws2812._len do
		local idx=i*3		
		if idx < ws2812._state:len() then
			local r=ws2812._state:byte(idx+3)
			local g=ws2812._state:byte(idx+2)
			local b=ws2812._state:byte(idx+1)
			love.graphics.setColor(r,g,b,255)
			love.graphics.rectangle('fill',i*20,0,20,20)
		end		
	end
end
}