#ifndef UTIL_H
#define UTIL_H
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#ifndef _CALLOC
#define _CALLOC calloc
#endif

#define streq(s1, s2) (strcmp(s1,s2) == 0)
#define arr_rand(a) a[rand() % ARRAY_LEN(a)]
#define da_rand(a)  (a.items)[rand() % (a.count)]
extern int rand_range(int min, int max);
extern bool is_time(time_t timer);
extern bool startsWith(const char *prefix, const char *str);
extern bool istartsWith(const char *prefix, const char *str);
extern char *__concat_(const char *s1, const char *s2, bool space, uint8_t nexc);
#define concat(s1,s2   )         (__concat_((s1),(s2),false,0)  )
#define concats(s1,s2   )        (__concat_((s1),(s2),true, 0)  )
#define concatse(s1,s2,ne)       (__concat_((s1),(s2),true,  (ne)))
extern time_t time_offset(time_t t, int s, int min, int h, int d);

#define time_offset_s(s) (time_offset(time(NULL), (s), 0, 0, 0))
#define time_offset_min(min) (time_offset(time(NULL), 0, (min), 0, 0))
#define time_offset_h(h) (time_offset(time(NULL), 0, 0, (h), 0))
#define time_offset_d(d) (time_offset(time(NULL), 0, 0, 0, (d)))

#define seed() srand(time(NULL))
#ifdef _WIN32
# define ALIGN (sizeof(size_t))
# define ONES ((size_t)-1/UCHAR_MAX)
# define HIGHS (ONES * (UCHAR_MAX/2+1))
# define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)
  extern char *stpcpy(char *restrict d, const char *restrict s);
#endif
#endif // UTIL_H
