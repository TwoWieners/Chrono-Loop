#pragma once
#include "Actions/CodeComponent.hpp"
#include "Objects/MeshComponent.h"
#include "Objects/BaseObject.h"
#include "Input/VRInputManager.h"

namespace Epoch {

	class HeadsetFollow : public CodeComponent {
	private:

	public:

		virtual void Update() {
			if (!VRInputManager::Instance().IsInitialized()) {
				return;
			}

			matrix4 mat = matrix4(VRInputManager::Instance().GetTrackedPositions()[0].mDeviceToAbsoluteTracking) * VRInputManager::Instance().GetPlayerPosition();
			mObject->GetTransform().SetMatrix(mat);
		}

	};

}