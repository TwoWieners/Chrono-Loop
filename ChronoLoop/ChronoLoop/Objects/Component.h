#pragma once
#include "..\Common\Math.h"
#include "../Physics/Physics.h"
#include "../Rendering/Mesh.h"
#include <unordered_map>
#include <unordered_set>

namespace Epoch {

	class BaseObject;
	class Transform;
	//class Mesh;

	enum ComponentType {
		eCOMPONENT_UNKNOWN = 0,
		eCOMPONENT_CODE,
		eCOMPONENT_AUDIOEMITTER,
		eCOMPONENT_AUDIOLISTENER,
		eCOMPONENT_COLLIDER,
		eCOMPONENT_UI,
		eCOMPONENT_MESH,
		eCOMPONENT_MAX
	};

	class Component {
		friend class Physics;
		friend class BaseObject;

		static unsigned short mComponentCount;
		unsigned short mComponentId;	//unique component id
		unsigned short mComponentNum;  //the nth number component of a base object. This is for knowing the position in the bitset

	protected:
		bool mDestroyed = false;
		bool mIsEnabled = true, mIsValid = true;
		ComponentType mType = eCOMPONENT_MAX;
		BaseObject* mObject = nullptr;
	public:
		Component();
		Component(ComponentType _cType);
		virtual ~Component();
		inline ComponentType GetType() { return mType; };
		inline bool IsEnabled() { return mIsEnabled; };
		inline bool IsValid() { return mIsValid; }
		inline void Disable() { mIsEnabled = false; };
		inline void Enable() { mIsEnabled = true; };
		virtual void Update() = 0;
		virtual void Destroy() = 0;
		unsigned short GetComponentNum() { return mComponentNum; }
		void GetMatrix(matrix4& _m);
		unsigned short GetColliderId() { return mComponentId; };
		Transform& GetTransform();
		Transform& GetTransform() const;
	};

	class Listener : public Component {
	public:
		Listener() : Component(ComponentType::eCOMPONENT_AUDIOLISTENER) {}
		void Update() {}
		void Destroy() {}
	};

	class Emitter :public Component {
	public:
		enum sfxTypes { ePlayLoop, ePauseLoop, eResumeLoop, eStopLoop, ePlaySFX };

		Emitter() : Component(ComponentType::eCOMPONENT_AUDIOEMITTER) {
			mIsPaused = mIsPlaying = false;
		}

		void Play(int _id = 0);
		void Pause(int _id = 0);
		void Stop(int _id = 0);
		void PlaySFX(int _id = 0);
		void PlaySFX(int _id = 0, const vec4f* _pos = new vec4f());
		void AddSoundEvent(sfxTypes _type, int64_t _event);

		void Update();
		void Destroy();
	private:
		bool mIsPlaying = false, mIsPaused = false;
		std::unordered_map<sfxTypes, std::vector<int64_t>> mSFX;

	};

	class Collider : public Component {
	public:
		enum ColliderType {
			eCOLLIDER_Mesh,
			eCOLLIDER_Sphere,
			eCOLLIDER_Cube,
			eCOLLIDER_Plane,
			eCOLLIDER_Button,
			eCOLLIDER_Controller
		};

		virtual void Update() {}
		virtual void Destroy() {}

		ColliderType mColliderType;
		bool mShouldMove, mRewind;
		vec4f mVelocity, mAcceleration, mTotalForce, mForces, mImpulsiveForce, mGravity, mWeight, mDragForce;
		float mMass, mElasticity, mKineticFriction, mStaticFriction, mInvMass, mRHO, mDrag, mArea;

		vec4f AddForce(vec4f _force) { mForces = _force; return mForces; };
		virtual vec4f GetPos();
		virtual void SetPos(const vec4f& _newPos);
	};

	class MeshCollider : public Collider {
	public:
		MeshCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, char* _path);
		Mesh* mMesh;
	};

	class SphereCollider : public Collider {
	public:
		SphereCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _radius);
		vec4f mCenter;
		float mRadius;
		virtual void SetPos(const vec4f& _other);
	};

	class CubeCollider : public Collider {
	public:
		CubeCollider() {}
		CubeCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, vec4f _min, vec4f _max);
		vec4f mMin, mMax, mMinOffset, mMaxOffset;
		virtual void SetPos(const vec4f& _newPos);
	};

	class OrientedCubeCollider : public Collider {
	public:
		OrientedCubeCollider() {}
		OrientedCubeCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, vec4f _center, vec4f _xRadius, vec4f _yRadius, vec4f _zRadius, vec4f _xRotation, vec4f _yRotation, vec4f _zRotation);
		vec4f mCenter, mRx, mRy, mRz;
		vec4f mAxis[3];
	};

	class PlaneCollider : public Collider {
	public:
		PlaneCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _offset, vec4f _norm);
		vec4f mNormal, mMin, mMax;
		float mOffset;
	};

	class ButtonCollider : public CubeCollider {
	public:
		ButtonCollider(BaseObject* _obj, vec4f _min, vec4f _max, float _mass, float normForce, vec4f _pushNormal);
		vec4f mPushNormal;
		Plane mUpperBound, mLowerBound;
	};

	class ControllerCollider : public CubeCollider {
	public:
		ControllerCollider(BaseObject* _obj, vec4f _min, vec4f _max, bool _left);
		bool mLeft;
		std::unordered_set<Collider*> mHitting;
	};

	/*
	business entity- gmail, twitter, facebook, steam account
	art, audio, marketing, designer students ?

	first initial last name, password lower case
	gdserv.fullsail.com:8080
	install doc, follow it
	*/

}