#include "LabelUI.h"

#include <iostream>

namespace VIDEGui {

	LabelUI::LabelUI(const std::string &id, const std::string &text, Callbacks callbacks, bool selectable) : ItemUI(id,callbacks, ItemType::IT_Label), m_selectable(selectable)
	{
		m_text = text;
	}


	LabelUI::~LabelUI()
	{
	}

	std::string LabelUI::getText() {
		return m_text;
	}

	void LabelUI::setText(const std::string &text) {
		m_text = text;
	}

	void LabelUI::display() {
		//RENDER ON THE CURRENT WINDOW

		if (m_selectable) {
			bool lastState = system.selected;

			ImGui::Selectable(m_text.c_str(), &system.selected);

			if (system.selected != lastState) {
				if (system.selected) {
					if (m_callbacks.onSelected) {
						m_callbacks.onSelected(extra);
					}
				}
				else {
					if (m_callbacks.onUnselected) {
						m_callbacks.onUnselected(extra);
					}
				}
			}
		}
		else {
			ImGui::Text(m_text.c_str());
		}

		this->poll();
	}

	void LabelUI::poll() {
		//PROCESS ON THE NEXT ITEM
		updateStates();


	}

}