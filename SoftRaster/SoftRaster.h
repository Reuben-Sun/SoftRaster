#pragma once

#include "resource.h"
#include <memory>
#include "Camera.h"
#include "Draw.h"

namespace SoftRaster {
	int g_width = 0;
	int g_height = 0;

	HDC g_tempDC = nullptr;
	HBITMAP g_tempBM = nullptr;
	HBITMAP g_oldBM = nullptr;
	unsigned int* g_frameBuffer = nullptr;
	std::shared_ptr<float[]> g_depthBuffer = nullptr;

	unsigned int bgColor = ((123 << 16) | (195 << 8) | 221);	//一个浅蓝色背景

	//创建一个相机
	Camera camera(
		{ 5.0f, 5.0f, -5.0f, 1.0f },  // Pos
		{ 0.0f, 0.0f, 0.0f, 1.0f },   // LookAt
		{ 0.0f, 1.0f, 0.0f, 0.0f }    // 摄像机上方向
	);

	// 目标立方体8个顶点 摄像机方向
	std::vector<vertex> vertexes = {
		// 近相机面
		{{-1.0f, +1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}},
		{{+1.0f, +1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 0.0f}},
		{{+1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f}},

		// 远相机面
		{{-1.0f, +1.0f, +1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f}},
		{{+1.0f, +1.0f, +1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}},
		{{+1.0f, -1.0f, +1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f, +1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f}}
	};


	//初始化
	void initRenderer(int w, int h, HWND hWnd);

	void update(HWND hWnd);

	void clearBuffer();

	void shutDown();

	//鼠标事件
	void onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);
}

void SoftRaster::initRenderer(int w, int h, HWND hWnd) {
	g_width = w;
	g_height = h;


	camera.setPerspectiveForLH(
		3.1415926f * 0.25f,       // 上下45度视野
		(float)w / (float)h,	// 长宽比
		1.0f,		//近裁剪平面
		200.0f		//远裁剪平面
	);

	//创建一个屏幕缓冲
	HDC hDC = GetDC(hWnd);
	g_tempDC = CreateCompatibleDC(hDC);	//创建一个和当前设备兼容的DC
	ReleaseDC(hWnd, hDC);

	//创建该DC的bitmap缓冲，32位
	BITMAPINFO bi = { {sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, (DWORD)w * h * 4, 0, 0, 0, 0} };
	g_tempBM = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&g_frameBuffer, 0, 0);

	//选择该bitmap到DC中
	g_oldBM = (HBITMAP)SelectObject(g_tempDC, g_tempBM);

	//创建深度缓冲区
	g_depthBuffer.reset(new float[w * h]);

	//清空屏幕缓冲
	clearBuffer();	
}

void SoftRaster::update(HWND hWnd) {
	clearBuffer();

	//绘制内容
	drawCube();

	//将frameBuffer呈现到屏幕上
	HDC hDC = GetDC(hWnd);
	BitBlt(hDC, 0, 0, g_width, g_height, g_tempDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

void SoftRaster::clearBuffer() {
	for (int row = 0; row < g_height; row++) {
		for (int col = 0; col < g_width; col++) {
			int index = row * g_width + col;
			g_frameBuffer[index] = bgColor;		//颜色缓冲设为默认颜色
			g_depthBuffer[index] = 1.0f;		//深度缓冲设为1
		}
	}
}

void SoftRaster::shutDown() {
	if (g_tempDC) {
		if (g_oldBM) {
			SelectObject(g_tempDC, g_oldBM);
			g_oldBM = nullptr;
		}
		DeleteDC(g_tempDC);
		g_tempDC = nullptr;
	}
	if (g_tempBM) {
		DeleteObject(g_tempBM);
		g_tempBM = nullptr;
	}
}

void SoftRaster::onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	static bool isPressed = false;
	static LPARAM lpCur = 0;
	switch (message)
	{
	case WM_MOUSEWHEEL:
		camera.zoom(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case WM_LBUTTONDOWN:
		isPressed = true;
		lpCur = lParam;
		break;
	case WM_LBUTTONUP:
		isPressed = false;
		break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			short xMove = LOWORD(lParam) - LOWORD(lpCur);
			short yMove = HIWORD(lParam) - HIWORD(lpCur);
			lpCur = lParam;
			camera.circle(xMove, yMove);
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
			camera.reset();
		else if (wParam == VK_F1)
		{
			if (g_renderMode == RenderMode::RENDER_COLOR) g_renderMode = RenderMode::RENDER_WIREFRAME;
			else g_renderMode = RenderMode::RENDER_COLOR;
		}
		break;
	default:
		break;
	}
}