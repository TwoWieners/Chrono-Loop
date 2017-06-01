#pragma once
#include "CodeComponent.hpp"
#include "..\Common\Logger.h"
#include "..\Core\LevelManager.h"
#include "..\Core\Pool.h"
#include "..\Core\TimeManager.h"
#include "..\Common\Settings.h"
#include <wrl\client.h>
#include "DirectXMath.h"


namespace Epoch
{

	struct CCLoadLevel : public CodeComponent
	{
		const wchar_t* _basePath = L"../Resources/Soundbanks/";
		const wchar_t* _initSB = L"Init.bnk";
		const wchar_t* _aSB = L"Test_Soundbank.bnk";
		const wchar_t* _mainS = L"Chrono_Sound.bnk";


		bool once = true;
		void SetOnce(bool _set) { once = _set; };
		bool GetOnce() { return once; };
		virtual void OnTriggerEnter(Collider& _col1, Collider& _col2)
		{
			if (Settings::GetInstance().GetBool("CompleteLevel4"))
				once = false;
		}
		virtual void Start()
		{
			once = true;
		}
		virtual void Update()
		{
			if (!once)
			{
				Settings::GetInstance().SetBool("LevelIsLoading", true);
				Level* next = new Level;
				int current = Settings::GetInstance().GetInt("CurrentLevel") + 1;
				next->BinaryLoadLevel(LevelManager::GetInstance().LevelOrder[current]);
				// Todo: Un-hardcode this
				// use a setting string for next level path?
				//LM::LevelStatus status = LevelManager::GetInstance().LoadLevelAsync("../Resources/Level1_2_6.xml", &next);
				if (/*status == LM::LevelStatus::Success*/ true)
				{
					// Clean up the current level and request the new one be used next time.
					Physics::Instance()->PhysicsLock.lock();
					Physics::Instance()->mObjects.clear();
					ParticleSystem::Instance()->Clear();
					LevelManager::GetInstance().RequestLevelChange(next);

					SystemLogger::Debug() << "Loading complete" << std::endl;
					Physics::Instance()->PhysicsLock.unlock();
					Settings::GetInstance().SetBool("LevelIsLoading", false);
					Settings::GetInstance().SetInt("CurrentLevel", current);
				}
			}
		}
	};

}