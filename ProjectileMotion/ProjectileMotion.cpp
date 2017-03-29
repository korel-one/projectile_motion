// GdiPlus.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <tchar.h>
#include <wchar.h>
#include <gdiplus.h>
#include <vector>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include <ios>
#include <cstdio>
#include <io.h>
#include <fcntl.h>

#include <IModel.h>

#include "InputWatcher.h"
#include "LibraryManager.h"

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#include "Draw.h"

static LibraryLoadManager library_load_manager;
static IModel* p_flight_model = nullptr;

using FLIGHT_MODEL_FACTORY = IModel& (APIENTRY*)();

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void RedirectIOToConsole()
{
    BOOL f = AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

int APIENTRY wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	MyRegisterClass(hInstance);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Perform application initialization:
	HWND hWnd;
	if (!InitInstance(hInstance, nCmdShow, hWnd))
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	//init console output
    RedirectIOToConsole();

	std::wstring dll_dir(_T("..\\..\\bin\\"));
#ifdef _DEBUG
	dll_dir.append(_T("Debug\\CalcDll"));
#else
	dll_dir.append(_T("Release\\CalcDll"));
#endif

	if (!library_load_manager.load_dll(dll_dir.c_str())) {
		std::cerr << "unable to load Dll\n";
		return 1;
	}

	p_flight_model = library_load_manager.get_instance("flight_model_instance", FLIGHT_MODEL_FACTORY());
	if (!p_flight_model) {
		std::cerr << "unable to load \"flight_model_instance\" function from DLL\n";
		return 1;
	}

	std::thread(&InputWatcher::run, InputWatcher(p_flight_model, [hWnd](){::PostMessage(hWnd, WM_QUIT, 0, 0); })).detach();
    p_flight_model->run_trajectory_calculator(100, [hWnd]() {
        ::InvalidateRect(hWnd, nullptr, TRUE);
    });

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, 0, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"GDIPLUS";
	wcex.hIconSm = 0;

	return RegisterClassEx(&wcex);
}

//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
	hWnd = CreateWindowW(L"GDIPLUS"
		, L"ProjectileMotion"
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, 0
		, 1000
		, 800
		, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

float get_scale(float max_data_x, float max_data_y, float max_draw_region_x,
	float max_draw_region_y) {

	float log_x = log10(max_draw_region_x / max_data_x);
	float log_y = log10(max_draw_region_y / max_data_y);

	float scale = pow(10.f, floor(std::min<float>(log_x, log_y)));

	if (5 * scale*max_data_x < max_draw_region_x && 5 * scale*max_data_y < max_draw_region_y) return 5 * scale;
	if (2 * scale*max_data_x < max_draw_region_x && 2 * scale*max_data_y < max_draw_region_y) return 2 * scale;
	return scale;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_PAINT:
	{
					 RECT rc;
					 GetClientRect(hWnd, &rc);

					 Gdiplus::Point cs_origin = { 100, rc.bottom - rc.top - 100 };

					 int max_x = rc.right - rc.left - cs_origin.X - 50
						 , max_y = cs_origin.Y - 50;

					 PAINTSTRUCT ps;
					 HDC hDC = BeginPaint(hWnd, &ps);
					 Graphics graphics(hDC);

					 Draw::coord_sys(graphics, cs_origin, max_x, max_y);

					 if (p_flight_model != nullptr) {

						 FlightData curr_flight_data;
						 p_flight_model->clone_flight_data_to(curr_flight_data);

						 auto& points = curr_flight_data.trajectory;
						 if (!points.empty()) {
							 auto scale = get_scale(curr_flight_data.range, curr_flight_data.max_altitude
								 , (float)max_x, (float)max_y);

							 Draw::axis_gradation(graphics, cs_origin
								 , curr_flight_data.range, curr_flight_data.max_altitude, scale);

							 Draw::trajectory(graphics, points, scale, cs_origin);
						 }
					 }
					 EndPaint(hWnd, &ps);
	}
		break;

	case WM_GETMINMAXINFO:
	{

							 // set the MINMAXINFO structure pointer 
							 auto lpMinMaxInfo = (MINMAXINFO FAR *) lParam;
							 lpMinMaxInfo->ptMinTrackSize.x = 200;
							 lpMinMaxInfo->ptMinTrackSize.y = 200;
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}