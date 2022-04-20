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

HBITMAP hbitmapToShow;
LPWSTR textToShow;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
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

    HWND buttonPut = CreateWindow(L"BUTTON", L"Put in clipboard", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 200, 100, hwnd, (HMENU)WM_COMMAND_PUT, hInstance, NULL);
    HWND buttonGet = CreateWindow(L"BUTTON", L"Get from clipboard", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 110, 200, 100, hwnd, (HMENU)WM_COMMAND_GET, hInstance, NULL);

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
    case WM_CREATE:
    {
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
        }
        else if (LOWORD(wParam) == WM_COMMAND_GET)
        {
            getFromBuffer(hwnd);
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc;
        BITMAP bitmap;
        HDC hdcMem;
        HGDIOBJ oldBitmap;

        hdc = BeginPaint(hwnd, &ps);

        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, hbitmapToShow);

        GetObject(hbitmapToShow, sizeof(bitmap), &bitmap);
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

void openFile(HWND hwnd)
{
    OPENFILENAMEW ofn;       // common dialog box structure
    TCHAR szFile[260] = {0}; // if using TCHAR macros

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrCustomFilter = L"Bitmap\0*.bmp\0Text\0*.txt\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&ofn) == TRUE)
    {

        char extension[4];
        wcstombs(extension, ofn.lpstrFile + (int)ofn.nFileExtension, 3);

        if (strcmp(extension, "bmp") == 0)
        {
            // HBITMAP hbitmap = (HBITMAP)LoadImageW(GetModuleHandle(NULL), ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            //Открытие объекта ядра «файл»
            HANDLE hBmpFile = CreateFileA("LOVE.bmp", GENERIC_READ, FILE_SHARE_READ, NULL,
                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            RECT rect;
            GetClientRect(hwnd, &rect);
            //Создание объекта ядра «проекция файла» hBmp
            HANDLE hBmpMappingFile = CreateFileMappingA(hBmpFile,      //описатель файла
                                             NULL,          //аттрибуты защиты объекта ядра
                                             PAGE_READONLY, //атрибут защиты, присваиваемый
                                             // страницам физической памяти
                                             0, 0, //максимальный размер файла в
                                             // байтах (0 -- размер меньше 4Гб)
                                             "BmpFile"); //имя объекта ядра
            //Проецирование данных файла на адресное пространство процесса
            LPVOID hBmpMapFileAddr = MapViewOfFile(
                hBmpMappingFile, // описатель объекта «проекция файла»
                FILE_MAP_READ,   // вид доступа к данным
                0, 0,            // смещение в файле до байта файла данных,
                //который нужно считать в представлении первым
                0); // сколько байтов файла данных должно быть
            //спроецировано на адресное пространство
            //(0 -- от смещения и до конца файла)
            BITMAPFILEHEADER *bFileHeader = (BITMAPFILEHEADER *)hBmpMapFileAddr;
            BITMAPINFO *bInfo = (BITMAPINFO *)((char *)hBmpMapFileAddr + 14);
            HDC hdc = GetDC(hwnd);
            hBmpFile = CreateDIBitmap(hdc, &(bInfo->bmiHeader),
                                      CBM_INIT, (char *)hBmpMapFileAddr + bFileHeader->bfOffBits,
                                      bInfo, DIB_PAL_COLORS);
            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, hBmpFile);
            StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC,
                       0, 0, bInfo->bmiHeader.biWidth, bInfo->bmiHeader.biHeight,
                       SRCCOPY);
            ReleaseDC(hwnd, hdc);
            DeleteDC(hMemDC);
            DeleteObject(hBmpFile);
            return (INT_PTR)TRUE;
        }
        else if (strcmp(extension, "txt") == 0)
        {
            HFILE hFile = CreateFileW(ofn.lpstrFile,         // name of the write
                                      GENERIC_READ,          // open for writing
                                      FILE_SHARE_READ,       // do not share
                                      NULL,                  // default security
                                      OPEN_EXISTING,         // create new file only
                                      FILE_ATTRIBUTE_NORMAL, // normal file
                                      NULL);
            HANDLE hTxtMappingFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, TEXT("kate"));
            if (hTxtMappingFile == NULL)
                MessageBoxW(hwnd, L"Can't make the file mapping", L"test", MB_OK);
            LPVOID hTxtMapFileStartAddr = MapViewOfFile(hTxtMappingFile, FILE_READ_ACCESS, 0, 0, 0);
            if (hTxtMapFileStartAddr == NULL)
                MessageBoxW(hwnd, L"Can't map view of file", L"test", MB_OK);
        }
    }
}

void getFromBuffer(HWND hwnd)
{

    HANDLE hTxtMappingFile = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT("kate"));
    LPCSTR readBuffer = (LPCSTR)MapViewOfFile(hTxtMappingFile, FILE_MAP_READ, 0, 0, 0);
    MessageBoxA(hwnd, readBuffer, L"test", MB_OK);
    OpenClipboard(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);

    RECT rect;
    GetClientRect(hwnd, &rect);

    HANDLE hBmpMappingFile = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT("BmpFile")); //имя объекта ядра

    LPVOID hBmpMapFileAddr = MapViewOfFile(
                hBmpMappingFile, // описатель объекта «проекция файла»
                FILE_MAP_READ,   // вид доступа к данным
                0, 0,            // смещение в файле до байта файла данных,
                //который нужно считать в представлении первым
                0); // сколько байтов файла данных должно быть
            //спроецировано на адресное пространство
            //(0 -- от смещения и до конца файла)
    BITMAPFILEHEADER *bFileHeader = (BITMAPFILEHEADER *)hBmpMapFileAddr;
    BITMAPINFO *bInfo = (BITMAPINFO *)((char *)hBmpMapFileAddr + 14);
    HDC hdc = GetDC(hwnd);
    HANDLE hBmpFile = CreateDIBitmap(hdc, &(bInfo->bmiHeader),
                                CBM_INIT, (char *)hBmpMapFileAddr + bFileHeader->bfOffBits,
                                bInfo, DIB_PAL_COLORS);
    HDC hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBmpFile);
    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC,
                0, 0, bInfo->bmiHeader.biWidth, bInfo->bmiHeader.biHeight,
                SRCCOPY);
    ReleaseDC(hwnd, hdc);
    DeleteDC(hMemDC);
    DeleteObject(hBmpFile);
}