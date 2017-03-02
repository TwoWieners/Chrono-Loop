#pragma once
#include "../Objects/BaseObject.h"
#include "CodeComponent.hpp"
#include "../Core/TimeManager.h"
#include "../Common/Logger.h"
#include <unordered_set>

using namespace Epoch;

struct Bootleg {
	struct Node {
		vec2f data;
		Node *mNext, *mPrev;
		Node(vec2f _d) : data(_d) {}
	};
	Node *mHead, *mTail;
	unsigned int mSize, mLimit = 9;

	void AddHead(vec2f _node) {
		if (mHead == nullptr) {
			mHead = new Node(_node);
			mTail = mHead;
			mHead->mNext = mHead->mPrev = nullptr;
			mSize = 1;
		} else {
			if (mSize >= mLimit) {
				Node *tail = mTail->mPrev;
				delete mTail;
				mTail = tail;
				mTail->mNext = nullptr;
				--mSize;
			}
			Node* n = new Node(_node);
			n->mPrev = nullptr;
			n->mNext = mHead;
			mHead->mPrev = n;
			mHead = n;
			mSize++;
		}
	}

	vec2f& operator[](unsigned int _index) {
		Node* n = mHead;
		for (unsigned int i = 0; i < _index; ++i) {
			n = mHead->mNext;
		}
		return n->data;
	}

	~Bootleg() {
		Node* n = mHead;
		while (n) {
			Node* self = n;
			n = n->mNext;
			delete self;
		}
	}
};

struct BoxSnapToControllerAction : public CodeComponent {
	ControllerType mControllerRole = eControllerType_Primary;
	bool mHeld = false;
	Bootleg mBootleg;
	ControllerCollider* mCollider;
	Collider* mPickUp = nullptr;

	virtual void Start() {
		mCollider = (ControllerCollider*)(mObject->GetComponentIndexed(eCOMPONENT_COLLIDER, 0));
	}

	virtual void Update() override {

		if (VRInputManager::GetInstance().IsVREnabled()) {
			Controller &controller = VRInputManager::GetInstance().GetController(mControllerRole);
			if (controller.GetPress(vr::EVRButtonId::k_EButton_SteamVR_Trigger) && !mHeld && !mCollider->mHitting.empty()) {
				SnapToController();
			} else if (controller.GetPress(vr::EVRButtonId::k_EButton_SteamVR_Trigger) && !mHeld && !mCollider->mHitting.empty()) {
				SnapToController();
			} else if (mHeld) {
				ReleaseCube();
			}
			if (controller.GetPressDown((vr::EVRButtonId::k_EButton_Grip))) {
//				TimeManager::Instance()->RewindTimeline();
			} else if (controller.GetPressDown((vr::EVRButtonId::k_EButton_Grip))) {
	//			TimeManager::Instance()->RewindTimeline();
			}

#pragma region Gestures
			//vec2f touch = leftController.GetAxis();
			//mBootleg.AddHead(touch);

			////SystemLogger::GetLog() << "(" << touch.x << "," << touch.y << ")" << std::endl;
			//if (mBootleg.mSize == mBootleg.mLimit) {
			//	// Get initial point, get vector from it's negation (v - (-v)), and then cross it (v.y, -v.x)
			//	vec2f initialPoint = mBootleg[0];
			//	vec2f line = (initialPoint - (-initialPoint));
			//	vec2f counterClockwise = line.Cross().Normalize();

			//	vec2f pointEight = mBootleg[8];
			//	vec2f leg = (pointEight - initialPoint);
			//	vec2f nLeg = leg.Normalize();
			//	if (leg.SquaredMagnitude() >= 0.01f) {
			//		if (nLeg * counterClockwise < 0) {
			//			SystemLogger::GetLog() << "Somewhat Clockwise" << std::endl;
			//		}
			//		if (nLeg * counterClockwise > 0) {
			//			SystemLogger::GetLog() << "Somewhat Counter-Clockwise" << std::endl;
			//		}
			//	}
			//}
			//SystemLogger::GetLog() << "[Debug] Touchpad Axis: (" << touch.x << ", " << touch.y << ")" << std::endl;
#pragma endregion Gestures
		}
		//mObject->GetTransform().SetMatrix(Math::MatrixRotateInPlace(mObject->GetTransform().GetMatrix(), 1, 0, 0, DirectX::XM_PI / 1024.0f));
	}

	virtual void SnapToController() {
		mHeld = true;
		matrix4 m = VRInputManager::GetInstance().GetController(mControllerRole).GetPosition();

		vec4f pos, setPos;
		vec4f controllerPos = mCollider->GetPos();
		bool unset = true;
		for (auto iter = mCollider->mHitting.begin(); iter != mCollider->mHitting.end(); ++iter)
		{
			pos = (*iter)->GetPos();
			if (unset)
			{
				setPos = pos;
				mPickUp = (*iter);
				unset = false;
			}
			else
			{
				float tx = pos.x - mCollider->GetPos().x;
				float ty = pos.y - mCollider->GetPos().y;
				float tz = pos.z - mCollider->GetPos().z;
				float sx = setPos.x - mCollider->GetPos().x;
				float sy = setPos.y - mCollider->GetPos().y;
				float sz = setPos.z - mCollider->GetPos().z;
		
				if (tx < sx || ty < sy || tz < sz)
				{
					setPos = pos;
					mPickUp = (*iter);
				}
			}
		}
		
		if (mPickUp)
		{
			if (!mPickUp->mShouldMove){
				mCollider->mHitting.erase(mPickUp);
					return;
			}
			mPickUp->SetPos((m).tiers[3]);
			//mObject->GetTransform().SetMatrix(m);
			mPickUp->mShouldMove = false;
			mPickUp->mForces = { 0,0,0,0 };
			mPickUp->mVelocity = { 0,0,0,0 };
			mPickUp->mAcceleration = { 0,0,0,0 };
		}
	}

	virtual void ReleaseCube() {
		vec4f force = VRInputManager::GetInstance().GetController(mControllerRole).GetVelocity();
		force[2] *= -1; // SteamVR seems to Assume +Z goes into the screen.
		mPickUp->mVelocity = force;
		mPickUp->mShouldMove = true;
		mHeld = false;
	}
};
