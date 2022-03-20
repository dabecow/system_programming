#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN    0x0201
#define WM_RBUTTONDOWN    0x0204
#define WIDTH 400
#define HEIGHT 300

#include <windows.h>
#include <stdio.h>
#include <windowsx.h>

HGDIOBJ redDottedPen;
HGDIOBJ grayDashedPen;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ClientDraw(HWND hwnd, WPARAM wParam, LPARAM lPara);
void InitPens();
void DrawRectangle(HDC hdc, POINT topLeft, LONG width, LONG height);
void DrawCrossLines(HDC hdc, POINT topLeft, LONG width, LONG height);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    const wchar_t CLASS_NAME[]  = L"ParentWindow";
    const wchar_t WINDOW_NAME[] = L"Lab 1";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            WINDOW_NAME,                    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE: {
            InitPens();
        }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
       
        case WM_PAINT:
        {
            ClientDraw(hwnd, wParam, lParam);
        }
            return 0;
    
        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ClientDraw(HWND hwnd, WPARAM wParam, LPARAM lPara) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    POINT topLeft = {.x = 200, .y = 20};
    DrawRectangle(hdc, topLeft, WIDTH, HEIGHT);
    
    EndPaint(hwnd, &ps);
}

void DrawRectangle(HDC hdc, POINT topLeft, LONG width, LONG height) {
    SelectObject(hdc, redDottedPen);
    Rectangle(hdc, topLeft.x, topLeft.y, topLeft.x + width, topLeft.y + height);
    DrawCrossLines(hdc, topLeft, width, height);
}

void DrawCrossLines(HDC hdc, POINT topLeft, LONG width, LONG height) {
    POINT rectangleWidthCenter = {topLeft.x + width / 2, topLeft.y};
    POINT rectangleHeightCenter = {topLeft.x, topLeft.y + height / 2};
    LONG overlapping = height / 20;

    POINT verticalLineStart = {.x = rectangleWidthCenter.x, .y = topLeft.y - overlapping};
    POINT verticalLineEnd = {.x = rectangleWidthCenter.x, .y = topLeft.y + height + overlapping};
    POINT horizontalLineStart = {.x = rectangleHeightCenter.x - overlapping, .y = rectangleHeightCenter.y};
    POINT horizontalLineEnd = {.x = rectangleHeightCenter.x + width + overlapping, .y = rectangleHeightCenter.y};

    SelectObject(hdc, grayDashedPen);
    MoveToEx(hdc, verticalLineStart.x, verticalLineStart.y, NULL);
    LineTo(hdc, verticalLineEnd.x, verticalLineEnd.y);
    MoveToEx(hdc, horizontalLineStart.x, horizontalLineEnd.y, NULL);
    LineTo(hdc, horizontalLineEnd.x, horizontalLineEnd.y);
}

void InitPens() {
    LOGBRUSH lb;
    lb.lbColor = RGB(255, 0, 0);
    lb.lbStyle = BS_SOLID;
    lb.lbHatch = 0;
    
    redDottedPen = ExtCreatePen(PS_GEOMETRIC | PS_DOT, 4, &lb, 0, NULL);
    lb.lbColor = RGB(50, 50, 50);
    grayDashedPen = ExtCreatePen(PS_GEOMETRIC | PS_DASH, 5, &lb, 0, NULL);
}