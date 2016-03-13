#include "Rasterizer.h"
#include "Scene.h"
#include "Camera.h"

#include <Windows.h>
#include <cstdint>

static const unsigned int xResolution = 1024, yResolution = 768;
static float ratio;

static const float MAX_DISTANCE = 5000;

static int32_t *pBits;
static Rasterizer *renderer;
static Scene scene;
static Camera camera;
static bool draw;

static HBITMAP hBitmap;
static BITMAPINFOHEADER bmih;
static HDC screenDC;

static float distance;
static float angle;
static float phi;
static bool rotate;
static POINT previousMousePosition;
static POINT mousePosition;
static float x = 0, z = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitializeDevice(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	scene.LoadConfig("./config.txt");
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
	//UpdateWindow(hwnd);

	renderer = new Rasterizer(xResolution, yResolution);
	InitializeDevice(hwnd);

	//camera.LookAt(Point(scene.center.x+100,scene.center.y,scene.center.z + 100), Vector(-1, 0, -1), Vector(0, 1, 0));
	ratio = static_cast<float>(xResolution) / static_cast<float>(yResolution);
	camera.Frustum(-ratio, ratio, 1, -1, 10.0f, MAX_DISTANCE);
	
	draw = true;
	
	phi = 0;
	float width = scene.boxMax.x - scene.boxMin.x;
	float height = scene.boxMax.y - scene.boxMin.y;
	distance = std::fmaxf(width * 10 / 2 / ratio, height * 10);

	while (GetMessage(&msg,nullptr,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		x = distance*std::sinf(phi);
		z = distance*std::cosf(phi);
		camera.LookAt(Point(scene.center.x + x, scene.center.y, scene.center.z + z), Vector(scene.center.x - x, 0, scene.center.z - z), Vector(0, 1, 0));
		renderer->Render(scene, camera, pBits);
		if (draw)
		{
			HDC hdc = GetDC(hwnd);
			BitBlt(hdc, 0, 0, xResolution, yResolution, screenDC, 0, 0, SRCCOPY);
			ReleaseDC(hwnd, hdc);
		}
	}
	return msg.wParam;
}


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
		draw = false;
		return 0;
	case WM_LBUTTONDOWN:
		rotate = true;
		return 0;
	case WM_LBUTTONUP:
		rotate = false;
		return 0;
	case WM_MOUSEMOVE:
		mousePosition.x = LOWORD(lParam);
		mousePosition.y = HIWORD(lParam);
		if (rotate)
		{
			int speed = mousePosition.x - previousMousePosition.x;
			phi += 0.002*speed;
		}
		previousMousePosition = mousePosition;
		return 0;
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) > 0)
		{
			if (distance > 5)
			{
				distance -= 5;
			}
		}
		else
		{
			if (distance <= MAX_DISTANCE - 5)
			{
				distance += 5;
			}
		}
		return 0;
	case WM_KEYDOWN:
		scene.switchNext();
		phi = 0;
		distance = std::fmaxf((scene.boxMax.x - scene.boxMin.x) * 10 / 2 / ratio, (scene.boxMax.y - scene.boxMin.y) * 10);
		return 0;
	case WM_DESTROY:
		DeleteDC(screenDC);
		PostQuitMessage(0);
		return 0;
	default:
		draw = true;
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}
