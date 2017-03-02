#pragma once
#include <vector>
#include <unordered_map>
#include "../Objects/BaseObject.h"
#include <bitset>

namespace Epoch {

	//Componet Structs for storing;
#pragma region ComponetStructs
	struct SnapComponent {
		ComponentType mCompType;
		unsigned short mBitNum;
		unsigned short mId;
	};
	struct SnapComponent_Physics : SnapComponent {
		vec4f mForces;
		vec4f mVel;
		vec4f mAcc;
	};
	//Add more componets when we need it
#pragma endregion ComponetStructs


//Extracted values from BaseObjects and its componets
	struct SnapInfo {
		unsigned short mId;	//unique id of the object
		Transform mTransform;	//positional data of the object
		std::vector<SnapComponent*>mComponets;
		std::bitset<32>mBitset;	//Base object and componets enabled or disabled //0 for the baseobject and 1-31 for the components
	};
	struct SnapInfoPlayer : SnapInfo {

		matrix4 mPlayerWorldPos;
		//Left Controller World Pos
		matrix4 mLCWorldPos;
		//Right Controller World Pos
		matrix4 mRCWorldPos;

		SnapInfoPlayer() {};
		SnapInfoPlayer(matrix4 _playerWorldPos, matrix4 _leftController, matrix4 _rightController) {
			mPlayerWorldPos = _playerWorldPos;
			mLCWorldPos = _leftController;
			mRCWorldPos = _rightController;
		}

	};

	//If you rewind time and there is no snapshot for your object that means nothing changed from the last 
	struct Snapshot {
		unsigned int mTime = -1;									//This is the time in the world the snapshot was taken. If was not assigned mTime= -1;
		std::unordered_map<unsigned short, SnapInfo*> mSnapinfos;
		std::unordered_map<unsigned short, unsigned int> mUpdatedtimes;		//Map of int times of last updated times. Use the float retreaved to access a different snapshot 

		bool IsObjectStored(unsigned short _id);
	};

	struct ObjectLifeTime {
		int mBirth = -1;
		int mDeath = INT32_MAX;
	};

	class Timeline {
		std::vector<unsigned int> mSnaptimes;
		std::unordered_map<unsigned int, Snapshot*> mSnapshots;		//The key will be the time they were taken (mSnapTimes)
		std::unordered_map<unsigned short, BaseObject*> mLiveObjects;
		std::unordered_map<unsigned short, ObjectLifeTime*> mObjectLifeTimes;

	public:
		Timeline();
		~Timeline();
		//Where we are at in the timeline
		int mCurrentGameTimeIndx = 0;
		int GetCurrentGameTimeIndx() { return mCurrentGameTimeIndx; }
		void AddBaseObject(BaseObject* _object, unsigned short _id);						//add to the list of recorded objects.
		void AddPlayerBaseObject(BaseObject* _object, unsigned short _id);						//add to the player of recorded objects and .
		void AddSnapshot(unsigned int _snaptime, Snapshot* _snapshot);
		bool RewindNoClone(unsigned int _snaptime, unsigned short _id1, unsigned short _id2, unsigned short _id3);
		bool RewindMakeClone(unsigned int _snaptime);
		void ChangeBitsetToSnap(SnapInfo* _destinfo, Component* _curComp);
		void MoveObjectToSnap(unsigned int _snaptime, unsigned short _id);
		void MoveAllObjectsToSnap(unsigned int _snaptime);
		void MoveAllObjectsToSnapExceptPlayer(unsigned int _snaptime, unsigned short _id1, unsigned short _id2, unsigned short _id3);
		void ClearTimeLine();
		void SetComponent(SnapComponent* _destComp, BaseObject* _obj, SnapInfo* _destInfo);
		void SetCloneCreationTime(unsigned short _id1, unsigned short _id2, unsigned short _id3);
		void SetCloneDeathTime(unsigned short _id1, unsigned short _id2, unsigned short _id3);
		void SetBaseObjectDeathTime(unsigned short _id);
		SnapInfo* GenerateSnapInfo(BaseObject* _object, SnapInfo* _info);							//Error check agianst the BaseObject* if it is null or not
		Snapshot* GenerateSnapShot(unsigned int _time, std::vector<BaseObject*> & _clones);
		SnapInfoPlayer * GenerateSnapInfoPlayer();
		bool CheckForDuplicateData(unsigned short _id, BaseObject* _object);
	};

}