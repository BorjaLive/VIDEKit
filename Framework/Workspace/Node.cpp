#include "Node.h"

#include "Connection.h"
#include "../Variable.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace VIDEKit {

	Node::Node() :VIDEKit::Rectangle(VIDEKit::Transform(), VIDEKit::Size2D()) {

	}

	Node::Node(const std::string &name,const float &radius, const VIDEKit::Transform &transform) : VIDEKit::Rectangle(transform, VIDEKit::Size2D(radius*2.0f, radius*2.0f)){
		
		m_properties.name = name;
		m_properties.accent = Color("#CCCCCC");
		m_properties.type = Node::NodeType::NT_Function;
		m_properties.dragging = false;
		m_properties.selected = false;

		m_properties.radius = radius;

		m_properties.connectionsLength = 60.f;

		fillColor = Color("#212121");
		borderColor = m_properties.accent;
		borderWidth = 2;
		cornerRadius = m_properties.radius;

		
		if (!m_font.loadFromFile("fonts/Montserrat-Regular.ttf")) {
			std::cout << "Error loading" << std::endl;
		}

		m_caption = sf::Text(m_properties.name,m_font);
		m_caption.setFillColor((sf::Color)Color("#FFFFFF"));
		m_caption.setCharacterSize(20);
		m_caption.setPosition((sf::Vector2f)m_transform.position);

		extra = nullptr;

		blockID = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

		boost::replace_all(blockID, "-", "");

		animationTimeCoordinator = 0.0;
		animationTimeDelta = 0.01;
	}


	Node::~Node()
	{
	}
	
	void Node::load(std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos, std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos){

		//RESET THIS NODES CONNECTIONS
		m_properties.connections.clear();

		uint32_t inputCount = inputInfos.size();
		uint32_t outputCount = outputInfos.size();
		uint32_t inputIndex = 0;
		uint32_t outputIndex = 0;

		float inputAngleOffset = (M_PI_2 / (float)inputCount);
		float outputAngleOffset = (M_PI_2 / (float)outputCount);


		for (int i = -floorf(inputCount/ 2); i <= floorf(inputCount / 2); i++) {
			if (inputCount % 2 == 0 && i == 0) continue;

			float angle = (i * inputAngleOffset) + (M_PI_2);

			Vector2D fromTarget = Vector2D(cosf(angle) * (m_properties.radius + m_properties.connectionsLength), sinf(angle) * (m_properties.radius + m_properties.connectionsLength)) + m_transform.position;

			VIDEKit::Connection *con = new VIDEKit::Connection();

			con->getPhysics().anchorCurrent = m_transform.position;
			con->getLine().thickness = 2.0f;
			con->getInfo().connectionType = VIDEKit::Connection::ConnectionType::CT_Input;
			con->getInfo().fromPoint = fromTarget;
			con->getInfo().to = this;
			con->getInfo().from = nullptr;
			con->getInfo().type = inputInfos[inputIndex].type;

			m_properties.connections.push_back(con);

			inputIndex++;
		}



		for (int i = -floorf(outputCount / 2); i <= floorf(outputCount / 2); i++) {
			if (outputCount % 2 == 0 && i == 0) continue;

			float angle = (i * outputAngleOffset) - (M_PI_2);

			Vector2D toTarget = Vector2D(cosf(angle) * (m_properties.radius + m_properties.connectionsLength), sinf(angle) * (m_properties.radius + m_properties.connectionsLength)) + m_transform.position;

			VIDEKit::Connection *con = new VIDEKit::Connection();

			con->getPhysics().anchorCurrent = m_transform.position;
			con->getLine().thickness = 2.0f;
			con->getInfo().connectionType = VIDEKit::Connection::ConnectionType::CT_Output;
			con->getInfo().toPoint = toTarget;
			con->getInfo().to = nullptr;
			con->getInfo().from = this;
			con->getInfo().type = outputInfos[outputIndex].type;

			m_properties.connections.push_back(con);

			outputIndex++;
		}
	}
	

	void Node::render(SFMLRenderWindow *window) {

		if (!m_properties.selected)
			this->borderColor = m_properties.accent;
		else
			this->borderColor = Color("#FF6800");


		this->draw(window);
		
		m_caption.setPosition((sf::Vector2f)(m_transform.position));

		window->draw(m_caption);

	}

	void Node::render(SFMLRenderTexture *texture) {

		if (!m_properties.selected)
			this->borderColor = m_properties.accent;
		else
			this->borderColor = Color("#FF6800");

		if (m_properties.isDebugged) {
			this->borderColor = Color::CInterp(Color("#94FF0000"),Color("#94FF00"), (sinf(animationTimeCoordinator * 10.0f) + 1.0f)/2.0f);
		}

		this->draw(texture);


		if (m_properties.type == VIDEKit::Node::NodeType::NT_Variable && extra != nullptr) {
			m_properties.name = ((VIDEKit::Variable*)extra)->getName();
		}

		m_caption.setString(m_properties.name);
		
		sf::FloatRect captionBounds = m_caption.getLocalBounds();
		
		bool overflow = false;
		
		if (captionBounds.width >= this->m_properties.radius * 2.0f) {
			overflow = true;
			if (!m_properties.dragging) {
				m_caption.setString(m_properties.name.substr(0, 4) + "...");
			}
		}
		
		captionBounds = m_caption.getLocalBounds();
		
		if (!m_properties.dragging || !overflow) {
			m_caption.setPosition((sf::Vector2f)Vector2D::VInterp(m_caption.getPosition(), m_transform.position - Vector2D(captionBounds.width / 2.0f, captionBounds.height*0.85f), 0.2));
		}
		else{
			m_caption.setPosition((sf::Vector2f)Vector2D::VInterp(m_caption.getPosition(), m_transform.position - Vector2D(captionBounds.width / 2.0f, captionBounds.height*1.5f + m_properties.radius), 0.2));
		}
		texture->draw(m_caption);

		animationTimeCoordinator += animationTimeDelta;
	}

	bool Node::pointOver(const VIDEKit::Vector2D & point)
	{
		return Vector2D::Length(m_transform.position - point) <= m_properties.radius;
	}

	void Node::update()
	{
		if (m_properties.type == VIDEKit::Node::NodeType::NT_Variable && extra != nullptr) {
			m_properties.name = ((VIDEKit::Variable*)extra)->getName();
		}
	}


	Transform &Node::getTransform() {
		return m_transform;
	}

	Node::NodeProperties &Node::getProperties() {
		return m_properties;
	}
}