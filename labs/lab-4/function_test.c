#include <windows.h>
#define BUF_SIZE 256

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
    hOut = CreateFile(fOut, GENERIC_WRITE, 0, NULL, CREATE_NEW || CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    while (ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL) && nIn > 0 && WriteOK)
    {
        MultiByteToWideChar(CP_OEMCP, 0, aBuffer, -1, uBuffer, BUF_SIZE);

        WriteOK = WriteFile(hOut, uBuffer, BUF_SIZE, &nOut, NULL);
    }
    CloseHandle(hIn);
    CloseHandle(hOut);
    return WriteOK;
}

int main(int argc, LPTSTR argv[])
{
    BOOL Output;
    /* Вызываем функцию. */
    Output = Asc2Un("ascii-test.txt", "unicode_output.txt", FALSE);
    return 0;
}

