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


}