#pragma once
#include <DirectXMath.h>
#include "vec4f.h"

struct matrix4 {
	union {
		DirectX::XMMATRIX matrix;
		vec4f tiers[4];
		struct {
			vec4f first, second, third, fourth;
		};
		vec4f rows[4];
		struct {
			vec4f xAxis, yAxis, zAxis, Position;
		};
		float _floats[16];
	};

	matrix4();
	matrix4(matrix4& _copy);
	matrix4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44);
	matrix4(DirectX::XMMATRIX _xm);
	matrix4(vr::HmdMatrix44_t& _m);
	matrix4(vr::HmdMatrix34_t& _m);

	bool     matrix4::operator==(matrix4 const& _other);
	bool     matrix4::operator!=(matrix4 const& _other);
	matrix4& matrix4::operator=(matrix4 const& _other);
	matrix4  matrix4::operator*(matrix4 const& _other);
	matrix4& matrix4::operator*=(matrix4 const& _other);
	matrix4  matrix4::operator*(float _other);
	matrix4& matrix4::operator*=(float _other);
	matrix4  matrix4::operator+(matrix4 const& _other);
	matrix4& matrix4::operator+=(matrix4 const& _other);
	vec4f&   matrix4::operator[](unsigned int _index);

	matrix4  Inverse();
	matrix4  Invert();
	matrix4  Transpose();
	matrix4& ToIdentity();
	matrix4& RotateInPlace(float _x, float _y, float _z, float _rads);
	matrix4& RotateInPlace(const vec3f& _axis, float _rads);
	matrix4& RotateInPlaceQuaternion(const vec4f& _quat);

	static matrix4 CreateAxisRotation(const vec3f& _axis, float _rads);
	static matrix4 CreateQuaternionRotation(const vec4f& _quat);
	static matrix4 CreateXRotation(float _rads);
	static matrix4 CreateYRotation(float _rads);
	static matrix4 CreateZRotation(float _rads);
	static matrix4 CreateScale(float _x, float _y, float _z);
	static matrix4 CreateTranslation(float _x, float _y, float _z);
};