#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

int main()
{
	HDC hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	if (hdc == NULL) {
		printf("Couldn't create device context\n");
		return 1;
	}

	DWORD dwWidth, dwHeight, dwBPP, dwNumColors;
	//dwWidth  = GetDeviceCaps(hdc, HORZRES);
	//dwHeight = GetDeviceCaps(hdc, VERTRES);
	dwWidth = 1280;
	dwHeight = 1024;
	dwBPP = GetDeviceCaps(hdc, BITSPIXEL);
	if (dwBPP <= 8) {
		dwNumColors = GetDeviceCaps(hdc, NUMCOLORS);
		dwNumColors = 256;
	}
	else {
		dwNumColors = 0;
	}

	// Create compatible DC.
	HDC hdc2 = CreateCompatibleDC(hdc);
	if (hdc2 == NULL) {
		DeleteDC(hdc);
		printf("Couldn't create compatible device context\n");
		return 1;
	}

	// Create bitmap.
	LPVOID pBits;
	HBITMAP bitmap;
	BITMAPINFO bmInfo;

	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = dwWidth;
	bmInfo.bmiHeader.biHeight = dwHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = (WORD)dwBPP;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biXPelsPerMeter = 0;
	bmInfo.bmiHeader.biYPelsPerMeter = 0;
	bmInfo.bmiHeader.biClrUsed = dwNumColors;
	bmInfo.bmiHeader.biClrImportant = dwNumColors;

	bitmap = CreateDIBSection(hdc, &bmInfo, DIB_PAL_COLORS, &pBits, NULL, 0);
	if (bitmap == NULL) {
		DeleteDC(hdc);
		DeleteDC(hdc2);
		printf("Couldn't create compatible bitmap\n");
		return 1;
	}

	HGDIOBJ gdiobj = SelectObject(hdc2, (HGDIOBJ)bitmap);
	if ((gdiobj == NULL) || (gdiobj == (void*)(LONG_PTR)GDI_ERROR)) {
		DeleteDC(hdc);
		DeleteDC(hdc2);
		printf("Couldn't select bitmap\n");
		return 1;
	}
	if (!BitBlt(hdc2, 0, 0, dwWidth, dwHeight, hdc, 0, 0, SRCCOPY)) {
		DeleteDC(hdc);
		DeleteDC(hdc2);
		printf("Could not copy bitmap\n");
		return 1;
	}

	RGBQUAD colors[256];
	if (dwNumColors != 0)
		dwNumColors = GetDIBColorTable(hdc2, 0, dwNumColors, colors);

	// Fill in bitmap structures.
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bitmapinfoheader;

	bmfh.bfType = 0x04D42;
	bmfh.bfSize = ((dwWidth * dwHeight * dwBPP) / 8) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwNumColors * sizeof(RGBQUAD));
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwNumColors * sizeof(RGBQUAD));
	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfoheader.biWidth = dwWidth;
	bitmapinfoheader.biHeight = dwHeight;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = (WORD)dwBPP;
	bitmapinfoheader.biCompression = BI_RGB;
	bitmapinfoheader.biSizeImage = 0;
	bitmapinfoheader.biXPelsPerMeter = 0;
	bitmapinfoheader.biYPelsPerMeter = 0;
	bitmapinfoheader.biClrUsed = dwNumColors;
	bitmapinfoheader.biClrImportant = 0;

	ofstream file;
	file.open("image.bmp", ios::binary | ios::trunc | ios::out);
	file.write((char*)&bmfh, sizeof(BITMAPFILEHEADER));
	file.write((char*)&bitmapinfoheader, sizeof(BITMAPINFOHEADER));

	if (dwNumColors != 0)
		file.write((char*)colors, sizeof(RGBQUAD)*dwNumColors);
	file.write((char*)pBits, (dwWidth*dwHeight*dwBPP) / 8);

	// Done!
	DeleteObject(bitmap);
	DeleteDC(hdc2);
	DeleteDC(hdc);

	printf("Bitmap captured to disk file\n");
	system("pause");
	return 0;
}
