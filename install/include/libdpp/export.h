
#ifndef libdpp_EXPORT_H
#define libdpp_EXPORT_H

#ifdef libdpp_LIBDPP_STATIC_DEFINE
#  define libdpp_EXPORT
#  define libdpp_NO_EXPORT
#else
#  ifndef libdpp_EXPORT
#    ifdef libdpp_EXPORTS
        /* We are building this library */
#      define libdpp_EXPORT 
#    else
        /* We are using this library */
#      define libdpp_EXPORT 
#    endif
#  endif

#  ifndef libdpp_NO_EXPORT
#    define libdpp_NO_EXPORT 
#  endif
#endif

#ifndef libdpp_LIBDPP_DEPRECATED
#  define libdpp_LIBDPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef libdpp_LIBDPP_DEPRECATED_EXPORT
#  define libdpp_LIBDPP_DEPRECATED_EXPORT libdpp_EXPORT libdpp_LIBDPP_DEPRECATED
#endif

#ifndef libdpp_LIBDPP_DEPRECATED_NO_EXPORT
#  define libdpp_LIBDPP_DEPRECATED_NO_EXPORT libdpp_NO_EXPORT libdpp_LIBDPP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef libdpp_LIBDPP_NO_DEPRECATED
#    define libdpp_LIBDPP_NO_DEPRECATED
#  endif
#endif

#endif /* libdpp_EXPORT_H */
