#pragma once
#include "MathLibrary.h"
#include "SoftRaster.h"

namespace SoftRaster {
    extern int g_width;
    extern int g_height;
    extern unsigned int* g_frameBuffer;
    extern Camera camera;
    extern std::vector<vertex> vertexes;

    //������ɫ
    unsigned int drawColor = (255 << 8);    //������ɫ��Ĭ��Ϊ��ɫ

    //����ģʽ
    enum class RenderMode
    {
        RENDER_WIREFRAME,   //�߿�
        RENDER_COLOR,       //��ɫ
    };

    //��ʼ������ģʽ
    RenderMode g_renderMode = RenderMode::RENDER_WIREFRAME;

    //����۲���CVV�ü�
    bool checkCvv(const Vector4& v);

	//��������
	void drawPixel(int x, int y, unsigned int color);

	//������
	void drawLine(int x1, int y1, int x2, int y2, unsigned int color);

	//����������
	void drawPrimitive(const vertex& a, const vertex& b, const vertex& c);

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

    // 4. �����߿�
    if (g_renderMode == RenderMode::RENDER_WIREFRAME)
    {
        int x1 = (int)(p1.x + 0.5f), x2 = (int)(p2.x + 0.5f), x3 = (int)(p3.x + 0.5f);
        int y1 = (int)(p1.y + 0.5f), y2 = (int)(p2.y + 0.5f), y3 = (int)(p3.y + 0.5f);
        drawLine(x1, y1, x2, y2, drawColor);
        drawLine(x2, y2, x3, y3, drawColor);
        drawLine(x1, y1, x3, y3, drawColor);
    }
    else
    {
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
