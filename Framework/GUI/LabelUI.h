#pragma once

#include "ItemUI.h"

#include <string>

namespace VIDEGui {

	class LabelUI : public ItemUI
	{
	private:
		std::string m_text;
		bool m_selectable;
	public:
		LabelUI(const std::string &id, const std::string &text, Callbacks callbacks, bool selectable);
		~LabelUI();

		std::string getText();
		void setText(const std::string &text);

		virtual void display();
		virtual void poll();
	};

}