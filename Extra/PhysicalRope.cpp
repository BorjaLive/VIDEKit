#include "PhysicalRope.h"

namespace VIDEKit {

	PhysicalRope::PhysicalRope(){
		m_properties = {
			VIDEKit::Vector2D(),			//FROM
			VIDEKit::Vector2D(),			//TO
		};


		m_lineShape = new VIDEKit::Line({
			VIDEKit::Vector2D(),			//BEGIN OF BEZIER CURVE
			VIDEKit::Vector2D(),			//CONTROL POINTs
			VIDEKit::Vector2D(),			
			VIDEKit::Vector2D()				//END OF BEZIER CURVE
		});
	}


	PhysicalRope::~PhysicalRope(){

	}

	void PhysicalRope::tick(bool calcTarget){
		//UPDATE THE ROPE PHYSICS
		
		if (calcTarget) {
			//GET THE DIFFERENCE BETWEEN CURRENT TARGET POINT AND THE CURRENT POINT
			m_physics.anchorTarget = VIDEKit::Vector2D::VInterp(m_properties.from, m_properties.to, 0.5f);
		}

		//UPDATE THE TARGET VELOCITY FOR THE MIDDLE POINT
		m_physics.anchorVelocity += (m_physics.anchorTarget - m_physics.anchorCurrent) * m_physics.friction;

		//UPDATE THE CURRENT MIDDLE POINT WITH THE VELOCITY
		m_physics.anchorCurrent += m_physics.anchorVelocity * m_physics.hardness;

		//UPDATE THE VELOCITY WITH THE DAMPING
		m_physics.anchorVelocity *= m_physics.damping;

		m_lineShape->points = VIDEKit::Line::bezierCurve(
			{
				m_properties.from,
				m_physics.anchorCurrent,
				m_properties.to
			});

		m_lineShape->getTransform()->position = m_properties.from;
	}

	VIDEKit::PhysicalRope::Properties & PhysicalRope::getProperties(){
		return m_properties;
	}

	VIDEKit::PhysicalRope::Physics & PhysicalRope::getPhysics(){
		return m_physics;
	}

	VIDEKit::Line &PhysicalRope::getLine()
	{
		return *m_lineShape;
	}

}