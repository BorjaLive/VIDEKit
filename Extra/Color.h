#pragma once

#include <stdint.h>
#include <SFML/Graphics.hpp>

using SFMLColor = sf::Color;

namespace VIDEKit {
	

	class Color
	{
	public:
		double r;
		double g;
		double b;
		double a;

	public:
		Color();
		Color(double, double, double, double = 255.0);
		Color(const std::string &hexadecimal);
		~Color();

		void CInterp(const Color &, float);
		static Color CInterp(const Color &, const Color &, float);

		explicit operator SFMLColor();
	};
}