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
HBRUSH hbr1, hbr2;
HICON hIcon1, hIcon2;
int playerTurn =1;
int gameBoard[9] = { 0,0,0,0,0,0,0,0,0 };
int winner = 0;
int wins[3];

//Gets Game Board size
BOOL GetGameBoardRect(HWND hwnd, RECT * pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))
	{
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
	POINT pt = { x, y };
	RECT rc;

	if (GetGameBoardRect(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//user clicked inside game board
			//Normalize ( 0 to 3*CELL_SIZE)
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			//convert to index ( 0 to 8)
			return column + row * 3;
		}
	}
	return -1;	//outside game board or failure
}

void whiteBrush()
{
	(HBRUSH)GetStockObject(WHITE_BRUSH);
}

//Returns:
// 0 - no winner
// 1 - player 1 wins
// 2 - player 2 wins
// 3 - draw
int GetWinner(int wins[3])
{
	//list of possible wining combinations
	int cells[] = {0,1,2   ,3,4,5,  6,7,8,  0,3,6,  1,4,7,  2,5,8,  0,4,8,  2,4,6};

	
	for (int i = 0; i < ARRAYSIZE(cells); i+=3)
	{
		if ((0!=gameBoard[cells[i]]) && gameBoard[cells[i]] == gameBoard[cells [i+ 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			//We have a winner
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}

	//see if any cells are left open
	for (int i = 0; i < ARRAYSIZE(gameBoard); i++)
	{
		if (gameBoard[i] == 0)
		{
			//Continue to play
			return 0;
		}
		return 3;
	}
}

void DrawIconCentered(HDC hdc, RECT * pRect, HICON hIcon)
{
	const int ICON_WIDTH = 32;
	const int ICON_HEIGHT = 32;
	if (NULL != pRect)
	{
		int left = pRect->left + (pRect->right - pRect->left - ICON_WIDTH)/2;
		int top = pRect->top + ((pRect->bottom - pRect->top)- ICON_HEIGHT)/2;
		DrawIcon(hdc, left, top, hIcon);
	}
}

//Get cell dimensions
BOOL GetCellRect(HWND hWnd, int index, RECT * pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
		return FALSE;

	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		//Convert index from 0 to 8 into x,y pair
		int y = index / 3;  //Row number
		int x = index % 3;	// Column number

		pRect->left = rcBoard.left + x * CELL_SIZE + 1;
		pRect->top = rcBoard.top + y * CELL_SIZE + 1;
		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;

		return TRUE;
	}

	return FALSE;
}


void ShowTurn(HWND hWnd, HDC hdc)
{
	static const WCHAR szTurn1[] = L"Turn: Player 1";
	static const WCHAR szTurn2[] = L"Turn: Player 2";
	RECT rc;
	const WCHAR * pszTurnText = NULL;

	switch (winner)
	{
	case 0: //continue to play
		pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
		break;
	case 1: //Player 1 wins
		pszTurnText = L"Player 1 is the winner!! WOW!";
		break;
	case 2: //Player 2 wins
		pszTurnText = L"Player 2 is the winner!! WOW!";
		break;
	case 3: //Its a draw
		pszTurnText = L"It's a draw!!";
		break;
	}


	if (NULL != GetClientRect(hWnd, &rc))
	{
		rc.top = rc.bottom - 48; 
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		//Create red brush
		hbr1 = CreateSolidBrush(RGB(255, 0, 0));
		//Create blue brush
		hbr2 = CreateSolidBrush(RGB(0, 0, 255));

		//Load Player Icons
		hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
		hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_NEWGAME:
		{
			int ret = MessageBox(hWnd, L"Are you sure you would like to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
			//if user wants to start a new game:
			if (IDYES == ret)
			{
				//reset game
				playerTurn = 1;
				winner = 0;
				ZeroMemory(gameBoard, sizeof(gameBoard));

				//force a paint message
				InvalidateRect(hWnd, NULL, TRUE); //post a WM_PAINT to our windowProc, its put in the message queue
				UpdateWindow(hWnd); //forces immediate handling of WM_Paint
			}
		}
		break;

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

		//only handle clicks if it is a player turn.  (ie: 1 or 2)
		if (playerTurn == 0)
		{
			break;
		}

		int index = GetCellNumberFromPoint(hWnd, xPos, yPos);
		
		HDC hdc = GetDC(hWnd);
		if (hdc != NULL)
		{
			//__Output Cell index on click
			//WCHAR temp[100];
			//wsprintf(temp, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));
			

			//Get cell dimension from its index
			if (index != -1)
			{
				RECT rcCell;
				if ((gameBoard[index] == 0) && GetCellRect(hWnd, index, &rcCell))
				{
					//make cell 'unselectable'
					gameBoard[index] = playerTurn;

					//FillRect(hdc, &rcCell, (playerTurn==1) ? hbr1 : hbr2);
					DrawIconCentered(hdc, &rcCell, (playerTurn == 1) ? hIcon1 : hIcon2);

					//Check for winner
					winner = GetWinner(wins);
					if (winner == 1 || winner == 2)
					{
						//we have a winner, display message box
						MessageBox(hWnd, (winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!",
							L"You Win!", MB_OK | MB_ICONINFORMATION);
						playerTurn = 0;
					}
					else if(winner == 3)
					{
						//its a draw	
						MessageBox(hWnd,L"Cats game! No one wins this time!",L"Its a Draw!", MB_OK | MB_ICONEXCLAMATION);
						playerTurn = 0;
					}
					else if (winner == 0)
					{
						//Alternate player turn between one and two
						playerTurn = (playerTurn == 1) ? 2 : 1;
					}
					//Display Turn
					ShowTurn(hWnd, hdc);
				}
			}
			ReleaseDC(hWnd, hdc);
		}
		

	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;
		if (pMinMax != NULL)
		{
			pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
		}
	}
	break;

	//This builds/draws the window
	case WM_PAINT:
	{
		//This builds/draws the window
		PAINTSTRUCT ps;
		//define window?
		HDC hdc = BeginPaint(hWnd, &ps);
		//defeine rectangle
		RECT rc;

		// TODO: Add any drawing code that uses hdc here...
		if (GetGameBoardRect(hWnd, &rc))
		{
			RECT rcClient;
		
			//Display player text and turn
			
			if (GetClientRect(hWnd, &rcClient))
			{
				const WCHAR szPlayer1 [] = L"Player 1";
				const WCHAR szPlayer2 [] = L"Player 2";	

				SetBkMode(hdc, TRANSPARENT);

				
					//Draw player 1 and player 2
				SetTextColor(hdc, RGB(255, 255, 0));
				TextOut(hdc, 16, 16, szPlayer1, ARRAYSIZE(szPlayer1));
				SetTextColor(hdc, RGB(0, 0, 255));
				TextOut(hdc, rcClient.right - 72, 16, szPlayer2, ARRAYSIZE(szPlayer2));

				//Display turn
				ShowTurn(hWnd, hdc);
			}


			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)); // <- Draws white square with no border
			
			for (int i = 0; i < 4; i++)
			{

				//Draw Vertical lines
				DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);

				//Draw Horizontal lines
				DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
			}

			//refill boards colored cells on window resize
			//draw all occupied cells
			RECT rcCell;
			for (int i = 0; i < ARRAYSIZE(gameBoard); i++)
			{
				if ((gameBoard[i] != 0)&&GetCellRect(hWnd, i, &rcCell))
				{
					//FillRect(hdc, &rcCell, (gameBoard[i] == 2) ? hbr2 : hbr1);
					DrawIconCentered(hdc, &rcCell, (gameBoard[i] == 1) ? hIcon1 : hIcon2);
				}
			}
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		//clear brushes from memory
		DeleteObject(hbr1);
		DeleteObject(hbr2);
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
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
