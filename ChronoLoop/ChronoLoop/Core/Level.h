#pragma once
////////////////////
//2/9/2017
//Written by: Ryan Bronk & Drew Ritter
///////////////////
#include "../ChronoLoop/Objects/BaseObject.h"
#include "../ChronoLoop/Rendering/renderer.h"
#include "../Rendering/Draw2D.h"
#include "../Actions/TimeManipulation.h"
#include "../Input/CommandConsole.h"


namespace Epoch
{


	class Level
	{
		friend class LM;
		BaseObject* mHeadset;
		BaseObject* mController1;
		BaseObject* mController2;
		TimeManipulation* mTMComponent1 = nullptr, *mTMComponent2 = nullptr;
		std::list<BaseObject*> mObjectList;
		unsigned short mId;
		vec4f mStartPosition, mStartRotation;
		unsigned int mMaxNumofClones;

	public:
		Level();
		~Level();
		bool ChronoLoop = true;
		bool mmflip = true;

		void AssignPlayerControls(BaseObject *_headset, BaseObject *_lController, BaseObject *_rController);

		//**GETTERS**//
		BaseObject* FindObjectWithName(std::string _name);
		std::vector<BaseObject*> FindAllObjectsWithName(std::string _name);
		std::list<BaseObject*>& GetLevelObjects() { return mObjectList; };
		void AddObject(BaseObject* _obj);
		bool RemoveObject(BaseObject* _obj);
		unsigned short GetId() { return mId; };
		inline BaseObject* GetHeadset() { return mHeadset; }
		inline BaseObject* GetLeftController() { return mController1; }
		inline BaseObject* GetRightController() { return mController2; }
		inline unsigned int GetMaxClones() { return mMaxNumofClones; }
		inline vec4f GetStartPos() { return mStartPosition; }
		inline vec4f GetStartRot() { return mStartRotation; }
		TimeManipulation* GetLeftTimeManipulator() { return mTMComponent1; }
		TimeManipulation* GetRightTimeManipulator() { return mTMComponent2; }


		//**SETTERS**//
		void SetId(unsigned short _set) { mId = _set; };
		void SetHeadsetAndControllers(BaseObject*& _headset, BaseObject*& _controller1, BaseObject*& _controller2, bool _addNewHeadsetToLevel);
		void SwapPlayerComponentIds(BaseObject *& _first, BaseObject*& _other);

		//**FUNCTIONS**//
		void SetupObjects();
		void CallStart();
		void LoadLevel(std::string _file);
		void Update();

		//**CONSOLE COMMAND FUNCS**//
		static void ToggleEntireLevelsWireframe(void * _command, std::wstring _ifOn);
		static void LoadLevelCmnd(void* _commandConsole, std::wstring _Level);

	};

}