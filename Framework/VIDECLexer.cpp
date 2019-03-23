#include "VIDECLexer.h"

#include <iostream>

VIDECLexer::VIDECLexer()
{
}


VIDECLexer::~VIDECLexer()
{
}

bool VIDECLexer::fromFile(const std::string &filename) {
	code = readFile(filename);

	bool valid = validate(code);



	return valid;
}

bool VIDECLexer::validate(const std::string &content) {
	bool valid = true;

	std::vector<std::string> codeLines;

	//SPLIT THE CODE IN LINES
	boost::split_regex(codeLines, content, boost::regex("\n"));
	
	boost::regex sectionDeclRegex(Regex_SectionDecl);
	boost::regex behaviorDeclRegex(Regex_BehaviorDecl);
	boost::regex dataTypesDeclRegex(Regex_DataTypesDecl);

	//GET ALL SECTIONS
	for (int i = 0; i < codeLines.size(); i++) {

		//REMOVE COMMENTS
		codeLines[i] = boost::regex_replace(codeLines[i], boost::regex(Token_Comment + Token_Any), "");

		boost::smatch regexValues;

		if (boost::regex_match(codeLines[i], regexValues, sectionDeclRegex)) {
			std::string sectionName = "", sectionContent = "";
			bool foundEnd = false;

			sectionName = std::string(regexValues[3].first, regexValues[3].second);
		
			i++;

			while (!foundEnd && i < codeLines.size()) {

				//REMOVE COMMENTS
				codeLines[i] = boost::regex_replace(codeLines[i], boost::regex(Token_Comment + Token_Any), "");

				if (boost::regex_match(codeLines[i], boost::regex(Regex_End))) {
					foundEnd = true;
				}
				else if (boost::regex_match(codeLines[i], boost::regex(Regex_Begin))) {
					foundEnd = false;
					valid = false;
					break;
				}
				else {
					sectionContent += codeLines[i] + "\n";
					i++;
				}
			}

			if (!foundEnd) {
				//SECTION DECLARATED BUT NOT CORRECT
				valid = false;
				break;
			}

			Section *section = new Section();

			//SECTION SUCCESSFULLY DECLARED AND CLOSED
			if (!validateSection(sectionName,sectionContent, section)) {
				//INVALID SECTION
				valid = false;
				break;
			}

			sections.push_back(section);
		}
		else if (boost::regex_match(codeLines[i], regexValues, behaviorDeclRegex)) {

			std::string behaviorSectionName = "", behaviorContent = "";
			bool foundEnd = false;

			behaviorSectionName = std::string(regexValues[3].first, regexValues[3].second);

			i++;

			while (!foundEnd && i < codeLines.size()) {

				//REMOVE COMMENTS
				codeLines[i] = boost::regex_replace(codeLines[i], boost::regex(Token_Comment + Token_Any), "");

				if (boost::regex_match(codeLines[i], boost::regex(Regex_End))) {
					foundEnd = true;
				}
				else if (boost::regex_match(codeLines[i], boost::regex(Regex_Begin))) {
					foundEnd = false;
					valid = false;
					break;
				}
				else {
					behaviorContent += codeLines[i] + "\n";
					i++;
				}
			}

			if (!foundEnd) {
				//SECTION DECLARATED BUT NOT CORRECT
				valid = false;
				break;
			}

			Behavior *behavior = new Behavior();

			int ind = -1;

			for (int i = 0; i < sections.size(); i++) {
				if (sections[i]->name.compare(behaviorSectionName) == 0) {
					ind = i;
					break;
				}
			}

			if (ind == -1) {
				valid = false;
				break;
			}

			//BEHAVIOR SUCCESSFULLY DECLARED AND CLOSED
			if (!validateBehavior(behaviorSectionName, *sections[ind], behaviorContent, behavior)) {
				//INVALID SECTION
				valid = false;
				break;
			}
			
			behaviors.push_back(behavior);
		}
		else if (boost::regex_match(codeLines[i], regexValues, dataTypesDeclRegex)) {
			bool foundEnd = false;
			i++;

			while (!foundEnd && i < codeLines.size()) {

				if (boost::regex_match(codeLines[i], boost::regex(Regex_End))) {
					foundEnd = true;
					break;
				}
				else if (!boost::regex_match(codeLines[i], boost::regex(Regex_AnySpace))) {
					VIDECLexer::Field *parsedField = new VIDECLexer::Field();
					if (!this->validateField(codeLines[i], parsedField)) {
						valid = false;
						break;
					}

					VIDECLexer::DataType *type = new VIDECLexer::DataType();

					std::string name = parsedField->get("target")->value;

					std::string color = parsedField->get("DColor")->value;

					std::string pyclass = parsedField->get("pyclass")->value;

					std::string defValueType = parsedField->get("default")->value;

					type->typeColor = VIDEKit::Color(color);
					type->typeName = name;
					type->pyClassName = pyclass;
					type->defaultTypeValue = defValueType;

					dataTypes.push_back(type);
				}
				i++;
			}

		}
		else if (codeLines[i].size() != 0 && codeLines[i][0] != '\0') {
			valid = false;
			break;
		}
	}





	return valid;
}

