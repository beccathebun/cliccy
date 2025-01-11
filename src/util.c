#include "util.h"
extern int rand_range(int min, int max)
{
    int diff = max-min;
    return (int) (((double)(diff+1)/RAND_MAX) * rand() + min);
}
extern bool is_time(time_t timer) {
  double diff = difftime(time(NULL), timer);
  return diff >= 0.0;
}
static bool __startsWith_(const char *prefix, const char *str, bool case_insensitive)
{
    size_t lenpre = strlen(prefix),
           lenstr = strlen(str);
    char *cmp     = (char*)(_CALLOC(lenpre + 1, sizeof(char)));
    strncpy(cmp, str, lenpre);

    if(case_insensitive) for(char *p = cmp; *p; ++p) *p = tolower(*p);

    bool res = lenstr < lenpre ? false : memcmp(prefix, cmp, lenpre) == 0;
    free(cmp);
    return res;
}

extern bool startsWith(const char *prefix, const char *str){
  return __startsWith_(prefix, str, false);
}

extern bool istartsWith(const char *prefix, const char *str){
  return __startsWith_(prefix, str, true);
}

extern char *__concat_(const char *s1, const char *s2, bool space, uint8_t nexc) {
  char *result = (char*)(_CALLOC(strlen(s1) + strlen(s2) + (space ? 2 : 1) + nexc, sizeof(char)));
  char *nb     = stpcpy(result, s1);
  if(nexc) for(;nexc > 0; --nexc,++nb) *nb='!'; 
  if(space) *nb = ' ';
  strcat(result, s2);
  return result;
}


extern time_t time_offset(time_t t, int s, int min, int h, int d) {
  struct tm *ptr = localtime(&t);
  ptr->tm_sec  += s;
  ptr->tm_min  += min;
  ptr->tm_hour += h;
  ptr->tm_mday += d;
  return mktime(ptr);
}

#ifdef _WIN32
// --------------------------------------------------
// stolen from:
// musl/src/string/stpcpy.c
char *stpcpy(char *restrict d, const char *restrict s)
{
#ifdef __GNUC__
	typedef size_t __attribute__((__may_alias__)) word;
	word *wd;
	const word *ws;
	if ((uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN) {
		for (; (uintptr_t)s % ALIGN; s++, d++)
			if (!(*d=*s)) return d;
		wd=(void *)d; ws=(const void *)s;
		for (; !HASZERO(*ws); *wd++ = *ws++);
		d=(void *)wd; s=(const void *)ws;
	}
#endif
	for (; (*d=*s); s++, d++);

	return d;
}
// --------------------------------------------------
#endif
