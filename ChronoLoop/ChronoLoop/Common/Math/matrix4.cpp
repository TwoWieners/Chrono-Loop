#include "matrix4.h"
#include "vec4f.h"

matrix4::matrix4() {
	tiers[0].Set(1, 0, 0, 0);
	tiers[1].Set(0, 1, 0, 0);
	tiers[2].Set(0, 0, 1, 0);
	tiers[3].Set(0, 0, 0, 1);
}

matrix4::matrix4(matrix4& _copy) {
	matrix = _copy.matrix;
}

matrix4::matrix4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) {
	tiers[0].Set(_11, _12, _13, _14);
	tiers[1].Set(_21, _22, _23, _24);
	tiers[2].Set(_31, _32, _33, _34);
	tiers[3].Set(_41, _42, _43, _44);
}

bool matrix4::operator==(matrix4 const& _other) {
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (tiers[i].xyzw[j] != _other.tiers[i].xyzw[j])
				return false;
	return true;
}

bool matrix4::operator!=(matrix4 const& _other) {
	return !(*this == _other);
}

matrix4& matrix4::operator=(matrix4 const& _other) {
	if (*this != _other)
		this->matrix = _other.matrix;
	return *this;
}

matrix4 matrix4::operator*(matrix4 const& _other) {
	matrix4 temp;
	temp.matrix = this->matrix * _other.matrix;
	return temp;
}

matrix4& matrix4::operator*=(matrix4 const& _other) {
	*this = *this * _other;
	return *this;
}

matrix4 matrix4::operator*(float _other) {
	matrix4 temp;
	temp.matrix = matrix * _other;
	return temp;
}

matrix4& matrix4::operator*=(float _other) {
	*this = *this * _other;
	return *this;
}

matrix4 matrix4::operator+(matrix4 const& _other) {
	matrix4 temp;
	temp.matrix = matrix + _other.matrix;
	return temp;
}

matrix4& matrix4::operator+=(matrix4 const& _other) {
	*this = *this + _other;
	return *this;
}

vec4f& matrix4::operator[](unsigned int _index) {
	if (_index < 4)
		return tiers[_index];
	return tiers[0];
}

matrix4 matrix4::Inverse() {
	matrix4 m;
	m.matrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(matrix), matrix);
	return m;
}