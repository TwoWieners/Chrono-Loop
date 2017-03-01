//#include "stdafx.h"
#include "Component.h"
#include "BaseObject.h"
#include "..\Messager\Messager.h"

namespace Epoch {

	// 0 is reserved for the player.
	unsigned short Component::mComponentCount = 0;

	Component::Component() {
		mComponentId = Component::mComponentCount++;
	}
	Component::Component(ComponentType _cType) {
		mType = _cType;
	}
	Component::~Component() {

	}
	//Base Component---------------------------------------------
#pragma region Base Component
	void Component::GetMatrix(matrix4& _m) {
		//_m = object->transform.m_matrix4;
	}
	Transform& Component::GetTransform() { return mObject->GetTransform(); }
	Transform& Component::GetTransform() const { return mObject->GetTransform(); }

#pragma endregion
	//------------------------------------------------------------
	//Listener----------------------------------------------------
#pragma region Listener

//void Listener::Update(){}











#pragma endregion
//------------------------------------------------------------
//Emitter-----------------------------------------------------
#pragma region Emitter

	void Emitter::Update() {

	}

	void Emitter::Play(int _id) {
		if (_id < 0 || _id > mSFX[ePlayLoop].size() - 1)
			return;
		if (mIsPlaying)
			return;
		m_Event* evnt = new m_Event(mSFX[ePlayLoop][_id], this);
		Message* msg = new Message(msgTypes::mSound, soundMsg::MAKEEVENT_Event, 0, false, (void*)evnt);
		Messager::Instance().SendInMessage(msg);
		mIsPlaying = true;
	}

	void Emitter::Pause(int _id) {
		if (_id < 0 || _id > mSFX[ePauseLoop].size() - 1 || mSFX[eResumeLoop].size() - 1)
			return;

		if (mIsPaused) {
			m_Event* evnt = new m_Event(mSFX[eResumeLoop][_id], this);
			Message* msg = new Message(msgTypes::mSound, soundMsg::MAKEEVENT_Event, 0, false, (void*)evnt);
			Messager::Instance().SendInMessage(msg);
			mIsPaused = false;
		} else {
			m_Event* evnt = new m_Event(mSFX[ePauseLoop][_id], this);
			Message* msg = new Message(msgTypes::mSound, soundMsg::MAKEEVENT_Event, 0, false, (void*)evnt);
			Messager::Instance().SendInMessage(msg);
			mIsPaused = true;
		}
	}

	void Emitter::Stop(int _id) {
		if (_id < 0 || _id > mSFX[eStopLoop].size() - 1)
			return;
		if (!mIsPlaying)
			return;
		m_Event* evnt = new m_Event(mSFX[eStopLoop][_id], this);
		Message* msg = new Message(msgTypes::mSound, soundMsg::MAKEEVENT_Event, 0, false, (void*)evnt);
		Messager::Instance().SendInMessage(msg);
		mIsPlaying = false;

	}

	void Emitter::PlaySFX(int _id) {
		if (_id < 0 || _id > mSFX[ePlaySFX].size() - 1)
			return;

		const vec4f * pos = GetTransform().GetPosition();
		m_LocEvent* evnt = new m_LocEvent(mSFX[ePlaySFX][_id], pos);
		Message* msg = new Message(msgTypes::mSound, soundMsg::MAKEEVENT_Loc, 0, false, (void*)evnt);
		Messager::Instance().SendInMessage(msg);
	}
	void Emitter::PlaySFX(int _id, const vec4f* _pos) {
		if (_id < 0 || _id > mSFX[ePlaySFX].size() - 1)
			return;

		const vec4f * pos = _pos;
		m_LocEvent* evnt = new m_LocEvent(mSFX[ePlaySFX][_id], pos);
		Message* msg = new Message(msgTypes::mSound, soundMsg::MAKEEVENT_Loc, 0, false, (void*)evnt);
		Messager::Instance().SendInMessage(msg);
	}


