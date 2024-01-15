#ifdef _WIN32
#define CDeclFunction __cdecl
#else
#define CDeclFunction 
#endif

void * CDeclFunction operator new[](size_t size, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    (void)name;
    (void)flags;
    (void)debugFlags;
    (void)file;
    (void)line;
    return new uint8_t[size];
}

void * CDeclFunction operator new[](size_t size, size_t alignement, size_t offset, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    (void)name;
    (void)flags;
    (void)debugFlags;
    (void)file;
    (void)line;
    (void)alignement;
    (void)offset;
    return new uint8_t[size];
}

//int CDeclFunction EA::StdC::Vsnprintf(char * __restrict pDestination, unsigned __int64 n, char const * __restrict pFormat, char * arguments)
//{
//    return vsnprintf(pDestination, n, pFormat, arguments);
//}