#include "mruby.h"
#include "mruby/string.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

static char*
utf8_to_str(const char* utf8, size_t len)
{
#ifdef _WIN32
  wchar_t* wcsp;
  char* mbsp;
  size_t mbssize, wcssize;

  if (len == 0)
    return strdup("");
  if (len == -1)
	len = strlen(utf8);
  wcssize = MultiByteToWideChar(CP_UTF8, 0, utf8, len,  NULL, 0);
  wcsp = (wchar_t*) malloc((wcssize + 1) * sizeof(wchar_t));
  wcssize = MultiByteToWideChar(CP_UTF8, 0, utf8, len, wcsp, wcssize + 1);
  wcsp[wcssize] = 0;

  mbssize = WideCharToMultiByte(GetACP(), 0, (LPCWSTR) wcsp, -1, NULL, 0, NULL, NULL);
  mbsp = (char*) malloc((mbssize + 1));
  mbssize = WideCharToMultiByte(GetACP(), 0, (LPCWSTR) wcsp, -1, mbsp, mbssize, NULL, NULL);
  mbsp[mbssize] = 0;
  free(wcsp);
  return mbsp;
#else
  return strndup(str, len);
#endif
}

static void
printstr(mrb_state *mrb, mrb_value obj)
{
  struct RString *str;
  char *s;

  if (mrb_string_p(obj)) {
    str = mrb_str_ptr(obj);
    s = utf8_to_str((const char*) str->ptr, (size_t) str->len);
    fwrite(s, strlen(s), 1, stdout);
    free(s);
  }
}

/* 15.3.1.2.9  */
/* 15.3.1.3.34 */
mrb_value
mrb_printstr(mrb_state *mrb, mrb_value self)
{
  mrb_value argv;

  mrb_get_args(mrb, "o", &argv);
  printstr(mrb, argv);

  return argv;
}

void
mrb_mruby_print_gem_init(mrb_state* mrb)
{
  struct RClass *krn;
  krn = mrb->kernel_module;
  mrb_define_method(mrb, krn, "__printstr__", mrb_printstr, MRB_ARGS_REQ(1));
}

void
mrb_mruby_print_gem_final(mrb_state* mrb)
{
}
