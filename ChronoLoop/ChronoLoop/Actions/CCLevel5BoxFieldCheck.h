#pragma once
#include "CodeComponent.hpp"
#include "TeleportAction.hpp"
#include "../Objects/BaseObject.h"
#include "../Core/LevelManager.h"
#include "../Physics/Physics.h"
#include "BoxSnapToControllerAction.hpp"
#include "CCLevel5Fields.h"


namespace Epoch
{
	struct CCLevel5BoxFieldCheck : public CodeComponent
	{
		BaseObject* mBox;
		Level* cLevel;
		ControllerType mControllerRole = eControllerType_Primary;
		TeleportAction* mRTeleportAction, *mLTeleportAction;
		BoxSnapToControllerAction* mRightBS,* mLeftBS;
		BaseObject* mStartField,* mExitField;
		CCLevel5Fields *mStartFScript, *mExitFScript;

		Triangle* mStartFieldTris, *mExitFieldTris;
		size_t mStartFieldTrisSize, mExitFieldTrisSize;
		bool mStartDisappear = false, mEndDisappear = false;
		float hitTime = 0.0f;

		virtual void Start()
		{
			cLevel = LevelManager::GetInstance().GetCurrentLevel();
			mBox = cLevel->FindObjectWithName("Box");
			mStartField = cLevel->FindObjectWithName("StartSideEnergyWall");
			mExitField = cLevel->FindObjectWithName("ExitSideEnergyWall");
			std::vector<Component*> codes1 = cLevel->GetRightController()->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<TeleportAction*>(codes1[x]))
				{
					mRTeleportAction = ((TeleportAction*)codes1[x]);
					break;
				}
			}
			codes1 = cLevel->GetLeftController()->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<TeleportAction*>(codes1[x]))
				{
					mLTeleportAction = ((TeleportAction*)codes1[x]);
					break;
				}
			}
			codes1 = cLevel->GetRightController()->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<BoxSnapToControllerAction*>(codes1[x]))
				{
					mRightBS = ((BoxSnapToControllerAction*)codes1[x]);
					break;
				}
			}
			codes1 = cLevel->GetLeftController()->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<BoxSnapToControllerAction*>(codes1[x]))
				{
					mLeftBS = ((BoxSnapToControllerAction*)codes1[x]);
					break;
				}
			}
			codes1 = mStartField->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<CCLevel5Fields*>(codes1[x]))
				{
					mStartFScript = ((CCLevel5Fields*)codes1[x]);
					break;
				}
			}			
			codes1 = mExitField->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<CCLevel5Fields*>(codes1[x]))
				{
					mExitFScript = ((CCLevel5Fields*)codes1[x]);
					break;
				}
			}
			mStartFieldTris = ((MeshComponent*)mStartField->GetComponentIndexed(eCOMPONENT_MESH, 0))->GetTriangles();
			mExitFieldTris = ((MeshComponent*)mExitField->GetComponentIndexed(eCOMPONENT_MESH, 0))->GetTriangles();

			mStartFieldTrisSize = ((MeshComponent*)mStartField->GetComponentIndexed(eCOMPONENT_MESH, 0))->GetTriangleCount();
			mExitFieldTrisSize = ((MeshComponent*)mExitField->GetComponentIndexed(eCOMPONENT_MESH, 0))->GetTriangleCount();
		}
		virtual void Update()
		{
			if ((VRInputManager::GetInstance().GetController(eControllerType_Primary).GetPressUp(vr::EVRButtonId::k_EButton_SteamVR_Touchpad) ||
				VRInputManager::GetInstance().GetController(eControllerType_Secondary).GetPressUp(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)) &&
				!Settings::GetInstance().GetBool("CantTeleport"))
			{
				if (mLeftBS)
				{
					if (mLeftBS->mHeld)
					{
						mLTeleportAction->Update();
						for (unsigned int i = 0; i < mStartFieldTrisSize; i++)
						{
						matrix4 inverse = mStartField->GetWorld().Invert();
						vec4f direction = mLTeleportAction->GetEndPos().Position - mBox->GetTransform().GetMatrix().Position;
						//direction *= inverse;
						direction = direction.Normalize();
						vec3f startPosition = (mBox->GetTransform().GetMatrix()).Position;
							hitTime = FLT_MAX;
							if (Physics::Instance()->RayToTriangle(
								(mStartFieldTris + i)->Vertex[0],
								(mStartFieldTris + i)->Vertex[1],
								(mStartFieldTris + i)->Vertex[2],
								(mStartFieldTris + i)->Normal,
								startPosition,
								vec3f(direction),
								hitTime))
							{
								if (hitTime < FLT_MAX)
								{
									SystemLogger::GetLog() << "Raycast hit from left controller" << std::endl;

									mStartFScript->SetIsBoxShrinking(true);
									return;
								}
							}
						}
						for (unsigned int i = 0; i < mExitFieldTrisSize; i++)
						{
							hitTime = FLT_MAX;
							if (Physics::Instance()->RayToTriangle(
								(mExitFieldTris + i)->Vertex[0],
								(mExitFieldTris + i)->Vertex[1],
								(mExitFieldTris + i)->Vertex[2],
								(mExitFieldTris + i)->Normal,
								vec3f(mBox->GetTransform().GetMatrix().Position),
								vec3f(mLTeleportAction->GetEndPos().Position - mBox->GetTransform().GetMatrix().Position),
								hitTime))
							{
								if (hitTime < FLT_MAX)
								{
									mExitFScript->SetIsBoxShrinking(true);
									return;
								}
							}
						}

					}
				}
				if (mRightBS)
				{
					if (mRightBS->mHeld)
					{
						for (unsigned int i = 0; i < mStartFieldTrisSize; i++)
						{
							hitTime = FLT_MAX;
							Physics::Instance()->RayToTriangle(
								(mStartFieldTris + i)->Vertex[0],
								(mStartFieldTris + i)->Vertex[1],
								(mStartFieldTris + i)->Vertex[2],
								(mStartFieldTris + i)->Normal,
								vec3f(mBox->GetTransform().GetMatrix().Position),
								vec3f(mRTeleportAction->GetEndPos().Position - mBox->GetTransform().GetMatrix().Position),
								hitTime);
							
							if (hitTime < FLT_MAX)
							{
								SystemLogger::GetLog() << "Raycast hit from right controller" << std::endl;

								mStartFScript->SetIsBoxShrinking(true);
								return;
							}
							
						}
						for (unsigned int i = 0; i < mExitFieldTrisSize; i++)
						{
							hitTime = FLT_MAX;
							Physics::Instance()->RayToTriangle(
								(mExitFieldTris + i)->Vertex[0],
								(mExitFieldTris + i)->Vertex[1],
								(mExitFieldTris + i)->Vertex[2],
								(mExitFieldTris + i)->Normal,
								vec3f(mBox->GetTransform().GetMatrix().Position),
								vec3f(mRTeleportAction->GetEndPos().Position - mBox->GetTransform().GetMatrix().Position),
								hitTime);
							
							if (hitTime < FLT_MAX)
							{
								mExitFScript->SetIsBoxShrinking(true);
								return;
							}
						}
					}
				}
			}
		}

	};
}