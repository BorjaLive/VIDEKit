#include "TreeView.h"

#include "LabelUI.h"

namespace VIDEGui {

	TreeView::TreeView(	const std::string &id,
						const std::string &rootName, 
						const std::function<void(TreeView*)> &dragEvent) : m_rootName(rootName) , ItemUI(id, VIDEGui::ItemUI::Callbacks(), ItemType::IT_TreeView)
	{
		selectedChild = nullptr;
		onDrag = dragEvent;
		extraButton = false;
		extraButtonTag = "";
	}


	TreeView::~TreeView()
	{
	}

	void TreeView::setExtraButton(bool enabled, const std::string & tag, std::function<void(TreeView*)> onExtraButtonPress)
	{
		extraButton = enabled;
		extraButtonTag = tag;
		extraButtonPress = onExtraButtonPress;
	}

	void TreeView::poll() {
		updateStates();
	}

	void TreeView::display() {

		if (ImGui::TreeNode(m_rootName.c_str())) {

			if (extraButton) {
				ImGui::SameLine();
				if (ImGui::SmallButton(extraButtonTag.c_str())) {
					if (extraButtonPress) {
						extraButtonPress(this);
					}
				}
			}

			ImGui::Indent();
			for (int i = 0; i < childItems.size(); i++) {
				if (childItems[i].itemNameStr.compare("TreeView") != 0) {
					childItems[i].item->display();
					if (childItems[i].draggable) {
						if (ImGui::BeginDragDropSource()) {
							selectedChild = &childItems[i];
							ImGui::SetDragDropPayload(childItems[i].itemNameStr.c_str(), &i, sizeof(int));
							ImGui::TextUnformatted(((VIDEGui::LabelUI*)childItems[i].item)->getText().c_str());
							ImGui::EndDragDropSource();

							if (onDrag) {
								onDrag(this);
							}
						}
					}
				}
				else {
					ImGui::Unindent();
					childItems[i].item->display();
					ImGui::Indent();
				}
			}
			ImGui::Unindent();

			ImGui::TreePop();
		}

		this->poll();
	}
	void TreeView::addItem(ChildItem & item)
	{
		item.parent = this;
		childItems.push_back(item);
	}

	TreeView::ChildItem * TreeView::getSelectedChild()
	{
		return selectedChild;
	}
	void TreeView::clearSelection()
	{
		selectedChild = nullptr;
	}
	std::vector<VIDEGui::TreeView::ChildItem>& TreeView::getChilds()
	{
		return childItems;
	}
	void TreeView::clear()
	{
		childItems.clear();
		selectedChild = nullptr;
	}
}