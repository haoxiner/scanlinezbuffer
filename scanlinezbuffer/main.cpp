#include <Windows.h>
#include <cstdint>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static TCHAR szAppName[] = TEXT("scanlinezbuffer");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

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
		1024,
		768,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg,nullptr,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	static BITMAPINFOHEADER bmih;
	static int32_t *pBits;
	static BITMAP bitmap;
	static HDC screenDC;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = 1024;
		bmih.biHeight = 768;
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
		ReleaseDC(hwnd,hdc);
		hBitmap = CreateDIBSection(screenDC, (BITMAPINFO*)&bmih, 0, (void**)&pBits, nullptr, 0);
		SelectObject(screenDC, hBitmap);

		memset(pBits, 0, 1024 * 768 * 4);
		for (size_t i = 0; i < 1024*768/2; i++)
		{
			pBits[i] = (255 << 16);
		}
		return 0;
	case WM_PAINT:
		hdc = GetDC(hwnd);
		BitBlt(hdc, 0, 0, 1024, 768, screenDC, 0, 0, SRCCOPY);
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
