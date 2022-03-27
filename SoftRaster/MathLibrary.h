#pragma once
#include <math.h>
#include <vector>

namespace SoftRaster {

	//��ά����
	struct Vector4
	{
		float x;
		float y;
		float z;
		float w;
	};

	//����
	struct Matrix
	{
		float m[4][4];
	};

	//����
	struct vertex
	{
		Vector4 pos;
		Vector4 color;
	};

	//�淶��
	Vector4 normalize(const Vector4& v) {
		float len = (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return { v.x / len, v.y / len, v.z / len, 0.0f };
	}

	//���
	Vector4 cross(const Vector4& u, const Vector4& v) {
		return { u.y * v.z - u.z * v.y,
				 u.z * v.x - u.x * v.z,
				 u.x * v.y - u.y * v.x, 0.0f };
	}

	//���
	float dot(const Vector4& u, const Vector4& v) {
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	//����˷�
	Matrix mul(const Matrix& a, const Matrix& b) {
		Matrix r;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				r.m[i][j] = a.m[i][0] * b.m[0][j] +
							a.m[i][1] * b.m[1][j] +
							a.m[i][2] * b.m[2][j] +
							a.m[i][3] * b.m[3][j];
			}
		}
		return r;
	}

	//�任
	Vector4 transform(const Vector4& v, const Matrix& m) {
		Vector4 r;
		r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
		r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
		r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
		r.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
		return r;
	}

	// ͸�ӳ��� ��γ���
	void perspectiveDivede(Vector4& v){
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;
		v.w = 1.0f;
	}

	// ת������Ļ����
	void transformScreen(Vector4& v, int w, int h){
		v.x = (v.x + 1.0f) * w * 0.5f;
		v.y = (1.0f - v.y) * h * 0.5f;
	}


}