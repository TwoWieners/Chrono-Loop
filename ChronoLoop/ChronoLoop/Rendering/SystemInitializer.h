#pragma once
#include "renderer.h"
#include "RasterizerStateManager.h"
#include "../VRInputManager.h"

namespace RenderEngine {
	bool InitializeSystems(HWND _Window, unsigned int _width, unsigned int _height,
												 bool _vsync, int _fps, bool _fullscreen, float _farPlane, float _nearPlane,
												 vr::IVRSystem* _vrsys) {
		Renderer::Instance()->Initialize(_Window, _width, _height, _vsync, _fps, _fullscreen, _farPlane, _nearPlane, _vrsys);
		RasterizerStateManager::Instance();
		if (_vrsys) {
			VRInputManager::Instance();
			VRInputManager::Initialize(_vrsys);
		}
		return true;
	}

	bool ShutdownSystems() {
		RasterizerStateManager::DestroyInstance();
		Renderer::DestroyInstance();
		TimeManager::Instance()->Destroy();
		VRInputManager::Shutdown();
		return true;
	}
}