#pragma once
//Credit to John Murphy

#ifndef AudioWrapper_h
#define AudioWrapper_h

#include <vector>
#include <map>

#include "AkSoundEngineDLL.h"

struct vec4f;
class BaseObject;
class Emitter;
class Listener;

//Needed to work with Wwise soundbanks and the .h they generate
#if defined(_WIN64)
typedef unsigned __int64 AudioEvent;				///< Integer (unsigned) type for pointers
#else
typedef __w64 unsigned int AudioEvent;			///< Integer (unsigned) type for pointers
#endif
	class AudioWrapper
	{
		static AudioWrapper* audioSystem;

	private:
		std::vector<const Emitter*> mEmitters;
		std::vector<const Listener*> mListeners;
		std::vector<const BaseObject*> mObjects;
		std::map<std::wstring, AkBankID> mRegisteredSoundBanks;

		bool mIsInitialize = false;
		float mWorldScale;
	public:
		AudioWrapper* GetInstance() { return audioSystem; }

		AudioWrapper();
		~AudioWrapper();

		bool Initialize();
		void Shutdown();
		void Update();

		void SetWorldScale(float _scale);
		bool IsInitialized() { return mIsInitialize; }

		bool AddListener(const Listener * _listener, const char* _name);
		bool RemoveListener(const Listener * _listener);
		bool AddEmitter(const Emitter * _emitter, const char* _name);
		bool RemoveEmitter(const Emitter * _emitter);

		bool AddObject(const BaseObject* _obj, const char* _name);
		bool RemoveObject(const BaseObject* _obj);

		//Posts an event at a pos, emitter location, or near a listener.
		bool MakeEventAtLocation(AudioEvent _id, vec4f* _pos);
		bool MakeEvent(AudioEvent _id, const Emitter * _emitter);
		bool MakeEventAtListener(AudioEvent _id, unsigned int _listenerID = 0);
		bool MakeEvent(AudioEvent _id, const BaseObject* _obj);

		//Set soundbank path(s)
		void SetBasePath(const wchar_t* _strPath);
		bool LoadSoundBank(const wchar_t* _BankName);
		bool UnloadSoundBank(const wchar_t* _BankName);

	};
#endif // AudioWrapper_h

