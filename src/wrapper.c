#define DLLEXPORT __declspec(dllexport)

static HMODULE real_dll;

void wrapper()
{
    if (real_dll)
        return;

    char path[MAX_PATH];
    GetSystemDirectoryA(path, MAX_PATH);
    lstrcatA(path, "\\version.dll");
    real_dll = LoadLibraryA(path);
}

DLLEXPORT BOOL WINAPI GetFileVersionInfoA(
    LPCSTR a, DWORD b, DWORD c, LPVOID d)
{
    wrapper();
    return ((BOOL (WINAPI *)(LPCSTR,DWORD,DWORD,LPVOID))
        GetProcAddress(real_dll,"GetFileVersionInfoA"))(a,b,c,d);
}

DLLEXPORT BOOL WINAPI GetFileVersionInfoW(
    LPCWSTR a, DWORD b, DWORD c, LPVOID d)
{
    wrapper();
    return ((BOOL (WINAPI *)(LPCWSTR,DWORD,DWORD,LPVOID))
        GetProcAddress(real_dll,"GetFileVersionInfoW"))(a,b,c,d);
}

DLLEXPORT DWORD WINAPI GetFileVersionInfoSizeA(
    LPCSTR a, LPDWORD b)
{
    wrapper();
    return ((DWORD (WINAPI *)(LPCSTR,LPDWORD))
        GetProcAddress(real_dll,"GetFileVersionInfoSizeA"))(a,b);
}

DLLEXPORT DWORD WINAPI GetFileVersionInfoSizeW(
    LPCWSTR a, LPDWORD b)
{
    wrapper();
    return ((DWORD (WINAPI *)(LPCWSTR,LPDWORD))
        GetProcAddress(real_dll,"GetFileVersionInfoSizeW"))(a,b);
}

DLLEXPORT DWORD WINAPI GetFileVersionInfoSizeExA(
    DWORD a, LPCSTR b, LPDWORD c)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPCSTR,LPDWORD))
        GetProcAddress(real_dll,"GetFileVersionInfoSizeExA"))(a,b,c);
}

DLLEXPORT DWORD WINAPI GetFileVersionInfoSizeExW(
    DWORD a, LPCWSTR b, LPDWORD c)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPCWSTR,LPDWORD))
        GetProcAddress(real_dll,"GetFileVersionInfoSizeExW"))(a,b,c);
}

DLLEXPORT BOOL WINAPI GetFileVersionInfoExA(
    DWORD a, LPCSTR b, DWORD c, DWORD d, LPVOID e)
{
    wrapper();
    return ((BOOL (WINAPI *)(DWORD,LPCSTR,DWORD,DWORD,LPVOID))
        GetProcAddress(real_dll,"GetFileVersionInfoExA"))(a,b,c,d,e);
}

DLLEXPORT BOOL WINAPI GetFileVersionInfoExW(
    DWORD a, LPCWSTR b, DWORD c, DWORD d, LPVOID e)
{
    wrapper();
    return ((BOOL (WINAPI *)(DWORD,LPCWSTR,DWORD,DWORD,LPVOID))
        GetProcAddress(real_dll,"GetFileVersionInfoExW"))(a,b,c,d,e);
}

DLLEXPORT DWORD WINAPI VerFindFileA(
    DWORD a, LPSTR b, LPSTR c, LPSTR d,
    LPSTR e, PUINT f, LPSTR g, PUINT h)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,PUINT,LPSTR,PUINT))
        GetProcAddress(real_dll,"VerFindFileA"))(a,b,c,d,e,f,g,h);
}

DLLEXPORT DWORD WINAPI VerFindFileW(
    DWORD a, LPWSTR b, LPWSTR c, LPWSTR d,
    LPWSTR e, PUINT f, LPWSTR g, PUINT h)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT,LPWSTR,PUINT))
        GetProcAddress(real_dll,"VerFindFileW"))(a,b,c,d,e,f,g,h);
}

DLLEXPORT DWORD WINAPI VerInstallFileA(
    DWORD a, LPSTR b, LPSTR c, LPSTR d,
    LPSTR e, LPSTR f, LPSTR g, PUINT h)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,PUINT))
        GetProcAddress(real_dll,"VerInstallFileA"))(a,b,c,d,e,f,g,h);
}

DLLEXPORT DWORD WINAPI VerInstallFileW(
    DWORD a, LPWSTR b, LPWSTR c, LPWSTR d,
    LPWSTR e, LPWSTR f, LPWSTR g, PUINT h)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT))
        GetProcAddress(real_dll,"VerInstallFileW"))(a,b,c,d,e,f,g,h);
}

DLLEXPORT DWORD WINAPI VerLanguageNameA(
    DWORD a, LPSTR b, DWORD c)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPSTR,DWORD))
        GetProcAddress(real_dll,"VerLanguageNameA"))(a,b,c);
}

DLLEXPORT DWORD WINAPI VerLanguageNameW(
    DWORD a, LPWSTR b, DWORD c)
{
    wrapper();
    return ((DWORD (WINAPI *)(DWORD,LPWSTR,DWORD))
        GetProcAddress(real_dll,"VerLanguageNameW"))(a,b,c);
}

DLLEXPORT BOOL WINAPI VerQueryValueA(
    LPCVOID a, LPCSTR b, LPVOID *c, PUINT d)
{
    wrapper();
    return ((BOOL (WINAPI *)(LPCVOID,LPCSTR,LPVOID*,PUINT))
        GetProcAddress(real_dll,"VerQueryValueA"))(a,b,c,d);
}

DLLEXPORT BOOL WINAPI VerQueryValueW(
    LPCVOID a, LPCWSTR b, LPVOID *c, PUINT d)
{
    wrapper();
    return ((BOOL (WINAPI *)(LPCVOID,LPCWSTR,LPVOID*,PUINT))
        GetProcAddress(real_dll,"VerQueryValueW"))(a,b,c,d);
}
