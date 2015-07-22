function lerp(a,b,t)
	return a+(b-a)*t
end

function clamp(low,value,high)
	return math.min(high,math.max(low,value))
end

function trunc(value)
	return math.floor(value+0.5)
end

function color(r,g,b)
	return {
		r=r,
		g=g,
		b=b
	}
end
