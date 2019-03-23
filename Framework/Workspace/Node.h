#pragma once

#include "../../Extra/Shape/Rectangle.h"
#include "../../Extra/Transform.h"

#include "Connection.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

namespace VIDEKit {

	class Node : public VIDEKit::Rectangle
	{
	public:

		enum NodeType {
			NT_Function,
			NT_Operator,
			NT_Variable,
			NT_Constant,
			NT_Event,
			NT_Undefined
		};

		struct NodeProperties {
			std::string name;
			VIDEKit::Color accent;
			VIDEKit::Node::NodeType type;
			float radius;
			float connectionsLength;
			bool dragging;
			bool selected;
			bool isDebugged;

			//CONNECTIONS
			std::vector<class Connection*> connections;

			Connection *getConnection(const VIDEKit::Connection::ConnectionType &type,const std::string &paramName) {
				for (int i = 0; i < connections.size(); i++) {
					if (connections[i]->getInfo().type.connectionName.compare(paramName) == 0 && connections[i]->getInfo().connectionType == type) {
						return connections[i];
					}
				}

				return nullptr;
			}
		};

	private:
		VIDEKit::Node::NodeProperties m_properties;

		sf::Text m_caption;
		sf::Font m_font;

		double animationTimeCoordinator;
		double animationTimeDelta;

	public:
		Node();
		Node(const std::string &name,const float &radius = 40.0f,const VIDEKit::Transform &transform = VIDEKit::Transform());
		~Node();

		void load(std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos, std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos);

	public:
		void render(SFMLRenderWindow *window);
		void render(SFMLRenderTexture *texture);

		bool pointOver(const VIDEKit::Vector2D & point);

		void update();

	public:
		Transform &getTransform();
		NodeProperties &getProperties();

		std::string blockID;

	public:
		void *extra;
		std::string sectionName;
	};

}