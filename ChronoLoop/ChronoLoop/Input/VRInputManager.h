#pragma once
#include <openvr.h>
#include "Controller.h"
#include "../Common/Math.h"
#include <utility>

namespace Epoch {

	enum class ControllerType {
		Primary = 0,
		Secondary
	};

	typedef std::pair<ControllerType, Controller> ControllerMap;

	class VIM {
		ControllerMap mPrimaryController;
		ControllerMap mSecondaryController;
		matrix4 mPlayerPosition;
		vr::TrackedDevicePose_t mPoses[vr::k_unMaxTrackedDeviceCount];
		vr::IVRSystem* mVRSystem;
		
		VIM(vr::IVRSystem* _vr);
		~VIM();
	
		friend class VRInputManager;
	public:
		void Update();
		Controller& GetController(ControllerType _t);
		inline unsigned int GetTrackedDeviceCount() { return vr::k_unMaxTrackedDeviceCount; }
		inline vr::TrackedDevicePose_t* GetTrackedPositions() { return mPoses; }
		inline matrix4& GetPlayerPosition() { return mPlayerPosition; }
		inline matrix4 GetPlayerView() { return (matrix4)(mPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking) * mPlayerPosition; }
		inline bool IsVREnabled() const { return mVRSystem != nullptr; }
		inline vr::IVRSystem* GetVRSystem() { return mVRSystem; }
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