#ifndef DEBUG_H_
#define DEBUG_H_ 1

/**
 * Debugging print levels.
 */
#define DLVL_ALL 7
#define DLVL_TRACE 6
#define DLVL_DEBUG 5
#define DLVL_INFO 4
#define DLVL_WARN 3
#define DLVL_ERROR 2
#define DLVL_FATAL 1

/**
 * Debugging print support.
 */
#ifdef DEBUG_LEVEL
#define DEBUG
#else
#define DEBUG_LEVEL DLVL_INFO
#endif

#ifdef DEBUG
#define DPRINTF(level, fmt, args...) do { \
        if ((level) <= DEBUG_LEVEL) \
            fprintf(stderr, "%s:%d " fmt, __FILE__, __LINE__, ##args); \
    } while (0)
#else
#define DPRINTF(level, fmt, args...) do { } while (0)
#endif

/**
 * Magic value is used for consistency checks.
 */
#define DMAGIC 0xDEADBEEF

#endif /* !DEBUG_H_ */
