#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN 0x0201
#define WM_RBUTTONDOWN 0x0204
#define WM_COMMAND_STOP_1 0x1000
#define WM_COMMAND_STOP_2 0x1001
#define WM_COMMAND_STOP_3 0x1002
#define WM_COMMAND_START_1 0x1003
#define WM_COMMAND_START_2 0x1004
#define WM_COMMAND_START_3 0x1005
#define WM_COMMAND_START_ALL 0x1006
#define WM_COMMAND_STOP_ALL 0x1007

#include <windows.h>
#include <stdio.h>
#include <windowsx.h>
#include <commctrl.h>

HWND TrackBars[3];
HANDLE hThread[3];
DWORD WINAPI TrackBarThread(PVOID pvParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    HWND stopThread1Button = CreateWindow(L"BUTTON", L"Stop Thread 1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 10, 100, 20, hwnd, (HMENU)WM_COMMAND_STOP_1, hInstance, NULL);
    HWND stopThread2Button = CreateWindow(L"BUTTON", L"Stop Thread 2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 50, 100, 20, hwnd, (HMENU)WM_COMMAND_STOP_2, hInstance, NULL);
    HWND stopThread3Button = CreateWindow(L"BUTTON", L"Stop Thread 3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 90, 100, 20, hwnd, (HMENU)WM_COMMAND_STOP_3, hInstance, NULL);
    HWND startThread1Button = CreateWindow(L"BUTTON", L"Start Thread 1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 130, 100, 20, hwnd, (HMENU)WM_COMMAND_START_1, hInstance, NULL);
    HWND startThread2Button = CreateWindow(L"BUTTON", L"Start Thread 2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 170, 100, 20, hwnd, (HMENU)WM_COMMAND_START_2, hInstance, NULL);
    HWND startThread3Button = CreateWindow(L"BUTTON", L"Start Thread 3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 210, 100, 20, hwnd, (HMENU)WM_COMMAND_START_3, hInstance, NULL);
    HWND startAllButton = CreateWindow(L"BUTTON", L"Start all", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 300, 250, 20, hwnd, (HMENU)WM_COMMAND_START_ALL, hInstance, NULL);
    HWND stopAllButton = CreateWindow(L"BUTTON", L"Stop all", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 350, 250, 20, hwnd, (HMENU)WM_COMMAND_STOP_ALL, hInstance, NULL);
  
    TrackBars[0] = CreateWindowW(TRACKBAR_CLASS, L"Thread 1", 
        WS_CHILD | WS_VISIBLE, 100, 50, 350, 50, hwnd, 0, hInstance, 0);
    
    TrackBars[1] = CreateWindowW(TRACKBAR_CLASS, L"Thread 2", 
        WS_CHILD | WS_VISIBLE, 100, 250, 350, 50, hwnd, 0, hInstance, 0);
    
    TrackBars[2] = CreateWindowW(TRACKBAR_CLASS, L"Thread 3", 
        WS_CHILD | WS_VISIBLE, 100, 450, 350, 50, hwnd, 0, hInstance, 0);
    
    // for (size_t i = 0; i < 3; i++)
    // {
    //     ShowWindow()
    // }
    
    int first = 0, second = 1, third = 2;

    hThread[0] = CreateThread(
        NULL, // указатель на структуру SECURITY_ATTRIBUTES
        0, // определяет размер стека по умолчанию 
        TrackBarThread, //определяет адрес потоковой функции 
        &first, // указатель на аргумент потоковой функции 
        CREATE_SUSPENDED, // поток остановлен 
        NULL);
    
    hThread[1] = CreateThread(
        NULL, 
        0, 
        TrackBarThread, 
        &second, 
        CREATE_SUSPENDED,
        NULL);

    hThread[2] = CreateThread(
        NULL,
        0, 
        TrackBarThread, 
        &third, 
        CREATE_SUSPENDED,
        NULL);
    
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

void processWmCommand(WPARAM wparam)
{
    switch (LOWORD(wparam))
    {
    case WM_COMMAND_START_1:
        ResumeThread(hThread[0]);
        break;
    case WM_COMMAND_START_2:
        ResumeThread(hThread[1]);
        break;
    case WM_COMMAND_START_3:
        ResumeThread(hThread[2]);
        break;
    case WM_COMMAND_STOP_1:
        SuspendThread(hThread[0]);
        break;
    case WM_COMMAND_STOP_2:
        SuspendThread(hThread[1]);
        break;
    case WM_COMMAND_STOP_3:
        SuspendThread(hThread[2]);
        break;
    case WM_COMMAND_START_ALL:
        for (size_t i = 0; i < 3; i++)
        {
            ResumeThread(hThread[i]);
        }
        break;
    case WM_COMMAND_STOP_ALL:
        for (size_t i = 0; i < 3; i++)
        {
            SuspendThread(hThread[i]);
        }
        break;
        
    }
}

DWORD WINAPI TrackBarThread(LPVOID pvParam)
{
    int myCounter = *(int*) pvParam;
    // int myCounter = 0;
    int count = 0;
    for (;;) // for(int i=0;i<=100;i++)
    {
        for (int i = 0; i <= 10000000; i++)
        {
        }
        count++;
        if (count == 100)
            count = 0;
        SendMessage(TrackBars[myCounter], TBM_SETPOS, (WPARAM)TRUE, (LPARAM)count);
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
        processWmCommand(wParam);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}