#include "WindowUI.h"

#include <iostream>

#include "GraphicsUI.h"
#include "../../Extra/Utils.h"


static void StaticWindowSizeCallback(ImGuiSizeCallbackData *data) {
	((VIDEGui::WindowUI*)(data->UserData))->onSize(data);
}

namespace VIDEGui {

	WindowUI::WindowUI(const uint32_t &id,const std::function<void(VIDEGui::WindowUI *window, const std::string& title)> customRendering){
		//CTOR
		windowID = id;
		m_properties = {};
		m_system = {};
		control.open = true;
		m_customRenderingCallback = customRendering;
		control.mouseOver = false;
	}

	WindowUI::~WindowUI() {

	}

	bool WindowUI::create(const std::string &title, const ImVec2 &minSize, const ImVec2 &maxSize) {
		
		m_properties.title = title;
		
		m_properties.minSize = minSize;
		m_properties.maxSize = maxSize;

		return true;
	}

	void WindowUI::display() {
		
		if (control.open) {

			ImGui::SetNextWindowSizeConstraints(m_properties.minSize, m_properties.maxSize, StaticWindowSizeCallback, this);

			if (m_system.settingPosition) {
				m_system.settingPosition = false;
				ImGui::SetNextWindowPos(ImVec2(m_system.targetSetPosition.x, m_system.targetSetPosition.y));
			}

			if (m_system.settingSize) {
				m_system.settingSize = false;
				ImGui::SetNextWindowSize(ImVec2(m_system.targetSetSize.width, m_system.targetSetSize.height));
			}


			if (m_customRenderingCallback) {
				m_customRenderingCallback(this, m_properties.title);
			}
			else {
				if (ImGui::Begin(m_properties.title.c_str(), &control.open)) {

					//CONTROL FOR WINDOWING DRAG SYSTEM
					control.mouseOver = false;

					m_properties.position = ImGui::GetWindowPos();
					m_properties.size = ImGui::GetWindowSize();

					poll();

					ImGui::End();
				}
			}

			//ImGui::EndTabBar();

		}
	}

	void WindowUI::setPosition(const VIDEKit::Vector2D &position) {
		m_system.targetSetPosition = position;
		m_system.settingPosition = true;
	}

	void WindowUI::setSize(const VIDEKit::Size2D &size) {
		m_system.targetSetSize = size;
		m_system.settingSize = true;
	}

	bool WindowUI::pointOver(const VIDEKit::Vector2D & point)
	{
		return (PointRect(point, (sf::Vector2f)m_properties.position, (sf::Vector2f)m_properties.size));
	}

	std::vector<ItemUI*> WindowUI::getItems()
	{
		return m_items;
	}


	void WindowUI::addItem(ItemUI * item){
		m_items.push_back(item);
	}

	void WindowUI::poll(){

		//UPDATE CHILD ITEMS
		for (auto &child : m_items) {
			child->display();
		}

	}

	WindowUI::Properties &WindowUI::getProperties() {
		return m_properties;
	}


	void WindowUI::onSize(ImGuiSizeCallbackData * sizeData)
	{
		VIDEKit::Vector2D current = (VIDEKit::Vector2D)(sf::Vector2f)sizeData->CurrentSize;
		VIDEKit::Vector2D desired = (VIDEKit::Vector2D)(sf::Vector2f)sizeData->DesiredSize;

		if (current != desired && desired != VIDEKit::Vector2D(32, 40)) {
			control.resizing = true;
		}
		else {
			control.resizing = false;
		}

		control.currentSize = sizeData->CurrentSize;

	}
}
