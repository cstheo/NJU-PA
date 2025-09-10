#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	const char *a = s;
	for (; *s; s++);
	return s-a;
}

size_t strnlen(const char *s, size_t n) {
  const char *a = s;
  for (; n > 0 && *s; s++, n--);
  return s - a;
}

char *strcpy(char *dst, const char *src) {
  char *saved = dst;
  while ((*dst++ = *src++));
  return saved;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t size = strnlen(src, n);
  if (size != n) 
    memset(dst + size, '\0', n - size);
  return memcpy(dst, src, size);
}

char *strcat(char *dst, const char *src) {
  strcpy(dst + strlen(dst), src);
  return dst;
}

int strcmp(const char *s1, const char *s2) {
	for (; *s1==*s2 && *s1; s1++, s2++);
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *_l, const char *_r, size_t n) {
	const unsigned char *l=(void *)_l, *r=(void *)_r;
	if (!n--) return 0;
	for (; *l && *r && n && *l == *r ; l++, r++, n--);
	return *l - *r;
}

void *memset(void *dst, int c, size_t n) {
	unsigned char *s = dst;
	for (; n; n--, s++) *s = c;
  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
	char *d = dst;
	const char *s = src;

	if (d==s) return d;
	if ((uintptr_t)s-(uintptr_t)d-n <= -2*n) return memcpy(d, s, n);

	if (d<s) {
		for (; n; n--) *d++ = *s++;
	} else {
		while (n) n--, d[n] = s[n];
	}

	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = out;
  const char *s = in;
  for (; n; n--) *d++ = *s++;
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1, *p2 = s2;
  for (; n--; p1++, p2++)
    if (*p1 != *p2)
       return *p1 - *p2;
  return 0;
}

#endif
