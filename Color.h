//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	Color()
	{
		*((int *)this) = 0;
	}
	Color(int _r, int _g, int _b)
	{
		SetColor(_r, _g, _b, 0);
	}
	Color(int _r, int _g, int _b, int _a)
	{
		SetColor(_r, _g, _b, _a);
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int *)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int *)this);
	}

	static Color White() { return Color(255, 255, 255, 255); }
	static Color Black() { return Color(0, 0, 0, 255); }
	static Color Red() { return Color(244, 66, 66, 255); }
	static Color Orange() { return Color(255, 165, 0, 255); }
	static Color Yellow() { return Color(255, 255, 0, 255); }
	static Color Green() { return Color(40, 255, 47, 255); }
	static Color Blue() { return Color(0, 255, 157, 255); }
	static Color Indigo() { return Color(75, 0, 130, 255); }
	static Color Violet() { return Color(128, 66, 244, 255); }
	static Color Cyan() { return Color(57, 237, 236, 255); }
	static Color Grey() { return Color(177, 191, 202, 255); }
	static Color CyanD() { return Color(51, 102, 101, 255); }

	inline int r() const { return _color[0]; }
	inline int g() const { return _color[1]; }
	inline int b() const { return _color[2]; }
	inline int a() const { return _color[3]; }

	unsigned char &operator[](int index)
	{
		return _color[index];
	}

	const unsigned char &operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const Color &rhs) const
	{
		return (*((int *)this) == *((int *)&rhs));
	}

	bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	Color &operator=(const Color &rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

private:
	unsigned char _color[4];
};


#endif // COLOR_H