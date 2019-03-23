#pragma once

#include <boost/algorithm/string_regex.hpp>
#include <boost/regex.hpp>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else 
#include <boost/python.hpp>
#endif


#include <string>
#include <vector>

#include "../Extra/Utils.h"

#include "../Extra/Color.h"

class VIDECLexer
{
public:
	
	struct Property {
		std::string name;
		std::string type;
		std::string value;
	};

	struct Field {
		std::vector<Property> properties;

		//UTILS
		Property *get(const std::string &pName) {
			Property *ret = nullptr;

			for (int i = 0; i < properties.size(); i++) {
				if (properties[i].name.compare(pName) == 0) {
					ret = new Property();
					ret->name = properties[i].name;
					ret->type = properties[i].type;
					if (ret->type.compare("string") == 0) {
						ret->value = properties[i].value.substr(1, properties[i].value.size()-2);
					}
					else {
						ret->value = properties[i].value;
					}
					break;
				}
			}

			return ret;
		}
	};

	struct List {
		std::string name;
		std::vector<Field> fields;
	};

	struct Object {
		std::string name;
		std::vector<List> lists;

		List* getList(const std::string &listName) {
			for (int i = 0; i < lists.size(); i++) {
				if (lists[i].name.compare(listName) == 0) {
					return &lists[i];
				}
			}

			return nullptr;
		}
	};

	struct DataType {
		std::string typeName;
		VIDEKit::Color typeColor;
		std::string pyClassName;
		std::string defaultTypeValue;
	};


	struct Section {
		std::string name;
		std::vector<Object> objects;
	};

	struct BehaviorDefinition {
		std::string sectionTarget;
		std::string fooName;
		std::string pyFooName;
		std::string pyCode;
		Object targetObject;
		boost::python::object callable;
	};

	struct Behavior {
		std::string name;
		Section section;
		std::vector<BehaviorDefinition> definitions;
	};


private:
	std::string code;
	std::vector<DataType*> dataTypes;
	std::vector<Section*> sections;
	std::vector<Behavior*> behaviors;

public:
	VIDECLexer();
	~VIDECLexer();

	bool fromFile(const std::string &filename);
	bool validate(const std::string &content);

	std::vector<DataType*> &getDataTypes();
	std::vector<Section*> &getSections();
	std::vector<Behavior*> &getBehaviors();

	BehaviorDefinition *get(const std::string &section, const std::string &object);

	DataType *getType(const std::string &typeName);

private:
	static bool validateSection(const std::string &sectionName, const std::string &sectionContent, Section *parsedSection = nullptr);
	static bool validateObject(const std::string &objectName, const std::string &objectContent, Object *parsedObject = nullptr);
	static bool validateList(const std::string &listName,const std::string &listContent, List *parsedList = nullptr);
	static bool validateField(const std::string& field, Field *parsedField = nullptr);
	static bool validateProperty(const std::string &propertyContent, Property *parsedProperty = nullptr);

	static bool validateBehavior(const std::string &behaviorName, Section behaviorSection, const std::string &behaviorContent, Behavior* parsedBehavior);
};

//KEYWORDS DICTIONARY
static const std::string Key_Begin = "BEGIN";
static const std::string Key_Section = "SECTION";
static const std::string Key_Behavior = "BEHAVIOR";
static const std::string Key_DataTypes = "DATA-TYPES";
static const std::string Key_End = "END";
static const std::string Key_Import = "IMPORT";

//TOKENS
static const std::string Token_Space = " ";
static const std::string Token_Tab = "\t";
static const std::string Token_EndLine = "\n";
static const std::string Token_EndString = "\0";
static const std::string Token_Any = "([ \\t-~]+)?";
static const std::string Token_OpenParenthesis = "\\\(";
static const std::string Token_CloseParenthesis = "\\\)";

//TOKENS VIDEC
static const std::string Token_OpenObject = "\\\{";
static const std::string Token_CloseObject = "\\\}";
static const std::string Token_OpenList = "\\\[";
static const std::string Token_CloseList = "\\\]";
static const std::string Token_AvailableStr = " _.\\/!-+¿?'\<\>";
static const std::string Token_Arrow = "\-\>";
static const std::string Token_OpenProperty = "\<";
static const std::string Token_CloseProperty = "\>";
static const std::string Token_TypeAsign = ":";
static const std::string Token_ValueAsign = "=";
static const std::string Token_Comment = "//";

//REGEX DICTIONARY

static const std::string Regex_AnySpace = "([[:space:]]+)?";
static const std::string Regex_Identifier = "([[:alpha:]_]([[:alnum:]_]+)?)";


//TYPES
static const std::string Regex_Number = "((+|-)?[[:decimal:]]+([\\.][[:decimal:]]+)?)";
static const std::string Regex_String = "\"([[:alnum:]"+Token_AvailableStr+"])\"";

static const std::string Regex_ImportDecl = Key_Begin + Regex_AnySpace + Key_Import + Regex_AnySpace;
static const std::string Regex_SectionDecl = Key_Begin + Regex_AnySpace + Key_Section + Regex_AnySpace + Regex_Identifier + Regex_AnySpace;
static const std::string Regex_BehaviorDecl = Key_Begin + Regex_AnySpace + Key_Behavior + Regex_AnySpace + Regex_Identifier + Regex_AnySpace;
static const std::string Regex_ObjectDecl = Regex_AnySpace + Regex_Identifier + Regex_AnySpace + Token_OpenObject + Regex_AnySpace;
static const std::string Regex_ListDecl = Regex_AnySpace + Regex_Identifier + Regex_AnySpace + Token_OpenList + Regex_AnySpace;
static const std::string Regex_DataTypesDecl = Key_Begin + Regex_AnySpace + Key_DataTypes;


static const std::string Regex_PropertyDeclUnvalued = Regex_AnySpace + Regex_Identifier + Regex_AnySpace + Token_TypeAsign + Regex_AnySpace + Regex_Identifier + Regex_AnySpace;
static const std::string Regex_PropertyDeclValued = Regex_AnySpace + Regex_Identifier + Regex_AnySpace + Token_TypeAsign + Regex_AnySpace + Regex_Identifier + Regex_AnySpace + Token_ValueAsign + Regex_AnySpace + Token_Any + Regex_AnySpace;

static const std::string Regex_Begin = Regex_AnySpace + Key_Begin + Regex_AnySpace;
static const std::string Regex_End = Regex_AnySpace + Key_End + Regex_AnySpace + "([\\0]+)?";

//BEHAVIOR DEFS

static const std::string Token_PyDef = "def";
static const std::string Token_PyRet = "return";

static const std::string Regex_PyFoo = Token_PyDef + Regex_AnySpace  + Regex_Identifier + Regex_AnySpace + Token_OpenParenthesis + Token_Any + Token_CloseParenthesis + Regex_AnySpace + ":";
static const std::string Regex_PyRet = Token_Tab + Token_PyRet + Regex_AnySpace + Token_Any;
