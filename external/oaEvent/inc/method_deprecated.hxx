#ifndef method_deprecated_hxx
#define method_deprecated_hxx

#ifndef METHOD_DEPRECATED
#if defined(__GNUC__) && !defined(__CINT__) && !defined(G__DICTIONARY)
#define METHOD_DEPRECATED __attribute__ ((deprecated))
#else
#define METHOD_DEPRECATED
#endif
#endif

#endif
