#pragma once
#include <math.h>
#include <vector>

namespace SoftRaster {

	//四维向量
	struct Vector4
	{
		float x;
		float y;
		float z;
		float w;
	};

	//矩阵
	struct Matrix
	{
		float m[4][4];
	};

	//顶点
	struct vertex
	{
		Vector4 pos;
		Vector4 color;
	};

	//规范化
	Vector4 normalize(const Vector4& v) {
		float len = (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return { v.x / len, v.y / len, v.z / len, 0.0f };
	}

	//叉积
	Vector4 cross(const Vector4& u, const Vector4& v) {
		return { u.y * v.z - u.z * v.y,
				 u.z * v.x - u.x * v.z,
				 u.x * v.y - u.y * v.x, 0.0f };
	}

	//点积
	float dot(const Vector4& u, const Vector4& v) {
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	//矩阵乘法
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

	//变换
	Vector4 transform(const Vector4& v, const Matrix& m) {
		Vector4 r;
		r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
		r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
		r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
		r.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
		return r;
	}

	// 透视除法 齐次除法
	void perspectiveDivede(Vector4& v){
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;
		v.w = 1.0f;
	}

	// 转换到屏幕坐标
	void transformScreen(Vector4& v, int w, int h){
		v.x = (v.x + 1.0f) * w * 0.5f;
		v.y = (1.0f - v.y) * h * 0.5f;
	}


}