#pragma once
#include "../Common/Math.h"
using namespace Math;
class Transform
{
private:
	matrix4 m_worldMatrix;//World
	vec4f up,forward,right;//Local//y,z,x
public:
	//**MOVEMENT**//
	void TranslateLocal(float x, float y, float z);
	void TranslateGlobal(float x, float y, float z) { this->m_worldMatrix = MatrixTranslation(x, y, z); };
	void Rotate(vec4f axis, float radians) { this->m_worldMatrix = MatrixRotateAxis(axis, radians); };
	void RotateAround(vec4f point, vec4f axis, float radians) {/*Insert Code Here*/ };
	void RotateInPlace(float x, float y, float z) {/*Insert Code Here*/ };
	void Scale(float x, float y, float z) { this->m_worldMatrix = MatrixScale(x, y, z); };

	//**GETTERS**//
	const vec4f* GetXAxis() 
	{
		return reinterpret_cast<const vec4f*>(&m_worldMatrix.data.first);
	};
	const vec4f* GetYAxis() 
	{
		return reinterpret_cast<const vec4f*>(&m_worldMatrix.data.second);
	};
	const vec4f* GetZAxis() 
	{
		return reinterpret_cast<const vec4f*>(&m_worldMatrix.data.third);
	};
	const vec4f* GetPosition() 
	{
		return reinterpret_cast<const vec4f*>(&m_worldMatrix.data.fourth);
	};

	//**OPERATORS**//
	Transform const* operator=(Transform const _t);
	bool operator==(Transform const _t);
	bool operator!=(Transform const _t);
};