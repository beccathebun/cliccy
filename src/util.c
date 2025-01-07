#include <ctype.h>
#include <stdbool.h>
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
static int rand_range(int min, int max)
{
    int diff = max-min;
    return (int) (((double)(diff+1)/RAND_MAX) * rand() + min);
}

bool __startsWith_(const char *pre, const char *str, bool ci)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    char *cmp     = _CALLOC(lenpre + 1, sizeof(char));
    strncpy(cmp, str, lenpre);

    if(ci) for(char *p = cmp; *p; ++p) *p = tolower(*p);

    bool res = lenstr < lenpre ? false : memcmp(pre, cmp, lenpre) == 0;
    free(cmp);
    return res;
}

#define startsWith(prefix, string) __startsWith_(prefix, string, false)


/// case insensitive `startsWith`
#define istartsWith(prefix, string) __startsWith_(prefix,string,true)

char *__concat_(char *s1, char *s2, bool space, uint8_t nexc) {
  char *result = _CALLOC(strlen(s1) + strlen(s2) + (space ? 2 : 1) + nexc, sizeof(char));
  char *nb     = stpcpy(result, s1);
  if(nexc) for(;nexc > 0; --nexc,++nb) *nb='!'; 
  if(space) *nb = ' ';
  strcat(result, s2);
  return result;
}
#define concat(s1,s2   )         (__concat_((s1),(s2),false,0)  )
#define concats(s1,s2   )        (__concat_((s1),(s2),true, 0)  )
#define concatse(s1,s2,ne)       (__concat_((s1),(s2),true,  (ne)))

time_t time_offset(time_t t, int s, int min, int h, int d) {
  struct tm *ptr = localtime(&t);
  ptr->tm_sec  += s;
  ptr->tm_min  += min;
  ptr->tm_hour += h;
  ptr->tm_mday += d;
  return mktime(ptr);
}

#define time_offset_s(s) (time_offset(time(NULL), (s), 0, 0, 0))
#define time_offset_min(min) (time_offset(time(NULL), 0, (min), 0, 0))
#define time_offset_h(h) (time_offset(time(NULL), 0, 0, (h), 0))
#define time_offset_d(d) (time_offset(time(NULL), 0, 0, 0, (d)))

#define seed() srand(time(NULL))