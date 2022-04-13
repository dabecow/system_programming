#include <stdio.h>
#include <windows.h>

int main(int argc, char const *argv[])
{
    HINSTANCE lib = LoadLibraryA("test.dll");
    FARPROC Asc2Un = GetProcAddress(lib, "Asc2Un");
    Asc2Un(argv[1], argv[2], FALSE);
    FreeLibrary(lib);
    /* code */
    return 0;
}
