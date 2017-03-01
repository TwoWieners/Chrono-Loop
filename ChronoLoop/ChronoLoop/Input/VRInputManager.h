#pragma once
#include <openvr.h>
#include "Controller.h"
#include "../Common/Math.h"

namespace Epoch {

	enum ControllerType {
		Primary = 0,
		Secondary
	};

	typedef std::pair<ControllerType, vr::ETrackedControllerRole> ControllerMap;

	class VIM {
		Controller mRightController;
		Controller mLeftController;
		matrix4 mPlayerPosition;
		vr::TrackedDevicePose_t mPoses[vr::k_unMaxTrackedDeviceCount];
		vr::IVRSystem* mVRSystem;
		
		VIM(vr::IVRSystem* _vr);
		~VIM();
	
		friend class VRInputManager;
	public:
		void Update();
		Controller& GetController(bool left);
		inline bool IsInitialized() const { return mVRSystem != nullptr; }
		inline matrix4& GetPlayerPosition() { return mPlayerPosition; }
		inline vr::TrackedDevicePose_t* GetTrackedPositions() { return mPoses; }
		inline unsigned int GetTrackedDeviceCount() { return vr::k_unMaxTrackedDeviceCount; }
		matrix4 GetPlayerWorldPos();
	};
	
	class VRInputManager {
	private:
		static VIM* sInstance;
	
		VRInputManager();
		~VRInputManager();
	public:
		static VIM& Instance();
		static VIM& Initialize(vr::IVRSystem* _vr);
		static void Shutdown();
	};

}

namespace std {
	template <>
	class hash<Epoch::ControllerMap> {
	public:
		size_t operator()(const Epoch::ControllerMap& contint) {
			return hash<int>()(contint.first);
		}
	};
}