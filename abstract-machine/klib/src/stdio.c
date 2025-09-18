#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static void reverse(char *str, int len) {
  char *start = str;
  char *end = str + len - 1;
  while (start < end) {
    char temp = *start;
    *start++ = *end;
    *end-- = temp;
  }
}

static int itoa(long long num, char *str, int base) {
  int i = 0;
  int neg = 0;
  
  if (num < 0 && base == 10) {
    neg = 1;
    num = -num;
  }
  
  if (num == 0) {
    str[i++] = '0';
  } else {
    while (num) {
      int rem = num % base;
      str[i++] = (rem > 9) ? (rem - 10 + 'a') : (rem + '0');
      num /= base;
    }
  }
  
  if (neg) str[i++] = '-';
  
  reverse(str, i);
  str[i] = '\0';
  return i;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int written = 0;
  const char *p = fmt;
    
  while (*p && written < n - 1) {
    if (*p != '%') {
      out[written++] = *p++;
      continue;
    }
        
    p++;
        
    switch (*p) {
      case 'd': {
        int val = va_arg(ap, int);
        char temp[20];
        int len = itoa(val, temp, 10);
        for (int i = 0; i < len && written < n - 1; i++) {
          out[written++] = temp[i];
        }
        break;
      }
      case 'x': {
        unsigned int val = va_arg(ap, unsigned int);
        char temp[20];
        int len = itoa(val, temp, 16);
        for (int i = 0; i < len && written < n - 1; i++) {
          out[written++] = temp[i];
        }
        break;
      }
      case 's': {
        char *str = va_arg(ap, char *);
        if (!str) str = "(null)";
        while (*str && written < n - 1) {
          out[written++] = *str++;
        }
        break;
      }
      case 'c': {
        char c = (char)va_arg(ap, int);
        out[written++] = c;
        break;
      }
      case 'p': {
        void *ptr = va_arg(ap, void *);
        uintptr_t val = (uintptr_t)ptr;
        if (written < n - 1) out[written++] = '0';
        if (written < n - 1) out[written++] = 'x';
        char temp[20];
        int len = itoa(val, temp, 16);
        for (int i = 0; i < len && written < n - 1; i++) {
          out[written++] = temp[i];
        }
        break;
      }
      case '%': {
        out[written++] = '%';
        break;
      }
      default: {
        out[written++] = '%';
        if (written < n - 1) out[written++] = *p;
        break;
      }
    }
    p++;
  }
    
  out[written] = '\0';
  return written;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, 0x7FFFFFFF, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int result = vsprintf(out, fmt, ap);
  va_end(ap);
  return result;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int result = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return result;
}

int printf(const char *fmt, ...) {
  char buffer[4096];
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
  va_end(ap);
  
  for (int i = 0; i < len; i++) {
    putch(buffer[i]);
  }
  
  return len;
}

#endif
