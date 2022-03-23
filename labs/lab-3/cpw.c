#include <windows.h>
#include <stdio.h>
#define BUF_SIZE 256
int main(int argc, LPTSTR argv[])
{
    HANDLE hIn, hOut;
    DWORD nIn, nOut;
    CHAR Buffer[BUF_SIZE];
    if (argc != 3)
    {
        printf("Usage: cpw file1 File2\n");
        return 1;
    }
    hIn = CreateFile(argv[1], GENERIC_READ, 0, NULL,
                     OPEN_EXISTING, 0, NULL);
    if (hIn == INVALID_HANDLE_VALUE)
    {

        printf("Unable to open the input file: %x\n", GetLastError());

        return 2;
    }
    hOut = CreateFile(argv[2], GENERIC_WRITE, 0, NULL,
                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                      NULL);
    if (hOut == INVALID_HANDLE_VALUE)
    {

        printf("Unable to open the output file: %x\n", GetLastError());

        return 3;
    }
    while (ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0)
    {
        WriteFile(hOut, Buffer, nIn, &nOut, NULL);
        if (nIn != nOut)
        {
            printf("Fatal while writing: %x\n", GetLastError());
            return 4;
        }
    }
    CloseHandle(hIn);
    CloseHandle(hOut);
    return 0;
}