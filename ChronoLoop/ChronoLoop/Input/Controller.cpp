//#include "stdafx.h"
#include "Controller.h"
#include "../Common/Logger.h"
#include "../Core/Level.h"
#include "VRInputManager.h"

namespace Epoch {

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

	Controller::Controller() {}

	void Controller::Update() {
		//update the contoller pose/state when called. 
		mPrevState = mState;
		//SystemLogger::Deug() << "Controller update." << std::endl;
		mValid = VRInputManager::GetInstance().GetVRSystem()->GetControllerStateWithPose(mTrackingSpace, mIndex, &mState, sizeof(mState), &mPose);
		if (mPrevState.ulButtonPressed != mState.ulButtonPressed) {
			UpdateHairTrigger();
		}
	}

	void Controller::UpdateHairTrigger() {
		mHairTriggerPrevState = mHairTriggerState;
		float value = mState.rAxis[vr::k_eControllerAxis_Trigger].x;
		if (mHairTriggerState)
			if (value < mHairTriggerLimit - mHairTriggerDZ || value <= 0.0f)
				mHairTriggerState = false;
			else
				if (value > mHairTriggerLimit + mHairTriggerDZ || value >= 1.0f)
					mHairTriggerState = true;

		mHairTriggerLimit = mHairTriggerState ? MAX(mHairTriggerLimit, value) : MIN(mHairTriggerLimit, value);
	}
	int Controller::CheckGesture() {
		if ((Level::Instance()->iGetLeftTimeManinpulator()->isTimePaused()) || (Level::Instance()->iGetRightTimeManinpulator()->isTimePaused())) {

			vec2f touch = this->GetAxis();
			if ((powf((touch.x), 2) + powf((touch.y), 2)) < 0.25f) //0.25f = 0.5f ^ 2
			{
				//SystemLogger::GetLog() << "Inside the Circle" << std::endl;

				InitialPos.x = -2;
			} else {
				//SystemLogger::GetLog() << "Outside the Circle" << std::endl;

				if (InitialPos.x == -2) {
					//SystemLogger::GetLog() << "First Touch Recognized" << std::endl;
					InitialPos = touch;
					return 0;
				}
				gestureCnt++;
				if (gestureCnt == 5) {
					gestureCnt = 0;
					//SystemLogger::GetLog() << "InitialPos: (" << InitialPos.x << "," << InitialPos.y << ")" << "CurPos: (" << CurPos.x << "," << CurPos.y << ")" << std::endl;
					vec2f CurPos = touch;
					vec2f line = InitialPos.Cross();
					vec2f diff = (CurPos - InitialPos);

					if (diff.x >= 0.05f || diff.y >= 0.05f || diff.x <= -0.05f || diff.y <= -0.05f) {
						float slope = (CurPos.y - InitialPos.y) / (CurPos.x - InitialPos.x);
						if ((slope >= 4 || slope <= -4) &&
							(CurPos.y > 0.5f || CurPos.y < -0.5f) &&
							(CurPos.x < 0.3f && CurPos.x > -0.3f)) {
							SystemLogger::GetLog() << "Vertical Wrongness" << std::endl;
							return 0;
						}
						if ((slope <= 0.25f && slope >= -0.25f) &&
							(CurPos.x > 0.5f || CurPos.x < -0.5f) &&
							(CurPos.y < 0.3f && CurPos.y > -0.3f)) {
							SystemLogger::GetLog() << "Horizontal Wrongness" << std::endl;
							return 0;
						}
						InitialPos = CurPos;
						if (diff * line > 0) {
							SystemLogger::GetLog() << "Somewhat Clockwise" << std::endl;
							return 1;
						} else if (diff * line < 0) {
							SystemLogger::GetLog() << "Somewhat Counter-Clockwise" << std::endl;
							return -1;
						}
					}

				}
			}
		}
		return 0;
	}

	vec2f Controller::GetAxis(vr::EVRButtonId buttonId) {
		if (!GetValid()) {
			return vec2f(0, 0);
		}
		int axisId = (int)buttonId - (int)vr::k_EButton_Axis0;
		return vec2f(mState.rAxis[axisId].x, mState.rAxis[axisId].y);
	}

