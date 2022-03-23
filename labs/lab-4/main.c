#include <windows.h>
__declspec(dllimport) BOOL __cdecl Asc2Un(LPCTSTR, LPCTSTR, BOOL);

int main(int argc, LPTSTR argv[])
{
    BOOL Output;
    /* Вызываем функцию. */
    Output = Asc2Un(argv[1], argv[2], FALSE);
    return 0;
}