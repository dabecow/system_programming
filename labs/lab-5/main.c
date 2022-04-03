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

typedef struct
{
    LPTSTR *InfData;
} MyClipboardData;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void putInBuffer(HWND hwnd);
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
    case WM_CREATE:
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == WM_COMMAND_PUT)
        {
            putInBuffer(hwnd);
        }
        else if (LOWORD(wParam) == WM_COMMAND_GET)
        {
            getFromBuffer(hwnd);
        }
        break;
    case WM_PAINT:
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

void putInBuffer(HWND hwnd)
{
    HANDLE hBmpFile = CreateFileA("LOVE.bmp", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hBmpMappingFile = CreateFileMappingA(
        hBmpFile,      //описатель файла
        NULL,          //аттрибуты защиты объекта ядра
        PAGE_READONLY, //атрибут защиты, присваиваемый
        // страницам физической памяти
        0, 0, //максимальный размер файла в
        // байтах (0 -- размер меньше 4Гб)
        "BmpFile"); //имя объекта ядра
    //Проецирование данных файла на адресное пространство процесса
    LPTSTR sharedBuffer = MapViewOfFile(
        hBmpMappingFile,     // описатель объекта «проекция файла»
        FILE_MAP_ALL_ACCESS, // вид доступа к данным
        0, 0,                // смещение в файле до байта файла данных,
        //который нужно считать в представлении первым
        0); // сколько байтов файла данных должно быть
    //спроецировано на адресное пространство

    UINT format = RegisterClipboardFormat(L"MyClipboardData");
    //регистрируем наш формат данных
    MyClipboardData MCD = {.InfData = sharedBuffer};
    // SendDlgItemMessageA(hwnd, NULL, WM_GETTEXT, 50, (LPARAM)MCD.InfData);
    if (OpenClipboard(hwnd))
    {
        if (EmptyClipboard())
        {
            HGLOBAL hGl;
            EmptyClipboard();                                             //очищаем буфер
            hGl = GlobalAlloc(GMEM_DDESHARE, sizeof(MyClipboardData));    //выделим память
            MyClipboardData *buffer = (MyClipboardData *)GlobalLock(hGl); //запишем данные в память
            *buffer = MCD;
            GlobalUnlock(hGl);
            SetClipboardData(format, hGl);
            CloseClipboard();
        }
    }
}

void getFromBuffer(HWND hwnd)
{

    if (OpenClipboard(hwnd))
    {
        UINT format = RegisterClipboardFormat(L"MyClipboardData");
        //вызываем второй раз, чтобы просто получить формат
        if (IsClipboardFormatAvailable(format))
        {
            MyClipboardData MCD;
            //извлекаем данные из буфера
            HANDLE hData = GetClipboardData(format);
            MyClipboardData *buffer = (MyClipboardData *)GlobalLock(hData);
            //заполняем нашу структуру полученными данными
            MCD = *buffer;
            GlobalUnlock(hData);
            CloseClipboard();

            RECT rect;
            PAINTSTRUCT ps;
            GetClientRect(hwnd, &rect);
            // MessageBoxA(hwnd, MCD.InfData, L"Data", MB_OK);
            BITMAPFILEHEADER *bFileHeader = (BITMAPFILEHEADER *)MCD.InfData;
            BITMAPINFO *bInfo = (BITMAPINFO *)((char *)MCD.InfData + 14);

            HDC hdc = BeginPaint(hwnd, &ps);
            HBITMAP hBmpFile = CreateDIBitmap(hdc, &(bInfo->bmiHeader),
                                      CBM_INIT, (char *)MCD.InfData + bFileHeader->bfOffBits,
                                      bInfo, DIB_PAL_COLORS);
            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, hBmpFile);
            StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC,
                       0, 0, bInfo->bmiHeader.biWidth, bInfo->bmiHeader.biHeight,
                       SRCCOPY);
            ReleaseDC(hdc, hdc);
            DeleteDC(hMemDC);
            DeleteObject(hBmpFile);
        }
    }
    CloseClipboard();
}

// void openTextFile()
// {
//     HANDLE hTxtMappingFile = OpenFileMappingA(FILE_MAP_READ, FALSE, L"kate.txt");
//     if (hTxtMappingFile != NULL)
//     {
//         //Проецирование данных файла на адресное пространство процесса
//         LPVOID hTxtMapFileStartAddr = MapViewOfFile(hTxtMappingFile, FILE_MAP_READ, 0, 0, 0);
//         //Вывод данных из текстового файла в эдит
//         MessageBoxA()
//             SetDlgItemTextA(hDlg, IDC_EDIT_TEXT, (char *)hTxtMapFileStartAddr);
//         //Отключение файла данных от адресного пространства процесса
//         UnmapViewOfFile(hTxtMapFileStartAddr);
//         //Закрытие объекта «проекция файла»
//         CloseHandle(hTxtMappingFile);
//     }
// }

// void openImageFile()
// {
//     //Открытие объекта ядра «файл»
//     HANDLE hBmpFile = CreateFileA("LOVE.bmp", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//     GetClientRect(GetDlgItem(hDlg, IDC_BMPFRAME), &rect);
//     //Создание объекта ядра «проекция файла»
//     hBmpMappingFile = CreateFileMappingA(
//         hBmpFile,      //описатель файла
//         NULL,          //аттрибуты защиты объекта ядра
//         PAGE_READONLY, //атрибут защиты, присваиваемый
//         // страницам физической памяти
//         0, 0, //максимальный размер файла в
//         // байтах (0 -- размер меньше 4Гб)
//         "BmpFile"); //имя объекта ядра
//     //Проецирование данных файла на адресное пространство процесса
//     hBmpMapFileAddr = MapViewOfFile(
//         hBmpMappingFile, // описатель объекта «проекция файла»
//         FILE_MAP_READ,   // вид доступа к данным
//         0, 0,            // смещение в файле до байта файла данных,
//         //который нужно считать в представлении первым
//         0); // сколько байтов файла данных должно быть
//     //спроецировано на адресное пространство
//     //(0 -- от смещения и до конца файла)
//     BITMAPFILEHEADER *bFileHeader =
//         (BITMAPFILEHEADER *)hBmpMapFileAddr;
//     BITMAPINFO *bInfo = (BITMAPINFO *)((char *)hBmpMapFileAddr + 14);
//     hdc = GetDC(GetDlgItem(hDlg, IDC_BMPFRAME));
//     hBmpFile = CreateDIBitmap(hdc, &(bInfo->bmiHeader),
//                               CBM_INIT, (char *)hBmpMapFileAddr + bFileHeader->bfOffBits,
//                               bInfo, DIB_PAL_COLORS);
//     hMemDC = CreateCompatibleDC(hdc);
//     SelectObject(hMemDC, hBmpFile);
//     StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC,
//                0, 0, bInfo->bmiHeader.biWidth, bInfo->bmiHeader.biHeight,
//                SRCCOPY);
//     ReleaseDC(GetDlgItem(hDlg, IDC_BMPFRAME), hdc);
//     DeleteDC(hMemDC);
//     DeleteObject(hBmpFile);
//     return (INT_PTR)TRUE;
// }