	void Emitter::AddSoundEvent(sfxTypes _type, int64_t _event) {
		switch (_type) {
			case sfxTypes::ePlayLoop:
			{
				mSFX[_type].push_back(_event);
			}
			break;
			case sfxTypes::ePauseLoop:
			{
				mSFX[_type].push_back(_event);
			}
			break;
			case sfxTypes::eResumeLoop:
			{
				mSFX[_type].push_back(_event);
			}
			break;
			case sfxTypes::eStopLoop:
			{
				mSFX[_type].push_back(_event);
			}
			break;
			case sfxTypes::ePlaySFX:
			{
				mSFX[_type].push_back(_event);
			}
			break;

		}
	}


	void Emitter::Destroy() {

	}
#pragma endregion
	//------------------------------------------------------------

	void Collider::Update() {}

	void Collider::Destroy() {}

	vec4f Collider::GetPos() {
		return mObject->GetTransform().GetMatrix().fourth;
	}

	void Collider::SetPos(const vec4f& _newPos) {
		mObject->GetTransform().GetMatrix().fourth = _newPos;
	}

	MeshCollider::MeshCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, char * _path) {
		mObject = _obj;
		mType = eCOMPONENT_COLLIDER;
		mColliderType = eCOLLIDER_Mesh;
		mGravity = _gravity;
		mVelocity = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mAcceleration = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mTotalForce = mGravity;
		mImpulsiveForce = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mShouldMove = _move;
		mMass = _mass;
		mDrag = _drag;
		if (mMass == 0)
			mInvMass = 0;
		else
			mInvMass = 1 / mMass;
		mWeight = mGravity * mMass;
		mRewind = false;
		mElasticity = _elasticity;
		mStaticFriction = _staticFriction;
		mKineticFriction = _kineticFriction;
		mMesh = &Mesh(_path);
	}

	SphereCollider::SphereCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _radius) {
		mObject = _obj;
		mType = eCOMPONENT_COLLIDER;
		mColliderType = eCOLLIDER_Sphere;
		mGravity = _gravity;
		mVelocity = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mAcceleration = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mTotalForce = mGravity;
		mImpulsiveForce = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mShouldMove = _move;
		mRewind = false;
		mMass = _mass;
		if (mMass == 0)
			mInvMass = 0;
		else
			mInvMass = 1 / mMass;
		mWeight = mGravity * mMass;
		mElasticity = _elasticity;
		mStaticFriction = _staticFriction;
		mKineticFriction = _kineticFriction;
		mRHO = 1;
		mDrag = _drag;
		mCenter = GetPos();
		mRadius = _radius;
		mArea = 4 * DirectX::XM_PI * powf(mRadius, 2.0f);
		mDragForce = mVelocity * (-0.5f * mRHO * mVelocity.Magnitude3() * mDrag * mArea);
	}

	void SphereCollider::SetPos(const vec4f& _other) {
		mObject->GetTransform().GetMatrix().fourth = _other;
		mCenter = _other;
	}

	CubeCollider::CubeCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, vec4f _min, vec4f _max) {
		mObject = _obj;
		mType = eCOMPONENT_COLLIDER;
		mColliderType = eCOLLIDER_Cube;
		mGravity = _gravity;
		mVelocity = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mAcceleration = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mTotalForce = mGravity;
		mImpulsiveForce = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mShouldMove = _move;
		mRewind = false;
		mMass = _mass;
		if (mMass == 0)
			mInvMass = 0;
		else
			mInvMass = 1 / mMass;
		mWeight = mGravity * mMass;
		mElasticity = _elasticity;
		mStaticFriction = _staticFriction;
		mKineticFriction = _kineticFriction;
		mRHO = 1;
		mDrag = _drag;
		mMinOffset = _min;
		mMin = _min + mObject->GetTransform().GetMatrix().fourth;
		mMaxOffset = _max;
		mMax = _max + mObject->GetTransform().GetMatrix().fourth;
		float a = (mMaxOffset - mMinOffset) * vec4f(0, 0, 1, 0);
		float b = (mMaxOffset - mMinOffset) * vec4f(0, 1, 0, 0);
		float c = (mMaxOffset - mMinOffset) * vec4f(1, 0, 0, 0);
		mArea = (2 * (a * b)) + (2 * (b * c)) + (2 * (a * c));
		mDragForce = mVelocity * (-0.5f * mRHO * mVelocity.Magnitude3() * mDrag * mArea);
	}

	void CubeCollider::SetPos(const vec4f& _newPos) {
		mObject->GetTransform().GetMatrix().fourth = _newPos;
		mMin = mMinOffset + _newPos;
		mMax = mMaxOffset + _newPos;
	}

	OrientedCubeCollider::OrientedCubeCollider(BaseObject * _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, vec4f _center, vec4f _xRadius, vec4f _yRadius, vec4f _zRadius, vec4f _xRotation, vec4f _yRotation, vec4f _zRotation) {

	}

	PlaneCollider::PlaneCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _offset, vec4f _norm) {
		mObject = _obj;
		mType = eCOMPONENT_COLLIDER;
		mColliderType = eCOLLIDER_Plane;
		mGravity = _gravity;
		mVelocity = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mAcceleration = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mTotalForce = mGravity;
		mImpulsiveForce = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mShouldMove = _move;
		mRewind = false;
		mMass = _mass;
		if (mMass == 0)
			mInvMass = 0;
		else
			mInvMass = 1 / mMass;
		mWeight = mGravity * mMass;
		mElasticity = _elasticity;
		mStaticFriction = _staticFriction;
		mKineticFriction = _kineticFriction;
		mRHO = 1;
		mDrag = _drag;
		mOffset = _offset;
		mNormal = _norm;
		//mArea = ;
		//mDragForce = mVelocity * (-0.5f * mRHO * mVelocity.Magnitude3() * mDrag * mArea);
	}

	ButtonCollider::ButtonCollider(BaseObject* _obj, vec4f _min, vec4f _max, float _mass, float _normForce, vec4f _pushNormal) {
		mObject = _obj;
		mType = eCOMPONENT_COLLIDER;
		mColliderType = eCOLLIDER_Button;
		mGravity = _pushNormal * _normForce;
		mVelocity = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mAcceleration = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mTotalForce = mGravity * _mass;
		mImpulsiveForce = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mShouldMove = true;
		mRewind = false;
		mMass = _mass;
		if (mMass == 0)
			mInvMass = 0;
		else
			mInvMass = 1 / mMass;
		mWeight = mGravity * mMass;
		mElasticity = 0;
		mMinOffset = _min;
		mMin = _min + mObject->GetTransform().GetMatrix().fourth;
		mMaxOffset = _max;
		mMax = _max + mObject->GetTransform().GetMatrix().fourth;
		mPushNormal = _pushNormal;
		mUpperBound.mNormal = mPushNormal;
		mUpperBound.mOffset = mMax * mPushNormal;
		mLowerBound.mNormal = mPushNormal;
		mLowerBound.mOffset = (mMin - mMax) * mPushNormal * 2;
	}

	ControllerCollider::ControllerCollider(BaseObject* _obj, vec4f _min, vec4f _max, bool _left) {
		mObject = _obj;
		mLeft = _left;
		mMass = 2;
		mForces = vec4f(0, -1, 0, 0);
		mGravity = vec4f(0, -2.0f, 0, 0);
		mType = eCOMPONENT_COLLIDER;
		mColliderType = eCOLLIDER_Controller;
		mTotalForce = { 0,-2,0,0 };
		mAcceleration = { 0,-2,0,0 };
		mShouldMove = false;
		mRewind = false;
		mMinOffset = _min;
		mMin = _min + mObject->GetTransform().GetMatrix().fourth;
		mMaxOffset = _max;
		mMax = _max + mObject->GetTransform().GetMatrix().fourth;
	}

}