#ifndef PATTERNS_H
#define PATTERNS_H

typedef enum {
    PATTERN_REL      = 1,
    PATTERN_OPTIONAL = 2,
    PATTERN_OFFSET   = 4,
} PatternFlags;

typedef struct {
    const char *name;
    const uint8_t *pattern;
    const char *mask;
    void **out;
    void *hook;
    int offset;
    PatternFlags flags;
}  Pattern_t;

bool scan_all(uint8_t *base, size_t size);

#endif // PATTERNS_H
