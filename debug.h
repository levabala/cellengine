#define DEBUG 0

#if DEBUG
#define IFDEBUG if (1)
#else
#define IFDEBUG if (0)
#endif

#if DEBUG
#define debug_print(fmt, args...) printf(fmt, ##args)
#else
#define debug_print(fmt, args...)
#endif
