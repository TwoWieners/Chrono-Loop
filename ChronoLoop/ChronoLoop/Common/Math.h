#pragma once
#include "Math/vec4f.h"
#include "Math/vec3f.h"
#include "Math/vec2f.h"
#include "Math/vec4i.h"
#include "Math/matrix4.h"

#include <DirectXMath.h>
#include <openvr.h>

struct matrix4;

namespace Math
{
	matrix4 MatrixRotateAxis(vec4f const& _axis, float _rads);
	matrix4 MatrixRotateX(float _rads);
	matrix4 MatrixRotateY(float _rads);
	matrix4 MatrixRotateZ(float _rads);
	matrix4 MatrixTranslation(float _x, float _y, float _z);
	matrix4 MatrixTranspose(matrix4 & other);
	matrix4 MatrixScale(float _x, float _y, float _z);
	matrix4 MatrixIdentity();
	matrix4 MatrixRotateInPlace(matrix4 _self, float _x, float _y, float _z, float _rads);
	matrix4 MatrixRotateInPlace(matrix4 _self, vec4f _axis, float _rads);
	matrix4 MatrixRotateAround(matrix4 _self, vec4f _axis, vec4f _point, float _rads);
	matrix4 FromMatrix(vr::HmdMatrix44_t _mat);
	matrix4 FromMatrix(vr::HmdMatrix34_t _mat);
}