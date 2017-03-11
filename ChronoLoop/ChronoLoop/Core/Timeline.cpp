//#include "stdafx.h"*
#include "Timeline.h"
#include "../Objects/Component.h"
#include "../Input/VrInputManager.h"
#include "../Rendering/Renderer.h"
#include "../Objects/MeshComponent.h"
#include "../Common/Logger.h"
#include "Pool.h"

namespace Epoch {
	bool Snapshot::IsObjectStored(unsigned short _id) {
		try {
			mSnapinfos.at(_id);
		} catch (std::exception e) {
			return false;
		}

		return true;
	}


	Timeline::Timeline() {

	}


	Timeline::~Timeline() {
		ClearTimeLine();
	}

	void Timeline::AddBaseObject(BaseObject* _object, unsigned short _id) {
		mLiveObjects[_id] = _object;
		ObjectLifeTime* newObject = new ObjectLifeTime();
		if (mSnaptimes.size() != 0)
			newObject->mBirth = mSnaptimes[mCurrentGameTimeIndx];
		else
			newObject->mBirth = 0;

		mObjectLifeTimes[_id] = newObject;

	}
	void Timeline::UpdatePlayerBaseObject(BaseObject* _object, unsigned short _id) {
		mLiveObjects[_id] = _object;
	}


	void Timeline::AddSnapshot(unsigned int _snaptime, Snapshot* _snapshot) {
		mSnapshots[_snaptime] = _snapshot;
	}

	bool Timeline::RewindNoClone(unsigned int _snaptime, unsigned short _id1, unsigned short _id2, unsigned short _id3) {
		if (_snaptime < 0 || _snaptime > mSnaptimes.size() - 1)
			return false;

		mCurrentGameTimeIndx = _snaptime;
		MoveAllObjectsToSnapExceptPlayer(_snaptime, _id1, _id2, _id3);
		//Set interpolators back and turn off;
		//std::vector<BaseObject*>clones = TimeManager::Instance()->GetClonesVec();
		//for (unsigned int i = 0; i < clones.size(); i++) {
		//	UpdateCloneInterpolators(clones[i]->GetUniqueId(), nullptr, _snaptime);
		//}
		//CheckforLostObjects(TimeManager::Instance()->GetClonesVec());
		return true;
	}

	void Timeline::HotFixResetLevel()
	{
		std::vector<BaseObject*>clones = TimeManager::Instance()->GetClonesVec();
		for(auto lifespan: mObjectLifeTimes)
		{
			//Delete the creation of the clones
			//Clones should only have been born after 0
			if (lifespan.second && lifespan.second->mBirth > 0)
				delete lifespan.second;
		}

		for (auto objects = mLiveObjects.begin(); objects != mLiveObjects.end(); ++objects) {
			for (unsigned int i = 0; i < clones.size(); ++i)
			{
				if ((*objects).second && (*objects).second->GetUniqueID() == clones[i]->GetUniqueID()) {
					auto tempIter = (*objects);
					++objects;
					mLiveObjects.erase(tempIter.first);
				}
			}
		}
	
	}

	bool Timeline::RewindMakeClone(unsigned int _snaptime) {
		if (_snaptime < 0 || _snaptime > mSnaptimes.size() - 1)
			return false;
		mCurrentGameTimeIndx = _snaptime;
		MoveAllObjectsToSnap(_snaptime);
		//Set interpolators back and turn off
	//std::vector<BaseObject*>clones = TimeManager::Instance()->GetClonesVec();
	//for (unsigned int i = 0; i < clones.size(); i++) {
	//	UpdateCloneInterpolators(clones[i]->GetUniqueId(), nullptr, _snaptime);
	//}
		//CheckforLostObjects(TimeManager::Instance()->GetClonesVec());
		return true;
	}

