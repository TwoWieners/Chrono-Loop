#include "stdafx.h"
#include "VrInputManager.h"
#include "../Common/Logger.h"

namespace Epoch {

	VIM* VRInputManager::sInstance = nullptr;
	
	VIM& VRInputManager::Instance() {
		return *sInstance;
	}
	
	VIM& VRInputManager::Initialize(vr::IVRSystem * _vr) {
		if (nullptr == sInstance) {
			sInstance = new VIM(_vr);
		}
		return *sInstance;
	}
	
	void VRInputManager::Shutdown() {
		if (nullptr != sInstance) {
			delete sInstance;
			sInstance = nullptr;
		}
	}
	
	VRInputManager::VRInputManager() {}
	
	VRInputManager::~VRInputManager() {}
	
	
	VIM::VIM(vr::IVRSystem *_vr) {
		if (nullptr == _vr) {
			SystemLogger::Warn() << "VR Input is disabled." << std::endl;
			return;
		}
		mVRSystem = _vr;
		int rightID = mVRSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		int leftID = mVRSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		SystemLogger::Info() << "Right controller ID: " << rightID << std::endl;
		SystemLogger::Info() << "Left controller ID:  " << leftID << std::endl;
		mPrimaryController.first = ControllerType::Primary;
		mPrimaryController.second = Controller();
		mPrimaryController.second.Setup(rightID);

		mSecondaryController.first = ControllerType::Secondary;
		mSecondaryController.second = Controller();
		mSecondaryController.second.Setup(leftID);
		mPlayerPosition = matrix4::CreateTranslation(2, -1, 8);
	}
	
	VIM::~VIM() {}
	
	void VIM::Update() {
		if (mRightController.GetIndex() < 0) {
			mRightController.mIndex = mVRSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
			if (mRightController.GetIndex() > 0) {
				mRightController.Update();
				SystemLogger::Info() << "Right Controller connected at index " << mRightController.GetIndex() << std::endl;
			}
		} else {
			mRightController.Update();
		}
		if (mLeftController.GetIndex() < 0) {
			mLeftController.mIndex = mVRSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
			if (mLeftController.GetIndex() > 0) {
				mLeftController.Update();
				SystemLogger::Info() << "Left Controller connected at index " << mLeftController.GetIndex() << std::endl;
			}
		} else {
			mLeftController.Update();
		}
		vr::VRCompositor()->WaitGetPoses(mPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	}
	
	
	Controller& VIM::GetController(ControllerType _t) {
		if (left) {
			return mLeftController;
		} else {
			return mRightController;
		}
	}

}