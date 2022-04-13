#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN 0x0201
#define WM_RBUTTONDOWN 0x0204
#define WM_COMMAND_PUT 0x1000
#define WM_COMMAND_GET 0x1001

#include <windows.h>
#include <stdio.h>
#include <windowsx.h>

HBITMAP hbitmap;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void putInBuffer(HWND hwnd, LPWSTR filePath);
void openFile(HWND hwnd);
void getFromBuffer(HWND hwnd);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"ParentWindow";
    const wchar_t WINDOW_NAME[] = L"Lab 1";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                   // Optional window styles.
        CLASS_NAME,          // Window class
        WINDOW_NAME,         // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    HWND buttonPut = CreateWindow(L"BUTTON", L"Press me", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 200, 100, hwnd, (HMENU)WM_COMMAND_PUT, hInstance, NULL);
    HWND buttonGet = CreateWindow(L"BUTTON", L"Press me", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 110, 200, 100, hwnd, (HMENU)WM_COMMAND_GET, hInstance, NULL);

    if (hwnd == NULL)
    {
        return -1;
    }
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = {};

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
    case WM_CREATE:{
    }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == WM_COMMAND_PUT)
        {
            openFile(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            putInBuffer(hwnd, NULL);
        }
        else if (LOWORD(wParam) == WM_COMMAND_GET)
        {
            getFromBuffer(hwnd);
        }
        break;
    case WM_PAINT:{
        PAINTSTRUCT     ps;
        HDC             hdc;
        BITMAP          bitmap;
        HDC             hdcMem;
        HGDIOBJ         oldBitmap;

        hdc = BeginPaint(hwnd, &ps);

        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, hbitmap);

        GetObject(hbitmap, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 200, 200, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
        break;
    }
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ClientDraw(HWND hwnd, WPARAM wParam, LPARAM lPara)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    EndPaint(hwnd, &ps);
}

void putInBuffer(HWND hwnd, LPWSTR filePath)
{

    OpenClipboard(hwnd);
    EmptyClipboard();

    SetClipboardData(CF_BITMAP, hbitmap);
    CloseClipboard();
}

void openFile(HWND hwnd) {
    OPENFILENAMEW ofn;       // common dialog box structure
    TCHAR szFile[260] = { 0 };       // if using TCHAR macros

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrCustomFilter = L"Bitmap\0*.bmp\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        hbitmap = (HBITMAP)LoadImageW(GetModuleHandle(NULL), ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        return ofn.lpstrFile;
    }

    return NULL;
}

void getFromBuffer(HWND hwnd)
{
    OpenClipboard(hwnd);
    hbitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    CloseClipboard();
    InvalidateRect(hwnd, NULL, TRUE);

    // if (OpenClipboard(hwnd))
    // {
    //     UINT format = RegisterClipboardFormat(L"MyClipboardData");
    //     //вызываем второй раз, чтобы просто получить формат
    //     if (IsClipboardFormatAvailable(format))
    //     {
    //         MyClipboardData MCD;
    //         //извлекаем данные из буфера
    //         HANDLE hData = GetClipboardData(format);
    //         MyClipboardData *buffer = (MyClipboardData *)GlobalLock(hData);
    //         //заполняем нашу структуру полученными данными
    //         MCD = *buffer;
    //         GlobalUnlock(hData);
    //         CloseClipboard();

    //         RECT rect;
    //         PAINTSTRUCT ps;
    //         GetClientRect(hwnd, &rect);
    //         // MessageBoxA(hwnd, MCD.InfData, L"Data", MB_OK);
    //         BITMAPFILEHEADER *bFileHeader = (BITMAPFILEHEADER *)MCD.InfData;
    //         BITMAPINFO *bInfo = (BITMAPINFO *)((char *)MCD.InfData + 14);
            
    //         HDC hdc = BeginPaint(hwnd, &ps);
    //         HBITMAP hBmpFile = CreateDIBitmap(hdc, &(bInfo->bmiHeader),
    //                                           CBM_INIT, (char *)MCD.InfData + bFileHeader->bfOffBits,
    //                                           bInfo, DIB_PAL_COLORS);
    //         HDC hMemDC = CreateCompatibleDC(hdc);
    //         SelectObject(hMemDC, hBmpFile);
    //         StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC,
    //                    0, 0, bInfo->bmiHeader.biWidth, bInfo->bmiHeader.biHeight,
    //                    SRCCOPY);
    //         ReleaseDC(hdc, hdc);
    //         DeleteDC(hMemDC);
    //         DeleteObject(hBmpFile);

    //         hbitmap = hBmpFile;
    //         InvalidateRect(hwnd, NULL, TRUE);
    //     }
    // }
    // CloseClipboard();
}