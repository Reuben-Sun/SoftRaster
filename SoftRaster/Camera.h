#pragma once
#include "MathLibrary.h"

namespace SoftRaster {
    class Camera {
    public:
        Camera(Vector4 pos, Vector4 target, Vector4 up) : _pos(pos), _posTemp(pos), _target(target), _up(up) {}
        virtual ~Camera() noexcept {}
        void setPos(const Vector4& pos)
        {
            _pos = pos;
            calcMatrix();
        }
        void setPerspectiveForLH(float fov, float aspect, float nearZ, float farZ)
        {
            _fov = fov; _aspect = aspect; _nearZ = nearZ; _farZ = farZ;
            calcMatrix();
        }
        // 环绕
        void circle(short xMove, short yMove)
        {
            // 鼠标移动像素与弧度的比例固定
            float circleLen = 100.f;

            // 1 计算绕y轴的旋转
            float radY = xMove / circleLen;
            Matrix mScaleY = {
                (float)cos(radY), 0.0f, -(float)sin(radY), 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                (float)sin(radY), 0.0f, (float)cos(radY), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            // 2 计算绕x轴 这里需要设定一个最大角度
            float radX = yMove / circleLen;
            float maxRad = 3.1415926f * 0.45f;
            _curXRand += radX;
            if (_curXRand < -maxRad)
            {
                _curXRand = -maxRad;
                radX = 0.0f;
            }
            if (_curXRand > maxRad)
            {
                _curXRand = maxRad;
                radX = 0.0f;
            }

            Matrix mScaleX = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, (float)cos(radX), (float)sin(radX), 0.0f,
                0.0f, -(float)sin(radX), (float)cos(radX), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            _pos = transform(_pos, mScaleX);
            _pos = transform(_pos, mScaleY);
            calcMatrix();
        }

        // 缩放
        void zoom(short wParam)
        {
            float t = 0.9f;
            if (wParam > 0) t = 1.1f;
            _pos.x *= t;
            _pos.y *= t;
            _pos.z *= t;
            calcMatrix();
        }

        void reset()
        {
            _pos = _posTemp;
            _curXRand = 0.0f;
            calcMatrix();
        }

    public:
        // 世界坐标系转到投影平面，VP矩阵
        Matrix _worldToProjection;
    private:
        void calcMatrix()
        {
            Vector4 dir = { _target.x - _pos.x, _target.y - _pos.y, _target.z - _pos.z, 0.0f };     //视线方向
            Vector4 w = normalize(dir);     //规范化后的视线
            Vector4 u = normalize(cross(_up, w));   //右向量（用于重建坐标系）
            Vector4 v = cross(w, u);        //更精确的上向量
            _worldToView = {
                u.x, v.x, w.x, 0.0f,
                u.y, v.y, w.y, 0.0f,
                u.z, v.z, w.z, 0.0f,
                -dot(_pos, u), -dot(_pos, v), -dot(_pos, w), 1.0
            };
            float f = 1.0f / (float)tan(_fov * 0.5f);
            _viewToProjection = {
                f / _aspect, 0.0f, 0.0f, 0.0f,
                0.0f, f, 0.0f, 0.0f,
                0.0f, 0.0f, _farZ / (_farZ - _nearZ), 1.0f,
                0.0f, 0.0f, -_nearZ * _farZ / (_farZ - _nearZ), 0.0f
            };
            _worldToProjection = mul(_worldToView, _viewToProjection);
        }
    private:
        Vector4 _pos;
        Vector4 _posTemp;
        Vector4 _target;
        Vector4 _up;
        float _fov;
        float _aspect;
        float _nearZ;
        float _farZ;
        float _curXRand = 0.0f;
        // 世界坐标系转观察坐标系，V矩阵
        Matrix _worldToView;
        // 观察坐标系转投影坐标系，P矩阵
        Matrix _viewToProjection;

    };

}