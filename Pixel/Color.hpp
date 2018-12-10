#pragma once

namespace Px
{
class Color
{
public:
	constexpr Color()
		:
		value{ 0 }
	{
	}

	explicit constexpr Color(const unsigned int value)
		:
		value{ value }
	{
	}

	constexpr Color(const unsigned char red, const unsigned char green, const unsigned char blue)
		:
		Color{ red, green, blue, 255u }
	{
	}

	constexpr Color(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
		:
		value{ static_cast<unsigned int>((alpha << 24u) | (red << 16u) | (green << 8u) | blue) }
	{
	}

	constexpr Color(const Color color, const unsigned char alpha)
		:
		value{ (alpha << 24u) | color.value }
	{
	}

public:
	constexpr unsigned char GetAlpha() const
	{
		return (value >> 24u);
	}

	constexpr unsigned char GetRed() const
	{
		return (value >> 16u) & 0xFFu;
	}

	constexpr unsigned char GetGreen() const
	{
		return (value >> 8u) & 0xFFu;
	}

	constexpr unsigned char GetBlue() const
	{
		return value & 0xFFu;
	}

	void SetAlpha(const unsigned char value)
	{
		this->value = (this->value & 0xFFFFFFu) | (value << 24u);
	}

	void SetRed(const unsigned char value)
	{
		this->value = (this->value & 0xFF00FFFFu) | (value << 16u);
	}

	void SetGreen(const unsigned char value)
	{
		this->value = (this->value & 0xFFFF00FFu) | (value << 8u);
	}

	void SetBlue(const unsigned char value)
	{
		this->value = (this->value & 0xFFFF00u) | value;
	}

private:
	unsigned int value;
};
}