	void Controller::TriggerHapticPulse(int duration_micro_sec, vr::EVRButtonId buttonId) {
		if (GetValid()) {
			int axisId = (int)buttonId - (int)vr::k_EButton_Axis0;
			VRInputManager::GetInstance().GetVRSystem()->TriggerHapticPulse(mIndex, axisId, (char)duration_micro_sec);
		}
	}

#pragma region Private Functions

	void Controller::Setup(int _index)
	{
		mIndex = _index;
	}

#pragma endregion Private Functions

#pragma region Public Functions

	matrix4 Controller::GetPosition() {
		if (GetValid()) {
			return matrix4(VRInputManager::GetInstance().GetTrackedPositions()[GetIndex()].mDeviceToAbsoluteTracking) * VRInputManager::GetInstance().GetPlayerPosition();
		} else {
			return matrix4();
		}
	}

	vec3f Controller::GetVelocity() {
		return vec3f(mPose.vVelocity.v[0], mPose.vVelocity.v[1], -mPose.vVelocity.v[2]);
	}

	vec3f Controller::GetAngularVelocity() {
		return vec3f(-mPose.vAngularVelocity.v[0], -mPose.vAngularVelocity.v[1], mPose.vAngularVelocity.v[2]);
	}

	vr::VRControllerState_t Controller::GetState() {
		return mState;
	}

	vr::VRControllerState_t	Controller::GetPrevState() {
		return mPrevState;
	}

	vr::TrackedDevicePose_t	Controller::GetPose() {
		return mPose;
	}

	int Controller::GetIndex() {
		return mIndex;
	}

	bool Controller::GetValid() {
		return mValid;
	}

	bool Controller::GetPress(vr::EVRButtonId _id) {
		if (!GetValid()) {
			return false;
		}
		return (mState.ulButtonPressed & vr::ButtonMaskFromId(_id)) != 0;
	}

	bool Controller::GetPressDown(vr::EVRButtonId _id) {
		if (!GetValid()) {
			return false;
		}
		return (mState.ulButtonPressed & vr::ButtonMaskFromId(_id)) != 0 &&
			(mPrevState.ulButtonPressed & vr::ButtonMaskFromId(_id)) == 0;
	}

	bool Controller::GetPressUp(vr::EVRButtonId _id) {
		if (!GetValid()) {
			return false;
		}
		return (mState.ulButtonPressed & vr::ButtonMaskFromId(_id)) == 0 &&
			(mPrevState.ulButtonPressed & vr::ButtonMaskFromId(_id)) != 0;
	}

	bool Controller::GetTouch(vr::EVRButtonId _id) {
		if (!GetValid()) {
			return false;
		}
		return (mState.ulButtonTouched & vr::ButtonMaskFromId(_id)) != 0;
	}

	bool Controller::GetTouchDown(vr::EVRButtonId _id) {
		if (!GetValid()) {
			return false;
		}
		return (mState.ulButtonTouched & vr::ButtonMaskFromId(_id)) != 0 &&
			(mPrevState.ulButtonTouched & vr::ButtonMaskFromId(_id)) == 0;
	}

	bool Controller::GetTouchUp(vr::EVRButtonId _id) {
		if (!GetValid()) {
			return false;
		}
		return (mState.ulButtonTouched & vr::ButtonMaskFromId(_id)) == 0 &&
			(mPrevState.ulButtonTouched & vr::ButtonMaskFromId(_id)) != 0;
	}

	bool Controller::GetHairTrigger() {
		if (!GetValid()) {
			return false;
		}
		return mHairTriggerState;
	}

	bool Controller::GetHairTriggerDown() {
		if (!GetValid()) {
			return false;
		}
		return mHairTriggerState && !mHairTriggerPrevState;
	}

	bool Controller::GetHairTriggerUp() {
		if (!GetValid()) {
			return false;
		}
		return !mHairTriggerState && mHairTriggerPrevState;
	}

#pragma endregion Public Functions

}