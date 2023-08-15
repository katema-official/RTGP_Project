#include "Utils.h"

#include <iostream>

#include <iomanip>
#include <cmath>

float* HSVtoRGB(float h, float s, float v)
{
    float* rgb = new float[3];
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		rgb[0] = v;
		rgb[1] = v;
		rgb[2] = v;
		return rgb;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch(i) {
		case 0:
			rgb[0] = v;
			rgb[1] = t;
			rgb[2] = p;
			break;
		case 1:
			rgb[0] = q;
			rgb[1] = v;
			rgb[2] = p;
			break;
		case 2:
			rgb[0] = p;
			rgb[1] = v;
			rgb[2] = t;
			break;
		case 3:
			rgb[0] = p;
			rgb[1] = q;
			rgb[2] = v;
			break;
		case 4:
			rgb[0] = t;
			rgb[1] = p;
			rgb[2] = v;
			break;
		default:		// case 5:
			rgb[0] = v;
			rgb[1] = p;
			rgb[2] = q;
			break;
	}

    return rgb;
}

float* getColorFromID(int ID)
{
	int H = (49 * ID) % 360;
	float offset = 1.0f / ((float) ID + 1);

	std::cout << "offset = " << offset << std::endl;

	while(offset > 0.5f)
	{
		offset -= 0.5f;
	}
	float S = 0.50f + offset;

	offset *= 0.2f;
	while(offset > 0.5f)
	{
		offset -= 0.5f;
	}
	float V = 1.0f - offset;

	float* rgb = HSVtoRGB(H, S, V);
	return rgb;
}