/*
 * CircularKernel.cpp
 *
 *  Created on: 2013. 2. 1.
 *      Author: HDSP
 */

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cmath>

#define PI 3.14159265358979

using namespace std;



typedef struct {
	double r;       // percent
	double g;       // percent
	double b;       // percent
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // percent
	double v;       // percent
} hsv;

static hsv      rgb2hsv(rgb in);
static rgb      hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
	unsigned long nan[2]={0xffffffff, 0x7fffffff};
	double g = *( double* )nan;
	hsv         out;
	double      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min  < in.b ? min  : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max  > in.b ? max  : in.b;

	out.v = max;                                // v
	delta = max - min;
	if( max > 0.0 ) {
		out.s = (delta / max);                  // s
	} else {
		// r = g = b = 0                        // s = 0, v is undefined
		out.s = 0.0;
		out.h = g;                            // its now undefined
		return out;
	}
	if( in.r >= max )                           // > is bogus, just keeps compilor happy
		out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
	else
		if( in.g >= max )
			out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if( out.h < 0.0 )
		out.h += 360.0;

	return out;
}


rgb hsv2rgb(hsv in)
{
	unsigned long nan[2]={0xffffffff, 0x7fffffff};
	double g = *( double* )nan;
	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
		if(in.h==g) {   // in.h == NAN
			out.r = in.v;
			out.g = in.v;
			out.b = in.v;
			return out;
		}
		out.r = 0.0;
		out.g = 0.0;
		out.b = 0.0;
		return out;
	}
	hh = in.h;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch(i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

void BMPout (
		unsigned char 			*img,
		const char				*filename,
		const int				w,
		const int				h)
{
	int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);

	FILE *f;
	f = fopen(filename,"wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; i++)
	{
		fwrite(img+(w*(h-i-1)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}
	fclose(f);
}

int main() {
	int w = 2500;
	int h = 2500;
	int sz = w*h;
	unsigned char *image = new unsigned char[sz*3];
	unsigned char *ptrImage;
	unsigned char baseColorR = 0, baseColorG = 0, baseColorB = 0;
	int wCenter = w>>1;
	int hCenter = h>>1;

	double outerRadius = 1100.0000000;
	double outerRadiusSquare = outerRadius*outerRadius;
	double innerRadius = outerRadius/sqrt(2.0);
	double innerRadiusSquare = innerRadius*innerRadius;

	for (int muls=1; muls<=5; muls++) {
		ptrImage = image;
		int mul = muls;
		for (int j = 0; j < h; j++){
			for (int i = 0; i < w; i++){
				int temp = (wCenter-i)*(wCenter-i)+(hCenter-j)*(hCenter-j);
				double rad = (sin((atan2(((double)(hCenter-j)),((wCenter-i)+0.000000000001))*(mul)))+1)*120;
				hsv a,a2;
				rgb b,b2;
				a.h = rad; 				a.s = 1;				a.v = 1;
				a2.h = 240-rad;			a2.s = 1;			a2.v = 1;
				b = hsv2rgb(a);
				b2 = hsv2rgb(a2);
				if (temp<innerRadiusSquare){
					*(ptrImage++) = b.r*255;
					*(ptrImage++) = b.g*255;
					*(ptrImage++) = b.b*255;
				} else if (temp<outerRadiusSquare){
					*(ptrImage++) = b2.r*255;
					*(ptrImage++) = b2.g*255;
					*(ptrImage++) = b2.b*255;
				} else {
					*(ptrImage++) = baseColorR;
					*(ptrImage++) = baseColorG;
					*(ptrImage++) = baseColorB;
				}
			}
		}

		stringstream abc;
		abc<<"./image/img"<<muls<<".bmp";
		BMPout(image,abc.str().c_str(),w,h);
		cout<<abc.str()<<" is processed."<<endl;

	}
}


