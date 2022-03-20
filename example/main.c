#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN    0x0201
#define WM_RBUTTONDOWN    0x0204

#include <windows.h>
#include <stdio.h>
#include <windowsx.h>


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK childWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    const wchar_t CLASS_NAME[]  = L"ParentWindow";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"ParentWindow",    // Window text
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
        return 0;
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
        case WM_CLOSE:
            DestroyWindow(hwnd);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
        {
            const wchar_t childClassName[] = L"Child Window Class";
            HWND checkHandle = FindWindowEx(hwnd, NULL, NULL, NULL);
            if (checkHandle != NULL) {
                DestroyWindow(checkHandle);
            }

                WNDCLASS wc = {};

                wc.lpfnWndProc = childWindowProc;
                wc.hInstance = NULL;
                wc.lpszClassName = childClassName;

                RegisterClass(&wc);
                POINT p;
                p.x = GET_X_LPARAM(lParam);
                p.y = GET_Y_LPARAM(lParam);

                HWND hwndChild = CreateWindowEx(0, childClassName, L"Coordinates", WS_CHILD  | WS_OVERLAPPEDWINDOW,
                                                p.x, p.y, 200, 100, hwnd, NULL, NULL, NULL);

                if (hwndChild == NULL) {
                    return 0;
                }

                ShowWindow(hwndChild, SW_SHOWNORMAL);

        }
            return 0;
        case WM_RBUTTONDOWN:
        {
            if (FindWindowEx(hwnd, NULL, NULL, NULL) != NULL) {

                HWND hwndChild = FindWindowEx(hwnd, NULL, NULL, NULL);

                if (hwndChild == NULL) {
                    printf("No windows found");
                    return 0;
                }
                POINT point, nextPos;

                GetCursorPos(&point);

                RECT rect, childRect;
                GetWindowRect(hwnd, &rect);
                GetWindowRect(hwndChild, &childRect);


                POINT center;
                center.x = rect.left + (rect.right-rect.left)/2;
                center.y = rect.top + (rect.bottom-rect.top)/2;
                ScreenToClient(hwnd, &center);
                ScreenToClient(hwnd, &point);
                POINT   q1, q3;

                q1.x = center.x + center.x / 2;
                q1.y = center.y / 2;
                q3.x = center.x / 2;;
                q3.y = center.y + center.y / 2;

                if (point.x > center.x)
                    nextPos.x = q3.x;
                else
                    nextPos.x = q1.x;

                if (point.y > center.y)
                    nextPos.y = q1.y;
                else
                    nextPos.y = q3.y;

                nextPos.x -= (childRect.right - childRect.left)/2;
                nextPos.y -= (childRect.bottom - childRect.top)/2;
                MoveWindow(hwndChild, nextPos.x, nextPos.y, 200, 100, TRUE);

            }
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+9));

            EndPaint(hwnd, &ps);
        }
            return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK childWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            POINT point;
            GetCursorPos(&point);
            ScreenToClient(GetParent(hwnd), &point);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            char letter[256];
            sprintf(letter, "X = %ld, Y = %ld", point.x, point.y);
            TextOutA(hdc, CW_USEDEFAULT, CW_USEDEFAULT, letter, strlen(letter));
            EndPaint(hwnd, &ps);
            ReleaseDC(hwnd, hdc);

            EndPaint(hwnd, &ps);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}