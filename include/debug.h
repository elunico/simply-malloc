#include <stdio.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define BLACK "\033[0;0m"

#ifdef DEBUG

#define debug(fmt, ...) fprintf(stderr, BLUE fmt BLACK, __VA_ARGS__);
#define info(fmt, ...) fprintf(stderr, MAGENTA fmt BLACK, __VA_ARGS__);
#define warn(fmt, ...) fprintf(stderr, YELLOW fmt BLACK, __VA_ARGS__);
#define should(fmt, ...) fprintf(stderr, GREEN fmt BLACK, __VA_ARGS__);
#define error(fmt, ...) fprintf(stderr, RED fmt BLACK, __VA_ARGS__);

#else

#define debug(fmt, ...)
#define info(fmt, ...)
#define warn(fmt, ...)
#define should(fmt, ...)
#define error(fmt, ...)

#endif
