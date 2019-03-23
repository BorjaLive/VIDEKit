#include "Shader.h"

#include <iostream>

namespace VIDEKit {

	Shader::Shader(const std::string &fragment){
		if (!m_shader.loadFromFile(fragment, sf::Shader::Fragment)) {
			std::cout << "Error loading the shader" << std::endl;
		}
	}
	Shader::~Shader(){}

	sf::Shader &Shader::getShader() {
		return m_shader;
	}

}