	void Timeline::ChangeBitsetToSnap(SnapInfo * _destinfo, Component* _curComp) {
		unsigned short bitnum = _curComp->GetComponentIndex();

		//Object is active
		if (_destinfo->mBitset[0]) {
			//If the object is active
			if (_curComp->GetType() != eCOMPONENT_MESH) {
				if (_destinfo->mBitset[bitnum])
					_curComp->Enable();

				else
					_curComp->Disable();
			}
			//The comp is a mesh
			else {
				//if (_destinfo->mBitset[bitnum])
					//((MeshComponent*)_curComp)->SetVisible(true);

				//else
				//	((MeshComponent*)_curComp)->SetVisible(false);
			}
		}

		//Object is inactive
		else {
			if (_curComp->GetType() != eCOMPONENT_MESH) {
				_curComp->Disable();
			}
			//The comp is a mesh
			else {
				//((MeshComponent*)_curComp)->SetVisible(false);
			}
		}
	}

	void Timeline::SetComponent(SnapComponent* _destComp, BaseObject * _obj, SnapInfo* _destInfo) {
		switch (_destComp->mCompType) {
			//For each of the collider in the vec
		case ComponentType::eCOMPONENT_COLLIDER:
		{
			//Loop to find the same collider component
			for (unsigned int j = 0; j < _obj->GetComponentCount(eCOMPONENT_COLLIDER); j++) {
				Component* currComp = _obj->GetComponentIndexed(eCOMPONENT_COLLIDER, j);
				if (currComp->GetColliderId() == _destComp->mId) {
					((Collider*)currComp)->mRewind = true;
					//((Collider*)currComp)->mShouldMove = false;
					((Collider*)currComp)->mAcceleration = ((SnapComponent_Physics*)_destComp)->mAcc;
					((Collider*)currComp)->mVelocity = ((SnapComponent_Physics*)_destComp)->mVel;
					((Collider*)currComp)->AddForce(((SnapComponent_Physics*)_destComp)->mForces);
					((Collider*)currComp)->SetPos(*_destInfo->mTransform.GetPosition());

					//Set the bitset
					ChangeBitsetToSnap(_destInfo, currComp);
				}
			}
			break;
		}
		default:
		{
			for (unsigned int j = 0; j < _obj->GetComponentCount(_destComp->mCompType); j++) {
				Component* currComp = _obj->GetComponentIndexed(_destComp->mCompType, j);
				if (currComp->GetColliderId() == _destComp->mId) {
					//Set the bitset
					ChangeBitsetToSnap(_destInfo, currComp);
				}
			}
			break;
		}
		}
	}

	void Timeline::UpdateCloneInterpolators(unsigned short _cloneid, SnapInfo* _currSnap, unsigned int _currTime) {
		Snapshot* nextsnap;
		SnapInfo* nextInfo;
		Interpolator<matrix4>* cloneInterp = TimeManager::Instance()->GetCloneInterpolator(_cloneid);

		if (mObjectLifeTimes.find(_cloneid) != mObjectLifeTimes.end() && (mObjectLifeTimes[_cloneid]->mBirth > mCurrentGameTimeIndx || mObjectLifeTimes[_cloneid]->mDeath < mCurrentGameTimeIndx))
			cloneInterp->SetActive(false);

		cloneInterp->SetType(InterpolatorType::I_Matrix4);
		if (_currTime + 1 <= mSnaptimes.size() - 1) {
			nextsnap = mSnapshots[mSnaptimes[_currTime + 1]];
			//found a clone snapinfo in the next snapshot
			if (nextsnap->mSnapinfos.find(_cloneid) != nextsnap->mSnapinfos.end()) {
				nextInfo = nextsnap->mSnapinfos[_cloneid];
				cloneInterp->SetStart(_currSnap->mTransform.GetMatrix());
				cloneInterp->SetEnd(nextInfo->mTransform.GetMatrix());
				cloneInterp->SetActive(true);
				//Loop to find the same clone's baseObject
				std::vector<BaseObject*> clones = TimeManager::Instance()->GetClonesVec();
				for (int i = 0; i < clones.size(); ++i)
				{
					if (_cloneid == clones[i]->GetUniqueID()) {
						cloneInterp->SetEdit(clones[i]->GetTransform().GetMatrix());
						break;
					}
				}
			} else {
				cloneInterp->SetActive(false);
			}
		} else {
			cloneInterp->SetActive(false);
		}
	}

