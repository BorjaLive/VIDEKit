#pragma once

#include "../../imgui/header/imgui.h"
#include <functional>

namespace VIDEGui {

	class ItemUI
	{
	public:
		enum ItemType {
			IT_Button,
			IT_Label,
			IT_Edit,
			IT_TreeView
		};

	public:
		struct Callbacks {
			std::function<void()> onClick;
			std::function<void()> onEntered;
			std::function<void()> onExited;
			std::function<void(void*)> onSelected;
			std::function<void(void*)> onUnselected;

			Callbacks(
				std::function<void()> clicked = std::function<void()>(), 
				std::function<void()> entered = std::function<void()>(),
				std::function<void(void*)> selected = std::function<void(void*)>(),
				std::function<void(void*)> unselected = std::function<void(void*)>(),
				std::function<void()> exited = std::function<void()>()){
				
				onClick = clicked;
				onEntered = entered;
				onSelected = selected;
				onExited = exited;
				onUnselected = unselected;
			}
		};

		struct Control {
			bool canBeSelected;
			bool dragDropEnabled;
		};

		struct System {
			bool hover;
			bool dragging;
			bool selected;
		};


	protected:
		Control control;
		System system;
		std::string m_id;
	protected:
		Callbacks m_callbacks;
		ItemType m_type;
	public:
		ItemUI(std::string id,Callbacks callbacks, ItemType type);
		~ItemUI();

		virtual void display() = 0;
		virtual void poll() = 0;

		Control &getControl();

		const std::string &id();

		void setId(const std::string &id);

		void *extra;

		System &getSystem();

	protected:
		void updateStates();
	};

}