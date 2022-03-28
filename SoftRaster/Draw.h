#pragma once
#include "MathLibrary.h"
#include "SoftRaster.h"

namespace SoftRaster {
    extern int g_width;
    extern int g_height;
    extern unsigned int* g_frameBuffer;
    extern Camera camera;
    extern std::vector<vertex> vertexes;
    extern std::shared_ptr<float[]> g_depthBuffer;

    //������ɫ
    unsigned int drawColor = (255 << 8);    //������ɫ��Ĭ��Ϊ��ɫ

    //����ģʽ
    enum class RenderMode
    {
        RENDER_WIREFRAME,   //�߿�
        RENDER_COLOR,       //��ɫ
    };

    //��ʼ������ģʽ
    RenderMode g_renderMode = RenderMode::RENDER_COLOR;

    //����۲���CVV�ü�
    bool checkCvv(const Vector4& v);

    //������������
    Vector4 barycentric(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& p);

	//��������
	void drawPixel(int x, int y, unsigned int color);

	//������
	void drawLine(int x1, int y1, int x2, int y2, unsigned int color);

	//��������ͼԪ
	void drawPrimitive(const vertex& a, const vertex& b, const vertex& c);

    //��������ͼԪ
    void drawPrimitiveScanLine(const vertex& a, const vertex& b, const vertex& c);

	//����ƽ��
	void drawPlane(int leftTop, int rightTop, int rightBottom, int leftBottom);

	//����������
	void drawCube();
}

bool SoftRaster::checkCvv(const Vector4& v) {
    //Ϊʲôʹ��CVV����Ϊ��׶����һ��׶�Σ��������ü�����CVV����οռ䣬��һ�������壬xyz����[-1,1]���������ü�
    if (v.z < 0.0f) return true;
    if (v.z > v.w) return true;
    if (v.x < -v.w) return true;
    if (v.x > v.w) return true;
    if (v.y < -v.w) return true;
    if (v.y > v.w) return true;
    return false;
}

SoftRaster::Vector4 SoftRaster::barycentric(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& p)
{
    Vector4 v1 = { c.x - a.x, b.x - a.x, a.x - p.x };   
    Vector4 v2 = { c.y - a.y, b.y - a.y, a.y - p.y };   

    Vector4 u = cross(v1, v2);
    if (std::abs(u.z) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return { 1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z };
    return { -1, 1, 1, 0 }; // in this case generate negative coordinates, it will be thrown away by the rasterizator
}


void SoftRaster::drawPixel(int x, int y, unsigned int color)
{
	if (x < 0 || x >= g_width || y < 0 || y >= g_height) return;

	int idx = y * g_width + x;
	g_frameBuffer[idx] = color;
}

void SoftRaster::drawLine(int x1, int y1, int x2, int y2, unsigned int color) {
    if (x1 == x2 && y1 == y2)
    {
        drawPixel(x1, y1, color);
    }
    else if (x1 == x2)
    {
        if (y1 > y2) std::swap(y1, y2);
        for (int y = y1; y <= y2; ++y)
            drawPixel(x1, y, color);
    }
    else if (y1 == y2)
    {
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; ++x)
            drawPixel(x, y1, color);
    }
    else
    {
        // Bresenham
        int diff = 0;
        int dx = std::abs(x1 - x2);
        int dy = std::abs(y1 - y2);
        if (dx > dy)    //��ת������㷨Ҫ���᳤������
        {
            if (x1 > x2) std::swap(x1, x2), std::swap(y1, y2);
            for (int x = x1, y = y1; x < x2; ++x)
            {
                drawPixel(x, y, color);
                diff += dy;
                if (diff >= dx)
                {
                    diff -= dx;
                    y += (y1 < y2) ? 1 : -1;
                }
            }
            drawPixel(x2, y2, color);
        }
        else
        {
            if (y1 > y2) std::swap(x1, x2), std::swap(y1, y2);
            for (int y = y1, x = x1; y < y2; ++y)
            {
                drawPixel(x, y, color);
                diff += dx;
                if (diff >= dy)
                {
                    diff -= dy;
                    x += (x1 < x2) ? 1 : -1;
                }
            }
            drawPixel(x2, y2, color);
        }
    }

}