VIDECLexer::BehaviorDefinition * VIDECLexer::get(const std::string & section, const std::string & object)
{
	for (int i = 0; i < behaviors.size(); i++) {
		if (behaviors[i]->name.compare(section) == 0) {
			for (int j = 0; j < behaviors[i]->definitions.size(); j++) {
				if (behaviors[i]->definitions[j].fooName.compare(object) == 0) {
					return &behaviors[i]->definitions[j];
				}
			}
			break;
		}
	}

	return nullptr;
}

VIDECLexer::DataType * VIDECLexer::getType(const std::string & typeName)
{
	VIDECLexer::DataType *ret = nullptr;

	for (int i = 0; i < dataTypes.size(); i++) {
		if (dataTypes[i]->typeName.compare(typeName) == 0) {
			ret = dataTypes[i];
			break;
		}
	}

	return ret;
}

bool VIDECLexer::validateSection(const std::string &sectionName,const std::string &sectionContent, Section *parsedSection) {
	bool valid = true;

	Section *section = new Section();

	section->name = sectionName;

	std::vector<std::string> codeLines;
	
	boost::split_regex(codeLines, sectionContent, boost::regex("\n"));

	for (int i = 0; i < codeLines.size(); i++) {

		boost::smatch regexValues;

		if (boost::regex_match(codeLines[i], regexValues, boost::regex(Regex_ObjectDecl))) {

			bool foundEnd = false;

			std::string objectName = "", objectContent = "";
			
			objectName = std::string(regexValues[2].first, regexValues[2].second);

			i++;

			while (i < codeLines.size() && !foundEnd) {

				if (boost::regex_match(codeLines[i], boost::regex(Regex_AnySpace + Token_CloseObject + Regex_AnySpace))) {
					foundEnd = true;
					break;
				}
				else {
					objectContent += codeLines[i] + "\n";
					i++;
				}
			}

			//INVALID OBJECT DECLARATION(OPEN BUT NOT CLOSED)
			if (!foundEnd) {
				valid = false;
				break;
			}

			Object object;

			valid = validateObject(objectName,objectContent, &object);
			if (!valid) {
				break;
			}

			section->objects.push_back(object);
		}
		else if(codeLines[i].size() != 0 && codeLines[i][0] != '\0' && !boost::regex_match(codeLines[i], boost::regex(Regex_AnySpace))){
			valid = false;
			break;
		}
	}

	if (parsedSection != nullptr && valid) {
		memcpy(parsedSection, section, sizeof(Section));
	}

	return valid;
}

bool VIDECLexer::validateObject(const std::string &objectName, const std::string &objectContent, Object *parsedObject) {
	bool valid = true;

	Object *object = new Object();

	object->name = objectName;

	std::vector<std::string> codeLines;

	boost::split_regex(codeLines, objectContent, boost::regex("\n"));

	for (int i = 0; i < codeLines.size(); i++) {

		boost::smatch regexValues;

		if (boost::regex_match(codeLines[i], regexValues, boost::regex(Regex_ListDecl))) {
			bool foundEnd = false;
			std::string listName = "", listContent = "";

			listName = std::string(regexValues[2].first, regexValues[2].second);

			i++;

			while (!foundEnd && i < codeLines.size()) {

				if (boost::regex_match(codeLines[i], boost::regex(Regex_AnySpace + Token_CloseList + Regex_AnySpace))) {
					foundEnd = true;
					i++;
					break;
				}
				else {
					listContent += codeLines[i] + "\n";
					i++;
				}
			}

			//INVALID LIST DECLARATION(OPEN BUT NOT CLOSED)
			if (!foundEnd) {
				valid = false;
				break;
			}

			List l;
			valid = validateList(listName,listContent, &l);
			if (!valid) {
				break;
			}

			object->lists.push_back(l);
		}
		else if (codeLines[i].size() != 0 && codeLines[i][0] != '\0' && !boost::regex_match(codeLines[i], boost::regex(Regex_AnySpace))) {
			valid = false;
			break;
		}
	}

	if (parsedObject != nullptr && valid) {
		memcpy(parsedObject, object, sizeof(Object));
	}

	return valid;
}

bool VIDECLexer::validateList(const std::string &listName, const std::string &listContent, List *parsedList) {
	bool valid = true;
	
	List *list = new List();

	list->name = listName;

	std::vector<std::string> codeLines;

	boost::split_regex(codeLines, listContent, boost::regex("\n"));

	for (int i = 0; i < codeLines.size(); i++) {


		
		if (boost::regex_match(codeLines[i], boost::regex("([[:space:]]+)?\<([ -~]+)?\>([[:space:]]+)?"))) {
			Field f;
			valid = validateField(codeLines[i], &f);
			list->fields.push_back(f);
		}
		else if (codeLines[i].size() != 0) {
			valid = false;
			break;
		}
	}

	if (parsedList != nullptr && valid) {
		memcpy(parsedList, list, sizeof(List));
	}

	return valid;
}

