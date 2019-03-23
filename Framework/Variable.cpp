#include "Variable.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace VIDEKit {

	Variable::Variable(VIDEGui::TreeView::ChildItem* guiReference) : m_guiReference(guiReference)
	{
		m_uuid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
	}


	Variable::~Variable()
	{
	}

	void Variable::setName(const std::string &name) {
		m_name = name;
	}

	const std::string &Variable::getName() {
		return m_name;
	}

	void Variable::setValue(const std::string &value) {
		m_value = value;
	}

	const std::string &Variable::getValue() {
		return m_value;
	}

	void Variable::setType(const std::string &type) {
		m_type = type;
	}

	const std::string& Variable::getType() {
		return m_type;
	}

	void  Variable::setUUID(const std::string &uuid) {
		m_uuid = uuid;
	}

	const std::string& Variable::getUUID() {
		return m_uuid;
	}

	VIDEGui::TreeView::ChildItem * Variable::getUIReference()
	{
		return m_guiReference;
	}
}