	void Timeline::SetCloneCreationTime(unsigned short _id1, unsigned short _id2, unsigned short _id3) {
		ObjectLifeTime* newObject = new ObjectLifeTime();
		newObject->mBirth = mSnaptimes[mCurrentGameTimeIndx];
		mObjectLifeTimes[_id1] = newObject;

		ObjectLifeTime* newObject1 = new ObjectLifeTime();
		newObject1->mBirth = mSnaptimes[mCurrentGameTimeIndx];
		mObjectLifeTimes[_id2] = newObject1;

		ObjectLifeTime* newObject2 = new ObjectLifeTime();
		newObject2->mBirth = mSnaptimes[mCurrentGameTimeIndx];
		mObjectLifeTimes[_id3] = newObject2;

	}

	void Timeline::SetCloneDeathTime(unsigned short _id1, unsigned short _id2, unsigned short _id3) {
		if (mObjectLifeTimes.find(_id1) != mObjectLifeTimes.end()) {
			ObjectLifeTime* newObject = mObjectLifeTimes[_id1];
			newObject->mDeath = mSnaptimes[mCurrentGameTimeIndx];
			//TODO PAT: Record 1 more snap with this component in it and record death of its componets
		}

		if (mObjectLifeTimes.find(_id2) != mObjectLifeTimes.end()) {
			ObjectLifeTime* newObject1 = mObjectLifeTimes[_id2];
			newObject1->mDeath = mSnaptimes[mCurrentGameTimeIndx];
		}

		if (mObjectLifeTimes.find(_id3) != mObjectLifeTimes.end()) {
			ObjectLifeTime* newObject2 = mObjectLifeTimes[_id3];
			newObject2->mDeath = mSnaptimes[mCurrentGameTimeIndx];
		}
	}

	void Timeline::SetBaseObjectDeathTime(unsigned short _id) {
		if (mObjectLifeTimes.find(_id) != mObjectLifeTimes.end()) {
			ObjectLifeTime* newObject = mObjectLifeTimes[_id];
			newObject->mDeath = mSnaptimes[mCurrentGameTimeIndx];
		}
	}

	void Timeline::CheckforLostObjects(std::vector<BaseObject*>& mClones) {
		for (auto obj : mObjectLifeTimes) {
			if (obj.second->mBirth > mCurrentGameTimeIndx) {
				for (unsigned int i = 0; i < mClones.size(); i++) {
					if (mClones[i]->GetUniqueID() != obj.first && i == mClones.size() - 1) {
						//delete the birth and death struct
						delete obj.second;
						//TODO PAT: ADD A FUNC TO  REMOVE AN OBJECT IN PHYSICS
						//Physics::mInstance->
						//add it back to the pool
						Pool::Instance()->iAddObject(mLiveObjects[obj.first]);
						mObjectLifeTimes.erase(obj.first);
						mLiveObjects.erase(obj.first);
					}
				}
			}
		}
	}

	ObjectLifeTime * Timeline::GetObjectLifetime(unsigned short _id) {
		if (mObjectLifeTimes.find(_id) != mObjectLifeTimes.end())
			return mObjectLifeTimes[_id];

		return nullptr;
	}

	void Timeline::MoveObjectToSnap(unsigned int _snaptime, unsigned short _id) {

		Snapshot* destination = mSnapshots[_snaptime];
		SnapInfo* destInfo;
		//If the object doesnt have a info, then check against the list for the last snap it was updated
		bool stored = destination->IsObjectStored(_id);
		if (stored) {
			destInfo = destination->mSnapinfos[_id];
		} else if (!stored) {
			if (destination->mUpdatedtimes.find(_id) == destination->mUpdatedtimes.end())
				return;
			destInfo = mSnapshots[destination->mUpdatedtimes[_id]]->mSnapinfos[_id];
		}

		//Set Object data
		BaseObject* baseobject = mLiveObjects[_id];
		baseobject->SetTransform(destInfo->mTransform);

		//Set all componets to time recorded
		for (unsigned int i = 0; i < destInfo->mComponets.size(); i++) {
			SnapComponent* destComp = destInfo->mComponets[i];
			SetComponent(destComp, baseobject, destInfo);
		}

	}