void SoftRaster::drawPrimitive(const vertex& a, const vertex& b, const vertex& c) {
    // 1.3 ˳·���򵥵�cvv�ü�
    Matrix m = camera._worldToProjection;
    Vector4 p1 = transform(a.pos, m); if (checkCvv(p1)) return;
    Vector4 p2 = transform(b.pos, m); if (checkCvv(p2)) return;
    Vector4 p3 = transform(c.pos, m); if (checkCvv(p3)) return;

    // 2. ͸�ӳ��� ��һ��NDC����ϵ
    // x[-1, 1] y[-1, 1] z[near, far]
    perspectiveDivede(p1);
    perspectiveDivede(p2);
    perspectiveDivede(p3);

    // 3. ת������Ļ����
    transformScreen(p1, g_width, g_height);
    transformScreen(p2, g_width, g_height);
    transformScreen(p3, g_width, g_height);

    if (g_renderMode == RenderMode::RENDER_WIREFRAME)  
    {
        //�����߿�
        int x1 = (int)(p1.x + 0.5f), x2 = (int)(p2.x + 0.5f), x3 = (int)(p3.x + 0.5f);
        int y1 = (int)(p1.y + 0.5f), y2 = (int)(p2.y + 0.5f), y3 = (int)(p3.y + 0.5f);
        drawLine(x1, y1, x2, y2, drawColor);
        drawLine(x2, y2, x3, y3, drawColor);
        drawLine(x1, y1, x3, y3, drawColor);
    }
    else
    {
        //������
        drawPrimitiveScanLine({ p1, a.color }, { p2, b.color }, { p3, c.color });
    }


}

void SoftRaster::drawPrimitiveScanLine(const vertex& a, const vertex& b, const vertex& c) {
    float xl = a.pos.x; if (b.pos.x < xl) xl = b.pos.x; if (c.pos.x < xl) xl = c.pos.x;
    float xr = a.pos.x; if (b.pos.x > xr) xr = b.pos.x; if (c.pos.x > xr) xr = c.pos.x;
    float yt = a.pos.y; if (b.pos.y < yt) yt = b.pos.y; if (c.pos.y < yt) yt = c.pos.y;
    float yb = a.pos.y; if (b.pos.y > yb) yb = b.pos.y; if (c.pos.y > yb) yb = c.pos.y;

    int xMin = (int)(xl + 0.5f), xMax = (int)(xr + 0.5f), yMin = (int)(yt + 0.5f), yMax = (int)(yb + 0.5f);
    for (int x = xMin; x <= xMax; ++x)
    {
        for (int y = yMin; y <= yMax; ++y)
        {
            // �����Ƿ����������ڲ�
            Vector4 ret = barycentric(a.pos, b.pos, c.pos, { (float)x, (float)y, 0.0f, 0.0f });
            if (ret.x < 0 || ret.y < 0 || ret.z < 0) continue;
            unsigned int colorR = (unsigned int)((a.color.x * ret.x + b.color.x * ret.y + c.color.x * ret.z) * 255);
            unsigned int colorG = (unsigned int)((a.color.y * ret.x + b.color.y * ret.y + c.color.y * ret.z) * 255);
            unsigned int colorB = (unsigned int)((a.color.z * ret.x + b.color.z * ret.y + c.color.z * ret.z) * 255);
            float depth = (a.pos.z * ret.x + b.pos.z * ret.y + c.pos.z * ret.z);
            if (g_depthBuffer[x + y * g_width] < depth)continue;
            g_depthBuffer[x + y * g_width] = depth;
            drawPixel(x, y, (colorR << 16) | (colorG << 8) | colorB);
        }
    }

}

void SoftRaster::drawPlane(int leftTop, int rightTop, int rightBottom, int leftBottom) {
	drawPrimitive(vertexes[leftTop], vertexes[rightTop], vertexes[rightBottom]);
	drawPrimitive(vertexes[leftTop], vertexes[rightBottom], vertexes[leftBottom]);
}

void SoftRaster::drawCube() {
	drawPlane(0, 1, 2, 3);  // ����
	drawPlane(1, 5, 6, 2);  // ����
	drawPlane(4, 0, 3, 7);  // ����
	drawPlane(4, 5, 1, 0);  // ����
	drawPlane(3, 2, 6, 7);  // ����
	drawPlane(5, 4, 7, 6);  // ����
}
