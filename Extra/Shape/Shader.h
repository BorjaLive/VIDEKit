#pragma once

#include <SFML/Graphics.hpp>

namespace VIDEKit {

	class Shader
	{
	private:
		sf::Shader m_shader;

	public:
		Shader(const std::string &fragment);
		~Shader();

		sf::Shader &getShader();
	};

}