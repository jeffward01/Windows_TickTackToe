// TickTackToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TickTackToe.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TICKTACKTOE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICKTACKTOE));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICKTACKTOE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1); // <-- Makes a white background
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TICKTACKTOE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
//Global varibles for game
const int CELL_SIZE = 100;

//Gets Game Board size
BOOL GetGameBoardRect(HWND hwnd, RECT * pRect)
{
	RECT rc;

	// TODO: Add any drawing code that uses hdc here...
	if (GetClientRect(hwnd, &rc))
	{
		//This creates and centers the game board
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;

		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;
		return TRUE;
	}

	SetRectEmpty(pRect);
	return FALSE;
}

//Draws line
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}
	
//Click handler for game board/Cell positon
int GetCellNumberFromPoint(HWND hwnd, int x, int y)
{
	RECT rc;
	POINT pt = { x ,y };
		
	if (GetGameBoardRect(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//User clicked inside game board
			//Normalize ( 0 to 3 * CELL_SIZE)
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			//convert to index ( 0 to 8 )
			return column + row * 3;
		}
	}
	return -1; //click outside the board or failure
}

//Get cell dimensions
BOOL GetCellRect(HWND hWnd, int index, RECT * pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
	{
		return FALSE;
	}

	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		int y = index / 3; //Row number
		int x = index % 3; // Column Number


		pRect->left = rcBoard.left + x * CELL_SIZE;
		pRect->top = rcBoard.top + y * CELL_SIZE;
		pRect->right = pRect->left + CELL_SIZE;
		pRect->bottom = pRect->top + CELL_SIZE;

		return TRUE;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	//This handles mouse clicks
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int index = GetCellNumberFromPoint(hWnd, xPos, yPos);
		
		HDC hdc = GetDC(hWnd);
		if (hdc != NULL)
		{
			WCHAR temp[100];

			wsprintf(temp, L"Index = %d", index);
			TextOut(hdc, xPos, yPos, temp, lstrlen(temp));
			

			//Get cell dimension from its index
			if (index != -1)
			{
				RECT rcCell;
				if (GetCellRect(hWnd, index, &rcCell))
				{
					FillRect(hdc, &rcCell, (HBRUSH)GetStockObject(BLACK_BRUSH));
				}
			}
			ReleaseDC(hWnd, hdc);
		}
		

	}
	case WM_GETMINMAXINFO:
	{
		//This prevents the window from being sized smalled than a 5x5
		MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;
		pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
		pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
	}
	break;

	case WM_PAINT:
	{
		//This builds/draws the window
		PAINTSTRUCT ps;
		//define window?
		HDC hdc = BeginPaint(hWnd, &ps);
		//defeine rectangle?
		RECT rc;

		// TODO: Add any drawing code that uses hdc here...
		if (GetGameBoardRect(hWnd, &rc))
		{
			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)); // <- Draws white square with no border
			//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);  //draw white rectangle with black border
		}

		for (int i = 0; i < 4; i++)
		{

			//Draw Vertical lines
			DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);

			//Draw Horizontal lines
			DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
		}

		EndPaint(hWnd, &ps);
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
