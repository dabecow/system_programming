#include <windows.h>
#ifndef UNICODE
#define UNICODE
#endif

#define BUF_SIZE 256
__declspec(dllexport) BOOL __cdecl Asc2Un(LPCTSTR, LPCTSTR, BOOL);

BOOL Asc2Un(LPCTSTR fin, LPCTSTR fOut, BOOL bFaillfExists)
{
/* Функция копирования файла ASCII в Unicode на базе CopyFile. */
    HANDLE hIn, hOut;
    DWORD fdwOut, nIn, nOut, iCopy;
    CHAR aBuffer[BUF_SIZE];
    WCHAR uBuffer[BUF_SIZE];
    BOOL WriteOK = TRUE;
    hIn = CreateFile(fin, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    /* Определяем действие CreateFile, если выходной файл уже существует.*/
    fdwOut = CREATE_NEW || CREATE_ALWAYS;
    hOut = CreateFile(fOut, GENERIC_WRITE, 0, NULL, fdwOut, FILE_ATTRIBUTE_NORMAL, NULL);

    while (ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL) && nIn > 0 && WriteOK)
    {
        MultiByteToWideChar(CP_OEMCP, 0, aBuffer, -1, uBuffer, BUF_SIZE);

        WriteOK = WriteFile(hOut, uBuffer, 2 * nIn, &nOut, NULL);
    }
    CloseHandle(hIn);
    CloseHandle(hOut);
    return WriteOK;
}