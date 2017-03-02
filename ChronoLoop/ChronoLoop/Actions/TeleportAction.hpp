#pragma once
#include "../Objects/BaseObject.h"
#include "../Common/Logger.h"
#include "CodeComponent.hpp"
#include "../Objects/MeshComponent.h"
#include "../Physics/Physics.h"
#include "../Input/VRInputManager.h"
#include "../Core/Level.h"
#include "../Actions/HeadsetFollow.hpp"


struct TeleportAction : public CodeComponent {
	MeshComponent *mPlaneMesh, *mWallsMesh, *mBlockMesh, *mExitMesh;
	BaseObject *mPlaneObject, *mWallsObject, *mBlockObject, *mExitObject;
	bool left;
	TeleportAction(bool _left) { left = _left; };

	virtual void Start() { 
		mPlaneObject = Level::Instance()->iFindObjectWithName("plane");
		mWallsObject = Level::Instance()->iFindObjectWithName("walls");
		mBlockObject = Level::Instance()->iFindObjectWithName("BlockDoor");
		mExitObject = Level::Instance()->iFindObjectWithName("ExitWall");
		mPlaneMesh = (MeshComponent*)mPlaneObject->GetComponentIndexed(eCOMPONENT_MESH, 0);
		mWallsMesh = (MeshComponent*)mWallsObject->GetComponentIndexed(eCOMPONENT_MESH, 0);
		mBlockMesh = (MeshComponent*)mBlockObject->GetComponentIndexed(eCOMPONENT_MESH, 0);
		mExitMesh = (MeshComponent*)mExitObject->GetComponentIndexed(eCOMPONENT_MESH, 0);
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
			MeshComponent* meshes[] = { mWallsMesh, mBlockMesh, mExitMesh };
			BaseObject* objects[] = { mWallsObject, mBlockObject, mExitObject };
			float meshTime = 0, wallTime = FLT_MAX;
			for (int i = 0; i < ARRAYSIZE(meshes); ++i) {
				vec4f meshPos = (mat * objects[i]->GetTransform().GetMatrix().Invert()).fourth;
				Triangle *tris = meshes[i]->GetTriangles();
				size_t numTris = meshes[i]->GetTriangleCount();
				for (unsigned int i = 0; i < numTris; ++i) {
					float hitTime = FLT_MAX;
					Physics::Instance()->RayToTriangle((tris + i)->Vertex[0], (tris + i)->Vertex[1], (tris + i)->Vertex[2], (tris + i)->Normal, meshPos, forward, hitTime);
					if (hitTime < wallTime) {
						wallTime = hitTime;
					}
				}
			}

			Triangle *tris = mPlaneMesh->GetTriangles();
			size_t numTris = mPlaneMesh->GetTriangleCount();
			vec4f position = (mat * mPlaneObject->GetTransform().GetMatrix().Invert()).fourth;
			for (unsigned int i = 0; i < numTris; ++i) {
				if (Physics::Instance()->RayToTriangle((tris + i)->Vertex[0], (tris + i)->Vertex[1], (tris + i)->Vertex[2], (tris + i)->Normal, position, forward, meshTime)) {
					if (meshTime < wallTime) {
						forward *= meshTime;
						VRInputManager::Instance().GetPlayerPosition()[3][0] += forward[0]; // x
						VRInputManager::Instance().GetPlayerPosition()[3][2] += forward[2]; // z
						//VRInputManager::Instance().iGetPlayerPosition()[3][3] += forward[3]; // w
					} else {
						SystemLogger::GetLog() << "[DEBUG] Can't let you do that, Starfox." << std::endl;
					}
				}
			}
		}
	}
};
