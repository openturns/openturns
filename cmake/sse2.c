#include <emmintrin.h>

int main()
{
  __m128i a, b, c;
  const int src[4] = { 1, 2, 3, 4 };
  int dst[8];
  a =  _mm_loadu_si128((__m128i*)src);
  b =  _mm_loadu_si128((__m128i*)src);
  c = _mm_add_epi32(a, b);
  _mm_storeu_si128((__m128i*)dst, c);
  for (int i = 0; i < 4; ++i) {
    if ((src[i] + src[i]) != dst[i]) {
      return -1;
    }
  }
  return 0;
}
