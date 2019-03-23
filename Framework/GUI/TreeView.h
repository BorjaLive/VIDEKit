#pragma once

#include "ItemUI.h"

#include <functional>
#include <vector>

namespace VIDEGui {

	class TreeView : public VIDEGui::ItemUI
	{
	public:
		struct ChildItem {
			ItemUI *item;
			bool draggable;
			std::string itemNameStr;
			std::string dragTagName;
			TreeView *parent;
		};

	private:
		std::string m_rootName;
		std::vector<ChildItem> childItems;
		std::function<void(TreeView*)> onDrop;
		std::function<void(TreeView*)> onDrag;

		ChildItem *selectedChild;

		bool extraButton;
		std::string extraButtonTag;
		std::function<void(TreeView*)> extraButtonPress;

	public:
		TreeView(const std::string &id, const std::string &rootName,const std::function<void(TreeView*)> &dragEvent = std::function<void(TreeView*)>());
		~TreeView();

		void setExtraButton(bool enabled, const std::string &tag, std::function<void(TreeView*)> = std::function<void(TreeView*)>());

		virtual void poll();
		virtual void display();

		void addItem(ChildItem& item);

		ChildItem* getSelectedChild();

		void clearSelection();

		std::vector<ChildItem> &getChilds();

		void clear();
	};

}