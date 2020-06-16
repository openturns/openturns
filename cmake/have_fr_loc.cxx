#include <locale.h>

int main(int, const char* [])
{
#if defined(_WIN32)
  const char * test_locale = setlocale(LC_NUMERIC, "fra_FRA.1252");
#else
  const char * test_locale = setlocale(LC_NUMERIC, "fr_FR.utf-8");
#endif
  return test_locale != NULL;
}
