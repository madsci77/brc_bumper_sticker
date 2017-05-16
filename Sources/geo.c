/*

 - Geographic math functions -
 
 Approximate distance using Pythagorean theorem: d = sqrt(x * x + y * y)
 Adjust delta x by cos(lat) to compensate for convergence of meridians
 

 Values are in semicircles (2^31/180 degrees)
 1 degree = 11930464.7111... semicircles
 
*/

#include <math.h> 
#include "geo.h"
#include "hardware.h"

#define pi 3.14159265358979323846

const float deg_tan[45] = {140, 57.2, 28.6, 19, 14.3, 11.4, 9.5, 8.1, 7.1, 6.3, 5.7, 5.1, 4.7, 4.3,
					       4, 3.73,	3.49, 3.27, 3.08, 2.90, 2.75, 2.60, 2.47, 2.35, 2.246, 2.145,
					       2.050, 1.963, 1.880,	1.804, 1.732, 1.664, 1.600, 1.540, 1.482, 1.428,
					       1.376, 1.327, 1.280, 1.235, 1.192, 1.150, 1.111, 1.072, 1.036};

// Arctangent of Y/X in degrees, compass format
unsigned int atan2deg(double y, double x)
{
	float ratio, ax, ay;
	unsigned char c;
	ax = fabs(x);
	ay = fabs(y);
	if (ax>ay)
	{
		if (y == 0) ratio = 150;
		else ratio = ax/ay;
	}
	else
	{
		if (x == 0) ratio = 150;
		else ratio = ay/ax;
	}
	for (c = 0; c < 45; c++)
	{
		if (ratio >= deg_tan[c]) break;
	}
	if (c && (deg_tan[c-1] - ratio) < (ratio - deg_tan[c])) c--;	
	if (ay>ax)
	{
		if (x > 0)
		{
			if (y > 0) return c;
			return 180-c;
		}
		if (y < 0) return 180+c;
		return 360-c;
	}
	if (x > 0)
	{
		if (y > 0) return 90-c;
		return 90+c;
	}
	if (y < 0) return 270-c;
	return 270+c;
}

void geodistance(float *dist, float *bearing, signed long lat1, signed long lon1, signed long lat2, signed long lon2)
{
	unsigned char c;
	signed long l;
	float lat, lon;
	c = (lat1 >> 24);							// Take high byte of latitude
	if (c > 0x80) c = 0 - (signed char)c;
	lat1 = lat1-lat2;							// Get dX and dY
	lon1 = lon1-lon2;
	// Apply correction for latitude zone
	if (c >= 0x3b) c = 7;						// 82.8 degrees
	else if (c >= 0x36) c = 6;					// 75.5
	else if (c >= 0x30) c = 5;					// 67.97
	else if (c >= 0x2b) c = 4;					// 60
	else if (c >= 0x24) c = 3;					// 51.3
	else if (c >= 0x1d) c = 2;					// 41.4
	else if (c >= 0x15) c = 1;					// 28.95
	l = (lon1 >> 3);							// 1/8 dX
	while (c--) lon1-=l;						// Decrement by 1/8 for each c
	lat = lat1;
	lon = lon1;
	*dist = sqrt((lat * lat) + (lon * lon)) * 0.000005797715857625;
	*bearing = atan2deg(lat, lon);
}

