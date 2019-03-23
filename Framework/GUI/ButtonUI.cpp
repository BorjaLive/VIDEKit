#include "ButtonUI.h"

namespace VIDEGui {

	ButtonUI::ButtonUI(const std::string &id, const std::string &text, const VIDEKit::Size2D &size, const VIDEGui::ItemUI::Callbacks &callbacks) : ItemUI(id, callbacks, ItemType::IT_Button)
	{
		m_text = text;
		m_size = size;
	}


	ButtonUI::~ButtonUI()
	{
	}

	void ButtonUI::display()
	{
		ImGui::Button((m_text).c_str(), ImVec2(m_size.width, m_size.height));

		this->poll();
	}

	void ButtonUI::poll()
	{
		updateStates();


	}

	void ButtonUI::setText(const std::string &text) {
		m_text = text;
	}

	const std::string &ButtonUI::getText() {
		return m_text;
	}
}