bool VIDECLexer::validateField(const std::string& fieldContent, Field *parsedField) {
	bool valid = false;

	Field *field = new Field();

	if (repeated(fieldContent, "\<") == 1 && repeated(fieldContent, "\>") == 1) {

		std::string fieldBody = fieldContent.substr(fieldContent.find("\<") + 1, (fieldContent.find("\>") - fieldContent.find("\<") - 1));

		std::vector<std::string> properties;

		boost::split_regex(properties, fieldBody, boost::regex(";"));

		for (int i = 0; i < properties.size(); i++) {

			Property p;

			if (properties[i].size() == 0) {
				valid = false;
				break;
			}
			else {
				valid = validateProperty(properties[i], &p);
				if (!valid) {
					break;
				}

				field->properties.push_back(p);
			}
		}
	}
	else {
		valid = false;
	}

	if (parsedField != nullptr && valid) {
		memcpy(parsedField, field, sizeof(Field));
	}

	return valid;
}

bool VIDECLexer::validateProperty(const std::string &propertyContent, Property *parsedProperty) {
	bool valid = true;

	Property *property = new Property();

	boost::smatch regexValues;

	boost::regex valuedProperty(Regex_PropertyDeclValued);
	boost::regex unvaluedProperty(Regex_PropertyDeclUnvalued);

	if (boost::regex_match(propertyContent, regexValues, valuedProperty)) {
		property->name = std::string(regexValues[2].first, regexValues[2].second);
		property->type = std::string(regexValues[6].first, regexValues[6].second);
		property->value = std::string(regexValues[10].first, regexValues[10].second);
	}
	else if(boost::regex_match(propertyContent, regexValues, unvaluedProperty)){
		property->name = std::string(regexValues[2].first, regexValues[2].second);
		property->type = std::string(regexValues[6].first, regexValues[6].second);
		property->value = "undefined";
	}
	else{
		//INVALID PROPERTY DECLARATION
		property->name = "undefined";
		property->type = "undefined";
		property->value = "undefined";
		valid = false;
	}

	if (parsedProperty != nullptr && valid) {
		memcpy(parsedProperty, property, sizeof(Property));
	}

	return valid;
}

bool VIDECLexer::validateBehavior(const std::string &behaviorName, Section behaviorSection, const std::string &behaviorContent, Behavior* parsedBehavior) {
	bool valid = true;

	Behavior *behavior = new Behavior();

	behavior->name = behaviorName;
	behavior->section = behaviorSection;
	
	std::vector<std::string> codeLines;

	boost::split_regex(codeLines, behaviorContent, boost::regex("\n"));

	for (int i = 0; i < codeLines.size(); i++) {
		boost::smatch regexValues;

		if (boost::regex_match(codeLines[i], regexValues, boost::regex(Regex_PyFoo))) {

			bool foundEnd = false;
			std::string fooContent = codeLines[i] + "\n";
			std::string fooName = std::string(regexValues[2].first, regexValues[2].second);

			i++;

			while (i < codeLines.size() && !foundEnd) {

				if (boost::regex_match(codeLines[i], boost::regex(Regex_PyRet))) {
					fooContent += codeLines[i] + "\n";
					foundEnd = true;
					break;
				}
				else {
					fooContent += codeLines[i] + "\n";
					i++;
				}
			}

			if (!foundEnd) {
				valid = false;
				break;
			}

			int targetIndex = -1;
			for (int i = 0; i < behaviorSection.objects.size(); i++) {
				if (behaviorSection.objects[i].name.compare(fooName) == 0) {
					targetIndex = i;
					break;
				}
			}

			if (targetIndex == -1) {
				valid = false;
				break;
			}

			BehaviorDefinition def;

			def.fooName = fooName;
			def.sectionTarget = behaviorName;
			def.pyFooName = def.sectionTarget + "_" + def.fooName;
			def.pyCode = fooContent;
			def.targetObject = behaviorSection.objects[targetIndex];

			boost::replace_first(def.pyCode, def.fooName, def.pyFooName);
			
			try {

				PyRun_SimpleString(def.pyCode.c_str());

				PyObject *mainModule = PyUnicode_FromString((char*)"__main__");
				PyObject* myModule = PyImport_Import(mainModule);

				def.callable = boost::python::object(boost::python::handle<>(PyObject_GetAttrString(myModule, def.pyFooName.c_str())));				
			}
			catch (boost::python::error_already_set &e) {
				PyErr_Print();
				valid = false;
				break;
			}
			
			behavior->definitions.push_back(def);
		}
	}

	if (parsedBehavior != nullptr) {
		memcpy(parsedBehavior, behavior, sizeof(Behavior));
	}

	return valid;
}

std::vector<VIDECLexer::Section*> &VIDECLexer::getSections() {
	return sections;
}

std::vector<VIDECLexer::Behavior*> &VIDECLexer::getBehaviors() {
	return behaviors;
}

std::vector<VIDECLexer::DataType*> &VIDECLexer::getDataTypes() {
	return dataTypes;
}