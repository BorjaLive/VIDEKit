#pragma once

#include <string>

#include "GUI/TreeView.h"

namespace VIDEKit{

	class Variable
	{
	private:
		std::string m_name;
		std::string m_type;
		std::string m_value;
		std::string m_uuid;
		VIDEGui::TreeView::ChildItem *m_guiReference;

	public:
		Variable(VIDEGui::TreeView::ChildItem* guiReference);
		~Variable();

		void setName(const std::string &name);
		const std::string& getName();

		void setValue(const std::string &value);
		const std::string& getValue();

		void setType(const std::string &type);
		const std::string& getType();

		void setUUID(const std::string &uuid);
		const std::string &getUUID();

		VIDEGui::TreeView::ChildItem *getUIReference();

	public:
		bool isConstant;
	};

}