#include "Connection.h"

#include "Node.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace VIDEKit {

	Connection::Connection(){

		//DRAGGABLE CONNECTION FROM(handler)
		m_fromHandleProperties = {
			8.0f,	//INNER RADIUS
			10.0f,	//OUTER RADIUS
			Color(255,255,255)	//COLOR
		};

		//DRAGGABLE CONNECTION TO(handler)
		m_toHandleProperties = {
			0.0f,	//INNER RADIUS
			8.0f,	//OUTER RADIUS
			Color(255,255,255)	//COLOR
		};

		flowAnimation.animationControllerDelta = 1.00;
		flowAnimation.animationCoordinatorTime = 0.00;
		flowAnimation.points.push_back(VIDEKit::Rectangle(VIDEKit::Transform(), VIDEKit::Size2D(10,10)));
		flowAnimation.points.back().cornerRadius = 5.0f;

		//CREATE A CIRCLE FOR THE FROM-HANDLER
		m_fromHandle = new VIDEKit::Rectangle(VIDEKit::Transform(), Size2D(m_fromHandleProperties.innerRadius * 2.0f, m_fromHandleProperties.innerRadius * 2.0f));
		m_fromHandle->cornerRadius = m_fromHandleProperties.innerRadius;

		//CREATE A CIRCLE OF THE TO-HANDLER
		m_toHandle = new VIDEKit::Rectangle(VIDEKit::Transform(), Size2D(m_toHandleProperties.outerRadius*2.0f, m_toHandleProperties.outerRadius*2.0f));
		m_toHandle->cornerRadius = m_toHandleProperties.outerRadius;

		//SET THE CONNECTION INFO
		m_info.from = nullptr;
		m_info.to = nullptr;
		m_info.fromPoint = Vector2D();
		m_info.toPoint = Vector2D();
		m_info.type = Connection::DataType();
		m_info.type.typeName = "undefined";
		m_info.type.typeColor = Color("#D16630");
		m_info.type.connectionName = "undefined";
		m_info.linkageInfo.overlapIndex = 0;

		if (!m_font.loadFromFile("fonts/Montserrat-Regular.ttf")) {
			std::cout << "Error loading" << std::endl;
		}

		m_displayText = sf::Text(m_info.type.connectionName, m_font);
		m_displayText.setFillColor((sf::Color)Color("#FFFFFF"));
		m_displayText.setCharacterSize(16);
		m_displayText.setPosition((sf::Vector2f)m_info.toPoint);

	}


	Connection::~Connection()
	{
	}

	void Connection::render(SFMLRenderWindow * window, bool calcTarget){
		
	}

	//RENDER TO TEXTURE
	void Connection::render(SFMLRenderTexture * texture, bool calcTarget) {

		m_lineShape->fillColor = m_info.type.typeColor;

		m_properties.from = (m_info.from != nullptr) ? m_info.from->getTransform().position : m_info.fromPoint;
		m_properties.to = (m_info.to != nullptr) ? m_info.to->getTransform().position : m_info.toPoint;

		if (m_info.linkageInfo.linked) {

			//RENDER

			if (m_info.linkageInfo.selfType == SLT_From) {

				//CALCULATE TARGET OFFSET OVERLAPP

				Vector2D translateDir = (m_info.linkageInfo.linkage->m_properties.to - m_properties.from).normalized().left();

				m_info.linkageInfo.linkagePhysics.targetOffset = translateDir * m_info.linkageInfo.overlapIndex * 50.0f;

				m_info.linkageInfo.linkagePhysics.anchorTarget = Vector2D::VInterp(m_properties.from, m_info.linkageInfo.linkage->m_properties.to, 0.5f);

				//UPDATE THE TARGET VELOCITY FOR THE MIDDLE POINT
				m_info.linkageInfo.linkagePhysics.anchorVelocity += (m_info.linkageInfo.linkagePhysics.anchorTarget - m_info.linkageInfo.linkagePhysics.anchorCurrent) * m_info.linkageInfo.linkagePhysics.friction;

				//UPDATE THE CURRENT MIDDLE POINT WITH THE VELOCITY
				m_info.linkageInfo.linkagePhysics.anchorCurrent += m_info.linkageInfo.linkagePhysics.anchorVelocity * m_info.linkageInfo.linkagePhysics.hardness * 0.5f;

				//UPDATE THE VELOCITY WITH THE DAMPING
				m_info.linkageInfo.linkagePhysics.anchorVelocity *= m_info.linkageInfo.linkagePhysics.damping;


				m_info.toPoint = m_info.linkageInfo.linkagePhysics.anchorCurrent;
				m_info.toPoint += m_info.linkageInfo.linkagePhysics.targetOffset;
				m_info.linkageInfo.linkage->m_info.fromPoint = m_info.linkageInfo.linkagePhysics.anchorCurrent;
				m_info.linkageInfo.linkage->m_info.fromPoint += m_info.linkageInfo.linkagePhysics.targetOffset;
			}
			else if (m_info.linkageInfo.selfType == SLT_To) {

				Vector2D translateDir = (m_info.linkageInfo.linkage->m_properties.from - m_properties.to).normalized().right();

				m_info.linkageInfo.linkagePhysics.targetOffset = translateDir * m_info.linkageInfo.overlapIndex * 50.0f;

				m_info.linkageInfo.linkagePhysics.anchorTarget = Vector2D::VInterp(m_properties.to, m_info.linkageInfo.linkage->m_properties.from, 0.5f);

				//UPDATE THE TARGET VELOCITY FOR THE MIDDLE POINT
				m_info.linkageInfo.linkagePhysics.anchorVelocity += (m_info.linkageInfo.linkagePhysics.anchorTarget - m_info.linkageInfo.linkagePhysics.anchorCurrent) * m_info.linkageInfo.linkagePhysics.friction;

				//UPDATE THE CURRENT MIDDLE POINT WITH THE VELOCITY
				m_info.linkageInfo.linkagePhysics.anchorCurrent += m_info.linkageInfo.linkagePhysics.anchorVelocity * m_info.linkageInfo.linkagePhysics.hardness * 0.5f;

				//UPDATE THE VELOCITY WITH THE DAMPING
				m_info.linkageInfo.linkagePhysics.anchorVelocity *= m_info.linkageInfo.linkagePhysics.damping;


				m_info.fromPoint = m_info.linkageInfo.linkagePhysics.anchorCurrent;
				m_info.fromPoint += m_info.linkageInfo.linkagePhysics.targetOffset;
				m_info.linkageInfo.linkage->m_info.toPoint = m_info.linkageInfo.linkagePhysics.anchorCurrent;
				m_info.linkageInfo.linkage->m_info.toPoint += m_info.linkageInfo.linkagePhysics.targetOffset;
			}
		}
		else {
			if (m_info.from != nullptr) {
				m_info.linkageInfo.selfType = SLT_From;
			}
			else if (m_info.to != nullptr) {
				m_info.linkageInfo.selfType = SLT_To;
			}
		}


		this->tick(calcTarget);
		m_lineShape->draw(texture);


		if (m_info.linkageInfo.selfType == SLT_From && m_info.linkageInfo.linked) {

			int index = 0;

			if (flowAnimation.animationCoordinatorTime >= 190.0f) {
				flowAnimation.animationCoordinatorTime = 10.0f;
			}

			index = flowAnimation.animationCoordinatorTime;

			flowAnimation.animationCoordinatorTime += flowAnimation.animationControllerDelta;

			if (index < 100) {
				flowAnimation.points[0].getTransform()->setPosition(m_lineShape->points[index]);
				flowAnimation.points[0].fillColor = m_info.type.typeColor;
			}
			else {
				flowAnimation.points[0].getTransform()->setPosition(m_info.linkageInfo.linkage->m_lineShape->points[(index + 1) - 100]);
				flowAnimation.points[0].fillColor = m_info.linkageInfo.linkage->getInfo().type.typeColor;
			}

			flowAnimation.points[0].draw(texture);
		}


		//DRAW FROM HANDLE
		m_fromHandle->cornerRadius = m_fromHandleProperties.innerRadius;
		m_fromHandle->borderWidth = (m_fromHandleProperties.outerRadius - m_fromHandleProperties.innerRadius);
		m_fromHandle->borderColor = m_fromHandleProperties.accentColor;
		//IF WE HAVE A DEFINED NODE FOR THE BEGIN SELECT IT, ELSE SET THE FROM VECTOR
		m_fromHandle->getTransform()->setPosition((m_info.from != nullptr) ? m_info.from->getTransform().position : m_info.fromPoint);
		m_fromHandle->fillColor = (!m_info.linkageInfo.linked) ? Color(10, 10, 10, 255) : Color(255, 255, 255, 255);
		m_fromHandle->draw(texture);


		//DRAW TO HANDLE
		//IF WE HAVE A DEFINED NODE FOR THE END SELECT IT, ELSE SET THE TO VECTOR
		m_toHandle->getTransform()->setPosition((m_info.to != nullptr) ? m_info.to->getTransform().position : m_info.toPoint);
		m_toHandle->cornerRadius = m_toHandleProperties.outerRadius;
		m_toHandle->draw(texture);

		bool shouldRender = false;
		bool onTop = false;
		VIDEKit::Vector2D targetPosition = VIDEKit::Vector2D();
		VIDEKit::Vector2D dirVector = VIDEKit::Vector2D();

		if (m_info.linkageInfo.linked && m_info.connectionType == CT_Input) {
			targetPosition = m_info.linkageInfo.linkagePhysics.anchorCurrent + m_info.linkageInfo.linkagePhysics.targetOffset;

			dirVector = m_properties.to - m_properties.from;

			shouldRender = true;
		}
		else if(!m_info.linkageInfo.linked){

			m_info.type.extra = "";

			shouldRender = true;

			if (m_info.from == nullptr) {
				targetPosition = m_info.fromPoint;
				dirVector = targetPosition - m_info.to->getTransform().position;
			}
			else if (m_info.to == nullptr) {
				targetPosition = m_info.toPoint;
				dirVector = targetPosition - m_info.from->getTransform().position;
			}
		}

		if (shouldRender) {

			onTop = abs(abs(atan2(dirVector.y, dirVector.x)) - (M_PI / 2.0)) > M_PI / 4.0;

			if (m_info.type.extra.size() > 0) {
				m_displayText.setString(m_info.type.connectionName + "("+m_info.type.extra+")");
			}
			else {
				m_displayText.setString(m_info.type.connectionName);
			}
			sf::FloatRect bounds = m_displayText.getLocalBounds();

			m_displayText.setPosition((sf::Vector2f)(targetPosition - Vector2D(bounds.width/2.0f, bounds.height * 0.85) + Vector2D(onTop ? 0.0f : bounds.width/2.0 + bounds.height, onTop ? bounds.height * 1.5f : 0.0f)));
			texture->draw(m_displayText);
		}
		else {
			if (m_info.linkageInfo.linked && m_info.type.connectionName.compare("exec") != 0 && m_info.type.typeName.compare("execution") == 0) {
				m_info.linkageInfo.linkage->getInfo().type.extra = m_info.type.connectionName;
			}
		}
	}



	Vector2D Connection::closestPoint(const Vector2D & point)
	{
		Vector2D ret = m_properties.from;

		float closestDistance = (m_properties.from - point).length();

		for (int i = 1; i < m_lineShape->points.size();i++) {


			if ((m_lineShape->points[i] - point).length() <= closestDistance) {
				ret = m_lineShape->points[i];
				closestDistance = (m_lineShape->points[i] - point).length();
			}
			else {
				break;
			}
		}

		return ret;
	}

	Connection::SelectionType Connection::pointOver(const Vector2D & point, Vector2D *overPoint){
		
		Connection::SelectionType ret = Connection::SelectionType::ST_None;

		if ((m_properties.from - point).length() < m_fromHandleProperties.outerRadius) {
			if (overPoint != nullptr) {
				*overPoint = m_properties.from;
			}
			ret = Connection::SelectionType::ST_FromHandler;
		}
		else if ((m_properties.to - point).length() < m_toHandleProperties.outerRadius) {
			if (overPoint != nullptr) {
				*overPoint = m_properties.to;
			}
			ret = Connection::SelectionType::ST_ToHandler;
		}
		else{

			Vector2D closest = closestPoint(point);

			if ((closest - point).length() < m_lineShape->thickness*2.0) {
				if (overPoint != nullptr) {
					*overPoint = closest;
				}
				ret = Connection::SelectionType::ST_Connection;
			}
			else {
				ret = Connection::SelectionType::ST_None;
			}

		}

		return ret;
	}


	bool VIDEKit::Connection::link(VIDEKit::Connection *other) {
		if (other == nullptr ||
			m_info.linkageInfo.linked ||
			other->m_info.linkageInfo.linked ||
			(m_info.from != nullptr && m_info.to != nullptr) ||
			(other->m_info.from != nullptr && other->m_info.to != nullptr)) { 
			return false; 
		}


		//COMBINE THEM
		m_info.linkageInfo.linkage = other;
		m_info.linkageInfo.linked = true;
		m_info.linkageInfo.selfType = (m_info.from != nullptr) ? SelfLinkType::SLT_From : SelfLinkType::SLT_To;
		//m_info.linkageInfo.selfType = SelfLinkType::SLT_From;

		other->m_info.linkageInfo.linkage = this;
		other->m_info.linkageInfo.linked = true;
		other->m_info.linkageInfo.selfType = (m_info.linkageInfo.selfType == SelfLinkType::SLT_From) ? SelfLinkType::SLT_To : SelfLinkType::SLT_From;

		VIDEKit::Node *from = nullptr;
		VIDEKit::Node *to = nullptr;

		std::vector<VIDEKit::Connection*> cons;

		if (m_info.linkageInfo.selfType == SLT_From) {
			m_info.linkageInfo.linkagePhysics.anchorTarget = Vector2D::VInterp(m_properties.from, m_info.linkageInfo.linkage->m_properties.to, 0.5f);
		
			from = m_info.from;
			to = m_info.linkageInfo.linkage->getInfo().to;

			cons = from->getProperties().connections;

			m_info.linkageInfo.linkagePhysics.anchorCurrent = m_info.linkageInfo.linkage->getInfo().linkageInfo.linkagePhysics.anchorCurrent;
		}
		else if (m_info.linkageInfo.selfType == SLT_To) {
			m_info.linkageInfo.linkagePhysics.anchorTarget = Vector2D::VInterp(m_properties.to, m_info.linkageInfo.linkage->m_properties.from, 0.5f);

			from = m_info.linkageInfo.linkage->getInfo().from;
			to = m_info.to;

			cons = from->getProperties().connections;

			m_info.linkageInfo.linkagePhysics.anchorCurrent = m_info.linkageInfo.linkage->getInfo().linkageInfo.linkagePhysics.anchorCurrent;
		}

		
		int overlapIndex = 0;

		for (int i = 0; i < cons.size(); i++) {
			
			if (cons[i]->getInfo().linkageInfo.linked) {

				if (cons[i]->getInfo().from == from && cons[i]->getInfo().linkageInfo.linkage->getInfo().to == to) {

					if (overlapIndex % 2 == 0) {
						m_info.linkageInfo.overlapIndex = overlapIndex;
						m_info.linkageInfo.linkage->getInfo().linkageInfo.overlapIndex = overlapIndex;
					}
					else {
						m_info.linkageInfo.overlapIndex = -overlapIndex;
						m_info.linkageInfo.linkage->getInfo().linkageInfo.overlapIndex = -overlapIndex;
					}

					overlapIndex++;
				}
			}
		}

		return true;
	}


	VIDEKit::Connection::ConnectionInfo &VIDEKit::Connection::getInfo() {
		return m_info;
	}

}