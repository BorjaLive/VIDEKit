#pragma once

#include "../../Extra/Vector2D.h"

#include "ItemUI.h"

#include <string>
#include <vector>
#include <functional>

namespace VIDEGui {

	class WindowUI
	{
	private:
		struct Properties {
			ImVec2 minSize;
			ImVec2 maxSize;
			std::string title;
			ImVec2 position;
			ImVec2 size;
		};

		struct System {
			bool settingPosition;
			bool settingSize;
			VIDEKit::Vector2D targetSetPosition;
			VIDEKit::Size2D targetSetSize;
		};

		WindowUI::Properties m_properties;
		WindowUI::System m_system;

		std::vector<ItemUI*> m_items;

		std::function<void(VIDEGui::WindowUI *window,const std::string& title)> m_customRenderingCallback;

	public:
		struct Control {
			bool mouseOver;
			bool resizing;
			ImVec2 currentSize;
			bool open;
			bool moving;
		};

		WindowUI::Control control;

		uint32_t windowID;

	public:
		WindowUI(const uint32_t &id,const std::function<void(VIDEGui::WindowUI *window, const std::string& title)> customRendering = std::function<void(VIDEGui::WindowUI *window, const std::string& title)>());
		~WindowUI();

		bool create(const std::string &title, const ImVec2 &minSize = ImVec2(0,0), const ImVec2 &maxSize = ImVec2(10000,10000));
		void display();

		void addItem(ItemUI *item);

		void setPosition(const VIDEKit::Vector2D &position);
		void setSize(const VIDEKit::Size2D &size);

		bool pointOver(const VIDEKit::Vector2D &point);

		std::vector<ItemUI*> getItems();

	private:
		void poll();

	public:
		Properties &getProperties();

		//CALLBACKS
	public:
		void onSize(ImGuiSizeCallbackData *data);
	};

}