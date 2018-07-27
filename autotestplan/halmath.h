#ifdef HALMATH_EXPORTS
#define HALMATH_API __declspec(dllexport)
#else
#define HALMATH_API __declspec(dllimport)
#endif

HALMATH_API int FindMaxValueofArray(int* iArray,unsigned int size_of_t);	
