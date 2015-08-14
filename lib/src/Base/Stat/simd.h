#ifndef TUTILS_SIMD_H
#define TUTILS_SIMD_H

#include <cstddef>

/* Quick and dirty patch because SSE2, as implemented in this file, doesn't work on x86_64 architecture */
#undef __SSE2__

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#include <stdint.h>

namespace tutils
{


  class uint64v2_t;

  static inline uint64v2_t operator&(uint64v2_t const& x, uint64v2_t const& y);
  static inline uint64v2_t operator|(uint64v2_t const& x, uint64v2_t const& y);
  static inline uint64v2_t operator^(uint64v2_t const& x, uint64v2_t const& y);
  static inline uint64v2_t operator>>(uint64v2_t const& x, int s);
  static inline uint64v2_t operator<<(uint64v2_t const& x, int s);
  static inline uint64v2_t swap64(uint64v2_t const& x);
  template <int S>
    static inline uint64v2_t rshift128(uint64v2_t const& x);
  template <int S>
    static inline uint64v2_t lshift_byte(uint64v2_t const& x);



  inline void simd_empty() {
#ifdef __SSE2__
    _mm_empty();
#endif
  }

  class uint64v2_t
  {
  public:
    uint64v2_t() {}

    uint64v2_t(uint64_t u1, uint64_t u0) {
      set(u1, u0);
    }

    void set(uint64_t u1, uint64_t u0) {
      (*this)[0] = u0;
      (*this)[1] = u1;
    }

    uint64_t const& operator[](std::size_t i) const {
#ifdef __SSE2__
      return reinterpret_cast<uint64_t const*>(&vec_)[i];
#else
      return u64_[i];
#endif
    }

    uint64_t& operator[](std::size_t i) {
#ifdef __SSE2__
      return reinterpret_cast<uint64_t*>(&vec_)[i];
#else
      return u64_[i];
#endif
    }

    uint64v2_t& operator&=(uint64v2_t const& y) {
#ifdef __SSE2__
      vec_ = _mm_and_si128(vec_, y.vec_);
#else
      u64_[0] &= y.u64_[0];
      u64_[1] &= y.u64_[1];
#endif
      return *this;
    }

    uint64v2_t& operator|=(uint64v2_t const& y) {
#ifdef __SSE2__
      vec_ = _mm_or_si128(vec_, y.vec_);
#else
      u64_[0] |= y.u64_[0];
      u64_[1] |= y.u64_[1];
#endif
      return *this;
    }

    uint64v2_t& operator^=(uint64v2_t const& y) {
#ifdef __SSE2__
      vec_ = _mm_xor_si128(vec_, y.vec_);
#else
      u64_[0] ^= y.u64_[0];
      u64_[1] ^= y.u64_[1];
#endif
      return *this;
    }

    uint64v2_t& operator>>=(int s) {
#ifdef __SSE2__
      vec_ = _mm_srli_epi64(vec_, s);
#else
      u64_[0] >>= s;
      u64_[1] >>= s;
#endif
      return *this;
    }

    uint64v2_t& operator<<=(int s) {
#ifdef __SSE2__
      vec_ = _mm_slli_epi64(vec_, s);
#else
      u64_[0] <<= s;
      u64_[1] <<= s;
#endif
      return *this;
    }

    friend uint64v2_t operator&(uint64v2_t const& x, uint64v2_t const& y);

    friend uint64v2_t operator|(uint64v2_t const& x, uint64v2_t const& y);

    friend uint64v2_t operator^(uint64v2_t const& x, uint64v2_t const& y);

    friend uint64v2_t operator>>(uint64v2_t const& x, int s);

    friend uint64v2_t operator<<(uint64v2_t const& x, int s);

    friend uint64v2_t swap64(uint64v2_t const& x);

    template <int S>
      friend uint64v2_t rshift128(uint64v2_t const& x);

    template <int S>
      friend uint64v2_t lshift_byte(uint64v2_t const& x);

  private:
#ifdef __SSE2__
    __m128i vec_;
  uint64v2_t(__m128i const& y) : vec_(y) {
    }
#else
    uint64_t u64_[2];
#endif
  };


  uint64v2_t operator&(uint64v2_t const& x, uint64v2_t const& y) {
#ifdef __SSE2__
    return _mm_and_si128(x.vec_, y.vec_);
#else
    return uint64v2_t(x[1] & y[1], x[0] & y[0]);
#endif
  }

  uint64v2_t operator|(uint64v2_t const& x, uint64v2_t const& y) {
#ifdef __SSE2__
    return _mm_or_si128(x.vec_, y.vec_);
#else
    return uint64v2_t(x[1] | y[1], x[0] | y[0]);
#endif
  }

  uint64v2_t operator^(uint64v2_t const& x, uint64v2_t const& y) {
#ifdef __SSE2__
    return _mm_xor_si128(x.vec_, y.vec_);
#else
    return uint64v2_t(x[1] ^ y[1], x[0] ^ y[0]);
#endif
  }

  uint64v2_t operator>>(uint64v2_t const& x, int s) {
#ifdef __SSE2__
    return _mm_srli_epi64(x.vec_, s);
#else
    return uint64v2_t(x[1] >> s, x[0] >> s);
#endif
  }

  uint64v2_t operator<<(uint64v2_t const& x, int s) {
#ifdef __SSE2__
    return _mm_slli_epi64(x.vec_, s);
#else
    return uint64v2_t(x[1] << s, x[0] << s);
#endif
  }

  uint64v2_t swap64(uint64v2_t const& x) {
#ifdef __SSE2__
    return _mm_shuffle_epi32(x.vec_, 0x4e);
#else
    return uint64v2_t(x[0], x[1]);
#endif
  }

  template <int S>
    uint64v2_t rshift128(uint64v2_t const& x) {
#ifdef __SSE2__
    return _mm_srli_si128(x.vec_, S);
#else
    int const s(8 * S);
    return uint64v2_t(x[1] >> s, (x[1] << (64 - s)) | (x[0] >> s));
#endif
  }

  template <int S>
    uint64v2_t lshift_byte(uint64v2_t const& x) {
#ifdef __SSE2__
    return _mm_slli_si128(x.vec_, S);
#else
    int const s(8 * S);
    return uint64v2_t((x[1] << s) | (x[0] >> (64 - s)), x[0] << s);
#endif
  }


}

#endif
