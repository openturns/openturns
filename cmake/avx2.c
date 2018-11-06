#include <immintrin.h>

int main()
{
  __m256i a, b, c;
  const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  int dst[8];
  a =  _mm256_loadu_si256((__m256i*)src);
  b =  _mm256_loadu_si256((__m256i*)src);
  c = _mm256_add_epi32(a, b);
  _mm256_storeu_si256((__m256i*)dst, c);
  for (int i = 0; i < 8; ++i) {
    if ((src[i] + src[i]) != dst[i]) {
      return -1;
    }
  }
  return 0;
}
