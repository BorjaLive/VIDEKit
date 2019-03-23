#include "Color.h"

#include <sstream>
#include <math.h>

namespace VIDEKit {

	Color::Color() : r(0),g(0),b(0),a(0){}

	Color::Color(double red, double green, double blue, double alpha) : r(red), g(green), b(blue), a(alpha) {}
	Color::Color(const std::string &hexadecimal) {
		
		//	available formats:
		//	
		//	#RRGGBB || #RRGGBBAA
		
		if (hexadecimal[0] != '#') return;

		size_t len = hexadecimal.length();

		if (len == 9 || len == 7) {
			
			std::stringstream converter;
			
			int red = 0, green = 0, blue, alpha = 0;
			//RED
			converter << std::hex << hexadecimal[1] << hexadecimal[2];
			converter >> red;
			converter.clear();
			//GREEN
			converter << std::hex << hexadecimal[3] << hexadecimal[4];
			converter >> green;
			converter.clear();
			//BLUE
			converter << std::hex << hexadecimal[5] << hexadecimal[6];
			converter >> blue;
			
			if (len == 9) {
				converter.clear();
				converter << std::hex << hexadecimal[7] << hexadecimal[8];
				converter >> alpha;
			}
			else {
				alpha = 255;
			}

			r = red;
			g = green;
			b = blue;
			a = alpha;

		}
		else {
			r = g = b = a = 0;	//INVALID FORMAT
		}

	}

	Color::~Color(){}


	void Color::CInterp(const Color &B, float t) {
		t = fminf(1.0f, fmaxf(0.0f, t));

		r = (r + (B.r - r) * t);
		g = (g + (B.g - g) * t);
		r = (b + (B.b - b) * t);
		r = (a + (B.a - a) * t);
	}
	Color Color::CInterp(const Color &A,const Color &B, float t) {
		t = fminf(1.0f, fmaxf(0.0f, t));

		return Color(
			(A.r + (B.r - A.r) * t),
			(A.g + (B.g - A.g) * t),
			(A.b + (B.b - A.b) * t),
			(A.a + (B.a - A.a) * t)
		);
	}

	Color::operator SFMLColor(){
		return sf::Color(r, g, b, a);
	}

}