	void Timeline::MoveAllObjectsToSnap(unsigned int _snaptime) {
		Snapshot* destination = mSnapshots[_snaptime];
		for (auto object : mLiveObjects) {
			unsigned short id = object.second->GetUniqueID();
			SnapInfo* destInfo;
			//If the object doesnt have a info, then check against the list for the last snap it was updated
			bool stored = destination->IsObjectStored(id);
			if (stored) {
				destInfo = destination->mSnapinfos[id];
			} else if (!stored) {
				if (destination->mUpdatedtimes.find(id) == destination->mUpdatedtimes.end())
					continue;
				destInfo = mSnapshots[destination->mUpdatedtimes[id]]->mSnapinfos[id];
			}

			//Set Object data
			BaseObject* baseobject = object.second;
			if (baseobject)
				baseobject->SetTransform(destInfo->mTransform);

			//Set all componets back to time recorded
			for (unsigned int i = 0; i < destInfo->mComponets.size(); i++) {
				SnapComponent* destComp = destInfo->mComponets[i];
				SetComponent(destComp, baseobject, destInfo);
			}
		}
	}

	void Timeline::MoveAllObjectsToSnapExceptPlayer(unsigned int _snaptime, unsigned short _id1, unsigned short _id2, unsigned short _id3) {
		Snapshot* destination = mSnapshots[_snaptime];
		for (auto object : mLiveObjects) {
			unsigned short id = object.second->GetUniqueID();
			if (id == _id1 || id == _id2 || id == _id3)
				continue;
			SnapInfo* destInfo;
			//If the object doesnt have a info, then check against the list for the last snap it was updated
			bool stored = destination->IsObjectStored(id);
			if (stored) {
				destInfo = destination->mSnapinfos[id];
			} else if (!stored) {
				if (destination->mUpdatedtimes.find(id) == destination->mUpdatedtimes.end())
					continue;
				destInfo = mSnapshots[destination->mUpdatedtimes[id]]->mSnapinfos[id];
			}
			//Set Object data
			BaseObject* baseobject = object.second;
			baseobject->SetTransform(destInfo->mTransform);

			//Set all componets back to time recorded
			for (unsigned int i = 0; i < destInfo->mComponets.size(); i++) {
				SnapComponent* destComp = destInfo->mComponets[i];
				SetComponent(destComp, baseobject, destInfo);
			}
		}
	}


	void Timeline::ClearTimeLine() {
		for (auto snapshot : mSnapshots) {
			for (auto snapInfo : snapshot.second->mSnapinfos) {
				if (snapInfo.second)
					for (auto snapComps : snapInfo.second->mComponets)
						delete snapComps;

				if (snapInfo.second)
					snapInfo.second->mComponets.clear();

				if (snapInfo.second)
					delete snapInfo.second;

			}
			snapshot.second->mSnapinfos.clear();
			snapshot.second->mUpdatedtimes.clear();
			if (snapshot.second)
				delete snapshot.second;
		}

		for (auto Objectlife : mObjectLifeTimes) {
			if (Objectlife.second)
				delete Objectlife.second;
		}
		mObjectLifeTimes.clear();
		mSnapshots.clear();
		mSnaptimes.clear();
		mLiveObjects.clear();
	}


