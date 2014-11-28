/*
  libco
  auto-selection module
  license: public domain
*/

#include "autoconf.h"

#if defined(__GNUC__) && defined(__i386__)
#define LIBCO_X86
#elif defined(__GNUC__) && defined(__amd64__)
#define LIBCO_AMD64
#elif defined(__GNUC__) && defined(_ARCH_PPC)
#define LIBCO_PPC
#elif defined(__GNUC__)
#define LIBCO_SJLJ
#elif defined(_MSC_VER) && defined(_M_IX86)
#define LIBCO_X86
#elif defined(_MSC_VER) && defined(_M_AMD64)
#define LIBCO_AMD64
#elif defined(_MSC_VER)
#define LIBCO_FIBER
#else
#define LIBCO_SJLJ
#endif
