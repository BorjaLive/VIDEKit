#pragma once

#include "Shape/Line.h"

namespace VIDEKit {

	class PhysicalRope
	{
	public:
		struct Properties {
			VIDEKit::Vector2D from;
			VIDEKit::Vector2D to;
		};

		struct Physics {
			VIDEKit::Vector2D anchorTarget;
			VIDEKit::Vector2D anchorCurrent;
			VIDEKit::Vector2D anchorVelocity;
			VIDEKit::Vector2D targetOffset;
			float damping;
			float hardness;
			float friction;

			Physics() {
				anchorTarget = anchorCurrent = anchorVelocity = targetOffset = Vector2D();
				damping = 0.91;
				hardness = 0.03;
				friction = 0.85;
			}
		};

	protected:
		VIDEKit::Line *m_lineShape;
		VIDEKit::PhysicalRope::Properties m_properties;
		VIDEKit::PhysicalRope::Physics m_physics;

	public:
		PhysicalRope();
		~PhysicalRope();

		void tick(bool calcTarget = true);

	public:

		VIDEKit::PhysicalRope::Properties &getProperties();

		VIDEKit::PhysicalRope::Physics &getPhysics();

		VIDEKit::Line &getLine();
	};

}