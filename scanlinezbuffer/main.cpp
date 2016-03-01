#include "Rasterizer.h"
#include "Scene.h"
#include "Camera.h"

#include <Windows.h>
#include <cstdint>

static unsigned const int xResolution = 1024, yResolution = 768;

static int32_t *pBits;
static Rasterizer renderer;
static Scene scene;
static Camera camera;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitializeDevice(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	static TCHAR szAppName[] = TEXT("scanlinezbuffer");

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = nullptr;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		TEXT("Demo: scanline_zbuffer"),
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		xResolution,
		yResolution,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	InitializeDevice(hwnd);

	while (GetMessage(&msg,nullptr,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		renderer.Render(scene, camera, pBits);
	}
	return msg.wParam;
}

static HBITMAP hBitmap;
static BITMAPINFOHEADER bmih;
static HDC screenDC;
void InitializeDevice(HWND hwnd)
{
	HDC hdc;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = xResolution;
	bmih.biHeight = yResolution;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 0;
	bmih.biYPelsPerMeter = 0;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;
	hdc = GetDC(hwnd);
	screenDC = CreateCompatibleDC(hdc);
	ReleaseDC(hwnd, hdc);
	hBitmap = CreateDIBSection(screenDC, (BITMAPINFO*)&bmih, 0, (void**)&pBits, nullptr, 0);
	SelectObject(screenDC, hBitmap);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = GetDC(hwnd);
		BitBlt(hdc, 0, 0, xResolution, yResolution, screenDC, 0, 0, SRCCOPY);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_DESTROY:
		DeleteDC(screenDC);
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}
