#pragma once

#include "ItemUI.h"
#include "../../Extra/Vector2D.h"

namespace VIDEGui {

	class ButtonUI : public ItemUI
	{
	private:
		std::string m_text;

		VIDEKit::Size2D m_size;

	public:
		ButtonUI(const std::string &id, const std::string &text, const VIDEKit::Size2D &size = VIDEKit::Size2D(100,25) , const VIDEGui::ItemUI::Callbacks &callbacks = VIDEGui::ItemUI::Callbacks());
		~ButtonUI();

		virtual void display();
		virtual void poll();

		void setText(const std::string &text);
		const std::string &getText();
	};

}