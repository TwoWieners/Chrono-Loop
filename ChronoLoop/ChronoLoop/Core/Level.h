#pragma once
////////////////////
//2/9/2017
//Written by: Ryan Bronk & Drew Ritter
///////////////////
#include "../ChronoLoop/Objects/BaseObject.h"
#include "../ChronoLoop/Rendering/renderer.h"


namespace Epoch {

	class Level {
		static Level* sInstance;
		BaseObject* mHeadset;
		BaseObject* mController1;
		BaseObject* mController2;
		//yea.... make a list or welcome to the hell of random
		//Change the name at position and DO NOT RE-INSERT
		//std::unordered_map<std::string, std::vector<BaseObject*>> mObjectMap;
		std::list<BaseObject*> mObjectList;
		unsigned short mId;


		Level();
		~Level();
	public:
		static Level* Instance();
		static void DestroyInstance();
		static void Initialize(BaseObject *_headset, BaseObject *_lController, BaseObject *_rController);

		//**GETTERS**//
		BaseObject* iFindObjectWithName(std::string _name);
		std::vector<BaseObject*> iFindAllObjectsWithName(std::string _name);
		std::list<BaseObject*> iGetLevelObjects() { return mObjectList; };
		void iAddObject(BaseObject* _obj);
		bool iRemoveObject(BaseObject* _obj);
		unsigned short iGetId() { return mId; };
		inline BaseObject* iGetHeadset() { return mHeadset; }
		inline BaseObject* iGetLeftController() { return mController1; }
		inline BaseObject* iGetRightController() { return mController2; }


		//**SETTERS**//
		void iSetId(unsigned short _set) { mId = _set; };
		void iSetHeadsetAndControllers(BaseObject* _headset, BaseObject* _controller1, BaseObject* _controller2, ControllerCollider* _c1Collider, ControllerCollider* _c2Collider);

		//**FUNCTIONS**//
		void iCallStart();
		void iLoadLevel();
		void iUpdate();
		bool iOnObjectNamechange(BaseObject* _obj, std::string _name);

	};

}