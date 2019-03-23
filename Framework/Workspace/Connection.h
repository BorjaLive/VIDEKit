#pragma once

#include "../../Extra/PhysicalRope.h"
#include "../../Extra/Shape/Rectangle.h"

namespace VIDEKit {

	class Connection : public VIDEKit::PhysicalRope
	{
	public:

		enum SelectionType {
			ST_FromHandler,
			ST_ToHandler,
			ST_Connection,
			ST_None
		};

		struct HandleProperties {
			float innerRadius;
			float outerRadius;
			VIDEKit::Color accentColor;
		};

		struct DataType {
			std::string typeName;
			VIDEKit::Color typeColor;
			std::string connectionName;
			std::string extra;
		};

		enum SelfLinkType {
			SLT_From,
			SLT_To,
			SLT_None
		};

		struct LinkInfo {
			bool linked;
			VIDEKit::Connection *linkage;
			SelfLinkType selfType;
			VIDEKit::PhysicalRope::Physics linkagePhysics;
			int overlapIndex;
		};

		enum ConnectionType {
			CT_Input,
			CT_Output
		};

		struct ConnectionInfo {
			class Node* from;
			class Node* to;
			VIDEKit::Point2D fromPoint;
			VIDEKit::Point2D toPoint;
			Connection::DataType type;
			Connection::ConnectionType connectionType;

			LinkInfo linkageInfo;

			ConnectionInfo() {
				from = to = nullptr;
				fromPoint = toPoint = Vector2D();
				type = {};
				linkageInfo.linked = false;
				linkageInfo.linkage = nullptr;
				linkageInfo.selfType = SelfLinkType::SLT_None;
			}

			ConnectionInfo(Connection::ConnectionType conn_type, Connection::DataType data_type) {
				connectionType = conn_type;
				type = data_type;
			}
		};

		struct FlowAnimationData {
			std::vector<VIDEKit::Rectangle> points;
			double animationControllerDelta;
			double animationCoordinatorTime;
		};

	private:
		VIDEKit::Rectangle *m_fromHandle;
		VIDEKit::Rectangle *m_toHandle;
		VIDEKit::Connection::HandleProperties m_fromHandleProperties;
		VIDEKit::Connection::HandleProperties m_toHandleProperties;

		VIDEKit::Connection::ConnectionInfo m_info;

		sf::Font m_font;
		sf::Text m_displayText;

		FlowAnimationData flowAnimation;

	public:
		Connection();
		~Connection();

		void render(SFMLRenderWindow *window, bool calcTarget = true);
		void render(SFMLRenderTexture *texture, bool calcTarget = true);

		Vector2D closestPoint(const Vector2D &point);

		SelectionType pointOver(const Vector2D &point, Vector2D *overPoint = nullptr);

		bool link(VIDEKit::Connection *other);

		VIDEKit::Connection::ConnectionInfo &getInfo();
	};

}