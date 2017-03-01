//#include "stdafx.h"
#include "Math.h"
#include <memory>



matrix4 Math::MatrixRotateAxis(vec4f const& _axis, float _rads)
{
	matrix4 temp;
	DirectX::XMVECTOR vec;

	for (int i = 0; i < 4; ++i)
		vec.m128_f32[i] = _axis.xyzw[i];

	temp.matrix = DirectX::XMMatrixRotationAxis(vec, _rads);
	return temp;
}

matrix4 Math::MatrixRotateX(float _rads)
{
	matrix4 temp;
	temp.matrix = DirectX::XMMatrixRotationX(_rads);
	return temp;
}

matrix4 Math::MatrixRotateY(float _rads)
{
	matrix4 temp;
	temp.matrix = DirectX::XMMatrixRotationY(_rads);
	return temp;
}

matrix4 Math::MatrixRotateZ(float _rads)
{
	matrix4 temp;
	temp.matrix = DirectX::XMMatrixRotationZ(_rads);
	return temp;
}

matrix4 Math::MatrixTranslation(float _x, float _y, float _z)
{
	matrix4 temp;
	temp.matrix = DirectX::XMMatrixTranslation(_x, _y, _z);
	return temp;
}

matrix4 Math::MatrixTranspose(matrix4& other)
{
	matrix4 temp;
	temp.matrix = DirectX::XMMatrixTranspose(other.matrix);
	return temp;
}

matrix4 Math::MatrixScale(float _x, float _y, float _z)
{
	matrix4 temp;
	temp.matrix = DirectX::XMMatrixScaling(_x, _y, _z);
	return temp;
}

matrix4 Math::MatrixIdentity() 
{
	matrix4 m;
	m.matrix = DirectX::XMMatrixIdentity();
	return m;
}

matrix4 Math::MatrixRotateInPlace(matrix4 _self, float _x, float _y, float _z, float _rads) {
	vec4f pos;
	for (int i = 0; i < 4; ++i) {
		pos[i] = _self[3][i];
		_self[3][i] = 0;
	}
	_self *= Math::MatrixRotateAxis({ _x, _y, _z, 0 }, _rads);
	for (int i = 0; i < 4; ++i) {
		_self[3][i] = pos[i];
	}
	return _self;
}

matrix4 Math::MatrixRotateInPlace(matrix4 _self, vec4f _axis, float _rads) {
	return MatrixRotateInPlace(_self, _axis.x, _axis.y, _axis.z, _rads);
}


matrix4 Math::MatrixRotateAround(matrix4 _self, vec4f _axis, vec4f _point, float _rads) {
	vec4f pos;
	for (int i = 0; i < 3; ++i) {
		pos[i] = _self[i][3];
		_self[i][3] = pos[i] - _point[i]; // Set object's position to the delta of where it is to where it rotates about.
		pos[i] = _point[i] - pos[i];
	}
	// TODO: Fix this.
	_self *= Math::MatrixRotateAxis(_axis, _rads);
	for (int i = 0; i < 3; ++i) {
		_self[i][3] += pos[i]; // pos[i] is now the offset on each component, which we add back into the rotated object.
	}
	return _self;
}

matrix4 Math::FromMatrix(vr::HmdMatrix44_t _mat)
{
	// so [i][j] would leave us with a column major version, which is bad.
	matrix4 temp;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			temp.tiers[i].xyzw[j] = _mat.m[j][i];

	return temp;
}

matrix4 Math::FromMatrix(vr::HmdMatrix34_t _mat)
{
	matrix4 matrixObj(
		_mat.m[0][0], _mat.m[1][0], _mat.m[2][0], 0.0,
		_mat.m[0][1], _mat.m[1][1], _mat.m[2][1], 0.0,
		_mat.m[0][2], _mat.m[1][2], _mat.m[2][2], 0.0,
		_mat.m[0][3], _mat.m[1][3], _mat.m[2][3], 1.0f
	);
	return matrixObj;
}