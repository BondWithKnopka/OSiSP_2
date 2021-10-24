#pragma once
#include <windows.h>
#include <math.h>
#include <string>
#include <time.h>
#include <fstream>
#include <shellscalingapi.h>
#include <iostream>

#define DELTA 3

using namespace std;

int rows = 10;
int columns = 5;
string cell_text = "On my work they always call me 007: 0 motivation, 0 will to work, 7 smoke breaks per hour";

HFONT generateFont()
{
	int fnWeight = 400;
	DWORD fdwItalic = FALSE;
	DWORD fdwUnderline = FALSE;

	DWORD fontFamily = FF_DECORATIVE;
	return CreateFont(0, 0, 0, 0, fnWeight, fdwItalic, fdwUnderline, FALSE, FALSE,
		FALSE, FALSE, FALSE, DEFAULT_PITCH | fontFamily, NULL);
}

int GetBlockHeight(HDC hdc, int currentLine, int width)
{
	RECT nonDrawableBlock;
	nonDrawableBlock.left = 0;
	nonDrawableBlock.top = 0;
	nonDrawableBlock.bottom = 1;
	nonDrawableBlock.right = width;
	int height = (int)DrawText(hdc, cell_text.c_str(), cell_text.length(), &nonDrawableBlock,
		DT_WORDBREAK | DT_CENTER | DT_CALCRECT | DT_EDITCONTROL) + DELTA;
	return height;
}


void DrawTextBlock(HDC hdc, int left, int top, int width, int height, int raw, int column)
{
	RECT drawableBlock;
	drawableBlock.left = left;
	drawableBlock.top = top + DELTA;
	drawableBlock.bottom = top + height;
	drawableBlock.right = left + width - DELTA;

	DrawText(hdc, cell_text.c_str(),
		cell_text.length(),
		&drawableBlock,
		DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER);
}


void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}


void DrawTable(HDC hdc, int width, int height)
{
	int blockWidth = (int)width / columns;
	int currentTop = 0;
	HFONT newFont = generateFont();
	SelectObject(hdc, newFont);

	for (int i = 0; i < rows; i++)
	{
		int blockHeigth = (int)GetBlockHeight(hdc, i, blockWidth + DELTA);

		for (int j = 0; j < columns; j++)
		{
			DrawTextBlock(hdc, (int)j * blockWidth, currentTop, blockWidth, blockHeigth, i, j);
			DrawLine(hdc, (j + 1) * blockWidth - 2, currentTop, (j + 1) * blockWidth - 2, blockHeigth + currentTop);
		}

		currentTop += blockHeigth; //moving down
		DrawLine(hdc, 0, currentTop, blockWidth * columns, currentTop);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int width = 0;
	static int height = 0;

	HDC hdc;
	PAINTSTRUCT ps;
	RECT window = {};

	switch (message)
	{
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		UpdateWindow(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawTable(hdc, width, height);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = "TableBuilderClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow("TableBuilderClass", "Formattable table",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}