	SnapInfo* Timeline::GenerateSnapInfo(BaseObject* _object, SnapInfo* _info) {
		if (_info == nullptr)
			_info = new SnapInfo();
		_info->mId = _object->GetUniqueID();
		_info->mTransform = _object->GetTransform();

		if (mObjectLifeTimes.find(_info->mId) == mObjectLifeTimes.end())
			_info->mBitset[0] = false;
		else
			_info->mBitset[0] = true;


		//TODO PAT: ADD MORE COMPONETS WHEN WE NEED THEM.
		std::vector<Component*>temp;
		for (int i = 1; i < ComponentType::eCOMPONENT_MAX; i++) {
			{
				if (i == ComponentType::eCOMPONENT_COLLIDER) {
					//Physics componets
					temp = _object->GetComponents(ComponentType::eCOMPONENT_COLLIDER);
					for (unsigned int i = 0; i < temp.size(); i++) {
						SnapComponent_Physics* newComp = new SnapComponent_Physics();
						newComp->mCompType = eCOMPONENT_COLLIDER;
						newComp->mBitNum = temp[i]->GetComponentIndex();
						_info->mBitset[newComp->mBitNum] = temp[i]->IsEnabled();
						////Dont waste cycles if component is not enabled
						//if (!_info->mBitset[newComp->mBitNum])
						//	continue;
						newComp->mForces = ((Collider*)temp[i])->mForces;
						newComp->mAcc = ((Collider*)temp[i])->mAcceleration;
						newComp->mVel = ((Collider*)temp[i])->mVelocity;

						//Set the bitset
						newComp->mId = temp[i]->GetColliderId();
						_info->mComponets.push_back(newComp);
					}
				} else {
					temp = _object->GetComponents((ComponentType)i);
					for (unsigned int i = 0; i < temp.size(); i++) {
						SnapComponent* newComp = new SnapComponent;
						newComp->mCompType = temp[i]->GetType();
						newComp->mBitNum = temp[i]->GetComponentIndex();
						_info->mBitset[newComp->mBitNum] = temp[i]->IsEnabled();
						newComp->mId = temp[i]->GetColliderId();
						_info->mComponets.push_back(newComp);
						////Dont waste cycles if component is not enabled
						//if (!_info->mBitset[newComp->mBitNum])
						//	continue;
					}
				}
			}
		}

		return _info;
	}

	//SnapInfoPlayer * Timeline::GenerateSnapInfoPlayer() {
	//	SnapInfoPlayer* snapP = new SnapInfoPlayer(
	//		*RenderEngine::Renderer::Instance()->GetPlayerWorldPos(),//Player World Matrix
	//		Math::FromMatrix(VRInputManager::Instance().GetController(true).GetPose().mDeviceToAbsoluteTracking),//Left Controller World Matrix
	//		Math::FromMatrix(VRInputManager::Instance().GetController(false).GetPose().mDeviceToAbsoluteTracking));//Right Controller World Matrix
	//
	//	return snapP;
	//}


