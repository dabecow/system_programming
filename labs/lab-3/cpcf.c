#include <windows.h>
#include <stdio.h>
int main(int argc, LPTSTR argv[])
{
    if (argc != 3)
    {
        printf("Usage: cpcf file1 file2\n");
        return 1;
    }
    if (!CopyFile(argv[1], argv[2], FALSE))
    {
        printf("CopyFile error: %x\n", GetLastError());
        return 2;
    }
    return 0;
}