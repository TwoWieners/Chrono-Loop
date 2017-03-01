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
		float _floats[16];
	};

	matrix4();
	matrix4(matrix4& _copy);
	matrix4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44);
	DirectX::XMMATRIX matrix4::GetUnderlyingType() { return matrix; }

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
	float*   AsArray() { return reinterpret_cast<float*>(matrix.r); }
};