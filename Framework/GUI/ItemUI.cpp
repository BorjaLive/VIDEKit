#include "ItemUI.h"

namespace VIDEGui {

	ItemUI::ItemUI(std::string id, Callbacks callbacks, ItemUI::ItemType type) {
		m_id = id;
		m_callbacks = callbacks;
		system.dragging = system.hover = system.selected = false;
		control = {};
		m_type = type;
		extra = nullptr;
	}


	ItemUI::~ItemUI()
	{
	}

	ItemUI::Control & ItemUI::getControl()
	{
		return control;
	}

	const std::string & ItemUI::id()
	{
		// TODO: insertar una instrucción return aquí
		return m_id;
	}

	void ItemUI::setId(const std::string & id)
	{
		m_id = id;
	}

	VIDEGui::ItemUI::System & ItemUI::getSystem()
	{
		return system;
	}

	void ItemUI::updateStates() {

		if (control.dragDropEnabled) {
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

				ImGui::SetDragDropPayload(m_id.c_str(), m_id.c_str(), sizeof(const char*));

				ImGui::TextUnformatted(m_id.c_str());

				ImGui::EndDragDropSource();
			}
		}

		if (ImGui::IsItemClicked()) {
			if (m_callbacks.onClick) {
				m_callbacks.onClick();
			}
		}
		else if (ImGui::IsItemHovered() && !system.hover) {
			if (m_callbacks.onEntered) {
				m_callbacks.onEntered();
			}
			system.hover = true;
		}
		else if (!ImGui::IsItemHovered() && system.hover) {
			if (m_callbacks.onExited) {
				m_callbacks.onExited();
			}
			system.hover = false;
		}
	}
}