	Snapshot* Timeline::GenerateSnapShot(unsigned int _time, std::vector<BaseObject*> & _clones) {
		Snapshot* snap;
		bool OldSnap = false;


		//We are making a new snap in the timeline
		//If the CurrentFrame is the last one on the list, make a new one
		if (mCurrentGameTimeIndx == mSnaptimes.size() - 1 || mSnaptimes.size() == 0) {
			snap = new Snapshot();
			snap->mTime = _time;
		}
		//We are on an old Snapshot
		else {
			snap = mSnapshots[_time];
			OldSnap = true;
		}

		//If first snapshot taken

		//TODO PAT: break up the logic loop here and 
		if (mSnapshots.size() == 0) {
			for (std::pair<unsigned short, BaseObject*> _b : mLiveObjects) {
				if (_b.second) {
					unsigned short id = _b.first;
					snap->mSnapinfos[id] = GenerateSnapInfo(_b.second, nullptr);
					snap->mUpdatedtimes[id] = _time;
				}
			}
		} else {
			//Copy the exciting updated times to this one
			snap->mUpdatedtimes = mSnapshots[mSnaptimes[mCurrentGameTimeIndx]]->mUpdatedtimes;
			for (std::pair<unsigned short, BaseObject*> _b : mLiveObjects) {
				if (_b.second) {
					unsigned short id = _b.first;
					if (_clones.size() > 0) {
						for (unsigned int i = 0; i < _clones.size(); i++) {
							//Move clone to next frame if frame is avalible
							if (snap->mSnapinfos.find(id) != snap->mSnapinfos.end() && id == _clones[i]->GetUniqueID()) {
								MoveObjectToSnap(_time, id);
								//Update the clone interpolators to move if there is a next next snap available.
								UpdateCloneInterpolators(_clones[i]->GetUniqueID(), snap->mSnapinfos[id], _time);
								break;
							}
							//If we are a clone but dont have a next movement then record one at position
							else if (snap->mSnapinfos.find(id) == snap->mSnapinfos.end() && id == _clones[i]->GetUniqueID()) {
								//If change add to mSnapinfos and Updatetime
								if (!CheckForDuplicateData(id, _b.second)) {
									snap->mSnapinfos[id] = GenerateSnapInfo(_b.second, nullptr);
									snap->mUpdatedtimes[id] = _time;
									break;
								}
							}
							//If we made it through the list do the normal
							else if (id != _clones[i]->GetUniqueID() && i == _clones.size() - 1) {
								//If change add to mSnapinfos and Updatetime
								if (!CheckForDuplicateData(id, _b.second)) {
									snap->mSnapinfos[id] = GenerateSnapInfo(_b.second, snap->mSnapinfos[id]);
									snap->mUpdatedtimes[id] = _time;
								}
							}
						}
					} else {
						//If change add to mSnapinfos and Updatetime
						if (!CheckForDuplicateData(id, _b.second)) {
							snap->mSnapinfos[id] = GenerateSnapInfo(_b.second, snap->mSnapinfos[id]);
							snap->mUpdatedtimes[id] = _time;
						}
					}
				}
			}
		}
		if (!OldSnap) {
			mSnaptimes.push_back(_time);
		}

		if (mSnapshots.size() != 0)
			mCurrentGameTimeIndx++;

		return snap;
	}

	//Returns True if the data is the same from last snap
	bool Timeline::CheckForDuplicateData(unsigned short _id, BaseObject* _object) {
		if (mCurrentGameTimeIndx == 0)
			return false;
		//If the object has not been made yet or is already dead return so we dont make one yet
		if (mObjectLifeTimes.find(_id) != mObjectLifeTimes.end() && (mObjectLifeTimes[_id]->mBirth > mCurrentGameTimeIndx || mObjectLifeTimes[_id]->mDeath < mCurrentGameTimeIndx))
			return true;

		SnapInfo* info;
		Snapshot* snap = mSnapshots[mSnaptimes[mCurrentGameTimeIndx]];
		//find if the object exist
		if (snap->mSnapinfos.find(_id) != snap->mSnapinfos.end())
			info = mSnapshots[mSnaptimes[mCurrentGameTimeIndx]]->mSnapinfos[_id];
		else if (snap->mUpdatedtimes.find(_id) != snap->mUpdatedtimes.end()) {
			info = mSnapshots[snap->mUpdatedtimes[_id]]->mSnapinfos[_id];
		} else {
			//if the object is not yet made(new headset or controller data) then we need to make it. This is the only exception I think
			//SystemLogger::GetLog() << _object->GetName() << std::endl;
			return false;
		}

		if (info->mTransform != _object->GetTransform())
			return false;

		for (unsigned int i = 0; i < info->mComponets.size(); i++) {
			SnapComponent* comp = info->mComponets[i];

			switch (comp->mCompType) {
				//For each of the collider in the vec
			case ComponentType::eCOMPONENT_COLLIDER:
			{
				//Loop to find the same collider component
				for (unsigned int j = 0; j < _object->GetComponentCount(eCOMPONENT_COLLIDER); j++) {
					Component* currComp = _object->GetComponentIndexed(eCOMPONENT_COLLIDER, j);
					if (currComp->GetColliderId() == comp->mId) {
						if (((Collider*)currComp)->mAcceleration != ((SnapComponent_Physics*)comp)->mAcc ||
							((Collider*)currComp)->mVelocity != ((SnapComponent_Physics*)comp)->mVel ||
							((Collider*)currComp)->mForces != ((SnapComponent_Physics*)comp)->mForces)
							return false;
					}
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}

		return true;
	}
}