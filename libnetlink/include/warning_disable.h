#ifndef __INCLUDE_WARNING_DISABLE_H__
#define __INCLUDE_WARNING_DISABLE_H__

#if defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningName) DO_PRAGMA(GCC diagnostic ignored warningName)


#define DISABLE_WARNING_OLD_STYLE_CAST DISABLE_WARNING("-Wold-style-cast")
#define DISABLE_WARNING_MISSING_FIELD_INITIALIZERS DISABLE_WARNING("-Wmissing-field-initializers")
#define DISABLE_WARNING_SIGN_CONVERSION DISABLE_WARNING("-Wsign-conversion")
#define DISABLE_WARNING_IMPLICIT_INT_CONVERSION DISABLE_WARNING("-Wimplicit-int-conversion")
#define DISABLE_WARNING_SIGN_COMPARE DISABLE_WARNING("-Wsign-compare")
#define DISABLE_WARNING_SHORTEN_64_TO_32 DISABLE_WARNING("-Wshorten-64-to-32")
#define DISABLE_WARNING_CAST_ALIGN DISABLE_WARNING("-Wcast-align")
// other warnings you want to deactivate...

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
// other warnings you want to deactivate...

#endif

#endif//!__INCLUDE_WARNING_DISABLE_H__