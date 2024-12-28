#pragma once

#include <cmath>

template<typename T = float>
constexpr T PI{ T{ 3.141592653589793238462643383279502884 } };


template <typename T>
constexpr auto sq(const T& x)
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta)
{
	const T modded = fmod(theta, static_cast<T>(2.0) * PI<T>);
	return (modded > PI<T>) ?
		(modded - static_cast<T>(2.0) * PI<T>) :
		modded;
}

template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha)
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad(T deg)
{
	return deg * PI<T> / static_cast<T>(180.0);
}
