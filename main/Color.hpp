#pragma once


#include <stdint.h>


class Color
{
public:
	using RawType = uint16_t;

	constexpr Color();
	constexpr Color(uint8_t r, uint8_t g, uint8_t b);

	constexpr RawType raw() const;

	constexpr bool operator==(Color rhs) const;
	constexpr bool operator!=(Color rhs) const;

	constexpr static Color fromBrightness(uint8_t brightness);

private:
	RawType _value;
};


constexpr Color::Color() :
	_value(0)
{
	
}


constexpr Color::Color(uint8_t r, uint8_t g, uint8_t b) :
	_value(
		(r & 0b11111000) |		// xxxxxxxxRRRRRxxx
		((b >> 3) << 8) |		// xxxBBBBBxxxxxxxx
		(g >> 5) |				// xxxxxxxxxxxxxGGG
		((g & 0b11100) << 11))	// GGGxxxxxxxxxxxxx
{

}


constexpr Color::RawType Color::raw() const
{
	return _value;
}


constexpr bool Color::operator==(Color rhs) const
{
	return _value == rhs._value;
}


constexpr bool Color::operator!=(Color rhs) const
{
	return _value != rhs._value;
}


constexpr Color Color::fromBrightness(uint8_t brightness)
{
	return Color(brightness, brightness, brightness);
}