#pragma once

#include "resource.h"
#include <memory>

namespace SoftRaster {
	int g_width = 0;
	int g_height = 0;

	HDC g_tempDC = nullptr;
	HBITMAP g_tempBM = nullptr;
	HBITMAP g_oldBM = nullptr;
	unsigned int* g_frameBuffer = nullptr;
	std::shared_ptr<float[]> g_depthBuffer = nullptr;

	unsigned int bgColor = ((123 << 16) | (195 << 8) | 221);	//一个浅蓝色背景

	//初始化
	void initRenderer(int w, int h, HWND hWnd);

	void update(HWND hWnd);

	void clearBuffer();

	void shutDown();
}

void SoftRaster::initRenderer(int w, int h, HWND hWnd) {
	g_width = w;
	g_height = h;

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