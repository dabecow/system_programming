#ifndef UNICODE
#define UNICODE
#endif

#define WIDTH 200
#define HEIGHT 150
#define STEP 10
#define TIMER_ID 1234
#define TIMER_ELAPSE 30
#define PATH_SQUARE_LENGTH 400
#include <windows.h>
#include <stdio.h>
#include <windowsx.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitPens();

void ClientDraw(HWND hwnd, WPARAM wParam, LPARAM lParam);
void ClientHdcDraw(HDC hdc, PAINTSTRUCT ps);

void DrawRectangle(HDC hdc, POINT topLeft, LONG width, LONG height);
void DrawCrossLines(HDC hdc, POINT topLeft, LONG width, LONG height);

void ProcessKey(hwnd, wParam, lParam);
void ProcessMouse(HWND hwnd, WPARAM wParam, LPARAM lParam);

void NextMoveStep(HWND hwnd);

enum States {
    MS_DOWN,
    MS_TOPRIGHT,
    MS_BOTRIGHT,
    MS_UP,
    MS_LEFT 
};

int currentState = MS_DOWN;

HGDIOBJ redDottedPen;
HGDIOBJ grayDashedPen;
POINT rectangleTopLeftPoint = {.x = 0, .y = 0};


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

    SetTimer(hwnd, TIMER_ID, TIMER_ELAPSE, NULL);

    // Run the message loop.

    MSG msg = { };

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(hwnd, TIMER_ID);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE: 
            InitPens();
            break;
        
        case WM_CLOSE:
            DestroyWindow(hwnd);

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
       
        case WM_PAINT:
            ClientDraw(hwnd, wParam, lParam);
            break;
        
        case WM_TIMER:
            if (wParam == TIMER_ID)
                NextMoveStep(hwnd);
            break;

        case WM_KEYDOWN: 
            ProcessKey(hwnd, wParam, lParam);
            break;

        case WM_MOUSEMOVE: 
            ProcessMouse(hwnd, wParam, lParam);
            break;

        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ClientDraw(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    HDC hdc = BeginPaint(hwnd, &ps);
    HDC hCmpDC = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    SelectObject (hCmpDC, hBmp);

    int saveDC = SaveDC(hCmpDC);

    ClientHdcDraw(hCmpDC, ps);
    
    SetStretchBltMode(hdc, COLORONCOLOR);
    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hCmpDC, 0, 0, SRCCOPY);
    RestoreDC(hCmpDC, saveDC);
    DeleteDC(hCmpDC);
    DeleteObject(hBmp);

    EndPaint(hwnd, &ps);
}

void ClientHdcDraw(HDC hdc, PAINTSTRUCT ps) {
    FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW));
    DrawRectangle(hdc, rectangleTopLeftPoint, WIDTH, HEIGHT);
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

void ProcessKey(hwnd, wParam, lParam) {
    switch (wParam) {
        case VK_UP:
            rectangleTopLeftPoint.y -= STEP;
            break;

        case VK_DOWN:
            rectangleTopLeftPoint.y += STEP;
            break;
        
        case VK_LEFT:
            rectangleTopLeftPoint.x -= STEP;
            break;
        
        case VK_RIGHT:
            rectangleTopLeftPoint.x += STEP;
            break;
    }

    InvalidateRect(hwnd, NULL, 1);
}

void ProcessMouse(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    if (wParam & MK_SHIFT) {
        rectangleTopLeftPoint.x = lParam % 0x10000;
        rectangleTopLeftPoint.y = lParam / 0x10000;

        InvalidateRect(hwnd, NULL, 1);
    }
}

void NextMoveStep(HWND hwnd) {
    
    switch (currentState) {
        case MS_DOWN:
            if (rectangleTopLeftPoint.y >= PATH_SQUARE_LENGTH)
                currentState = MS_TOPRIGHT;
            else 
                rectangleTopLeftPoint.y+=STEP;
            break;
        case MS_TOPRIGHT:
            if (rectangleTopLeftPoint.x >= PATH_SQUARE_LENGTH / 2 && rectangleTopLeftPoint.y <= PATH_SQUARE_LENGTH - PATH_SQUARE_LENGTH / 4) 
                currentState = MS_BOTRIGHT;
            else {
                rectangleTopLeftPoint.x += STEP;
                rectangleTopLeftPoint.y -= STEP / 2;
            }
            break;
        case MS_BOTRIGHT:
            if (rectangleTopLeftPoint.x >= PATH_SQUARE_LENGTH)
                currentState = MS_UP;
            else {
                rectangleTopLeftPoint.x += STEP;
                rectangleTopLeftPoint.y += STEP/2;
            }
            break;
        case MS_UP:
            if (rectangleTopLeftPoint.y <= 0)
                currentState = MS_LEFT;
            else 
                rectangleTopLeftPoint.y -= STEP;
            break;
        case MS_LEFT:
            if (rectangleTopLeftPoint.x <= 0)
                currentState = MS_DOWN;
            else 
                rectangleTopLeftPoint.x -= STEP;
            break;

        default: currentState = MS_DOWN;
    }

    InvalidateRect(hwnd, NULL, 1);

}