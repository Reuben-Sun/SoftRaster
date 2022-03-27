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
        // 世界坐标系转观察坐标系，V矩阵
        Matrix _worldToView;
        // 观察坐标系转投影坐标系，P矩阵
        Matrix _viewToProjection;

    };

}