#pragma once
#include "../Objects/BaseObject.h"
#include "../Common/Logger.h"
#include "CodeComponent.hpp"
#include "../Objects/MeshComponent.h"
#include "../Physics/Physics.h"
#include "../Input/VRInputManager.h"
#include "../Core/Level.h"
#include "../Actions/HeadsetFollow.hpp"

namespace Epoch {

	struct TeleportAction : public CodeComponent {
		MeshComponent *mPlaneMesh;
		BaseObject *mPlaneObject;
		bool left;
		TeleportAction(bool _left) { left = _left; };

		virtual void Start() {
			mPlaneObject = Level::Instance()->iFindObjectWithName("plane");
			mPlaneMesh = (MeshComponent*)mPlaneObject->GetComponentIndexed(eCOMPONENT_MESH, 0);
		}

		virtual void Update() {
			if (!VRInputManager::Instance().IsInitialized()) {
				return;
			}
			// I'm lazy so, let's just set this thing's position to the controller's position.
			matrix4 mat = VRInputManager::Instance().GetController(left).GetPosition();
			mObject->GetTransform().SetMatrix(mat);

			if (VRInputManager::Instance().GetController(left).GetPressDown(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)) {
				vec4f forward(0, 0, 1, 0);
				forward *= mObject->GetTransform().GetMatrix();
				Triangle *tris = mPlaneMesh->GetTriangles();
				size_t numTris = mPlaneMesh->GetTriangleCount();
				vec4f position = (mat * mPlaneObject->GetTransform().GetMatrix().Invert()).Position;
				for (unsigned int i = 0; i < numTris; ++i) {
					float time = 0;
					if (Physics::Instance()->RayToTriangle((tris + i)->Vertex[0], (tris + i)->Vertex[1], (tris + i)->Vertex[2], (tris + i)->Normal, position, forward, time)) {
						forward *= time;
						VRInputManager::Instance().GetPlayerPosition()[3][0] += forward[0]; // x
						VRInputManager::Instance().GetPlayerPosition()[3][2] += forward[2]; // z
						//VRInputManager::Instance().iGetPlayerPosition()[3][3] += forward[3]; // w
					}
				}
			}
		}
	};

}