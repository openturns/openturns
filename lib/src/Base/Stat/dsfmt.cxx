/*
 * dsfmt.cc
 *
 * C++ program for double precision SIMD oriented Fast Mersenne Twister (dSFMT)
 * pseudorandom number generator based on IEEE 754 format.
 *
 *
 * Original C version coded by
 * Mutsuo Saito and Makoto Matsumoto (Hiroshima University).
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
 *
 * dSFMT-src-1.2
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima University.
 * All rights reserved.
 *
 *
 * This C++ version coded by Takashi Takekawa (RIKEN).
 * Copyright (C) 2007 Takashi Takekawa. All rights reserved.
 *
 * Distributed under the BSD License, see LICENSE.txt
 */

#include "dsfmt.h"

namespace tutils
{
namespace detail
{

namespace
{
uint64_t const L(0x000FFFFFFFFFFFFFULL);
uint64_t const H(0x3FF0000000000000ULL);

uint64v2_t const LL(L, L);
uint64v2_t const HH(H, H);
uint64v2_t const INI(0x8B8B8B8B8B8B8B8BULL, 0x8B8B8B8B8B8B8B8BULL);

inline uint32_t mix1(uint32_t x)
{
  return 1812433253 * (x ^ (x >> 30));
}

inline uint32_t mix2(uint32_t x)
{
  return    1664525 * (x ^ (x >> 27));
}

inline uint32_t mix3(uint32_t x)
{
  return 1566083941 * (x ^ (x >> 27));
}

template <int M>
inline void twist(uint64v2_t& l, uint64v2_t& r,
                  uint64v2_t const& a, uint64v2_t const& b)
{
  int const s(8 * dsfmt<M>::A);
  r = swap64(l) ^ (r >> dsfmt<M>::C) ^ (r << dsfmt<M>::D);
  r ^= a ^ uint64v2_t((a[1] << s) | (a[0] >> (64 - s)), a[0] << s);
  r ^= (b >> dsfmt<M>::B) & dsfmt<M>::MSK;
  r &= LL;
  l ^= r;
  r |= HH;
}

inline int idx(int i)
{
#ifdef DSFMT_BIG_ENDIAN
  return i ^ 1;
#else
  return i;
#endif
}
}

template <int MEXP>
void
dsfmt<MEXP>::init(uint32_t seed)
{
  int const N32(4 * (N + 1));

  uint32_t* u32_(reinterpret_cast<uint32_t*>(u_));
  u32_[idx(0)] = seed;
  for (int i(1); i < N32; ++i)
  {
    u32_[idx(i)] = mix1(u32_[idx(i - 1)]) + i;
  }
  init();
}

template <int MEXP>
void
dsfmt<MEXP>::init(uint32_t const* seed, int size)
{
  int const N32(4 * (N + 1));
  int const MIG(N32 >= 623 ? 11 : (N32 >= 68 ? 7 : (N32 >= 39 ? 5 : 3)));
  int const MID((N32 - MIG) / 2);
  int const PRE(N32 - 1);
  int const LAG(MID + MIG);

  uint32_t* u32_(reinterpret_cast<uint32_t*>(u_));
  for (int i(0); i <= N; ++i)
  {
    u_[i] = INI;
  }
  int i(0);
  for (; i < 1; ++i)
  {
    int const iMOD(i % N32);
    int const iMID((i + MID) % N32);
    int const iPRE((i + PRE) % N32);
    int const iLAG((i + LAG) % N32);
    uint32_t r(mix2(u32_[idx(iMOD)] ^ u32_[idx(iMID)] ^ u32_[idx(iPRE)]));
    u32_[idx(iMID)] += r;
    r += size + iMOD;
    u32_[idx(iLAG)] += r;
    u32_[idx(iMOD)] = r;
  }
  for (; i <= size; ++i)
  {
    int const iMOD(i % N32);
    int const iMID((i + MID) % N32);
    int const iPRE((i + PRE) % N32);
    int const iLAG((i + LAG) % N32);
    uint32_t r(mix2(u32_[idx(iMOD)] ^ u32_[idx(iMID)] ^ u32_[idx(iPRE)]));
    u32_[idx(iMID)] += r;
    r += seed[i - 1] + iMOD;
    u32_[idx(iLAG)] += r;
    u32_[idx(iMOD)] = r;
  }
  for (; i < N32; ++i)
  {
    int const iMOD(i % N32);
    int const iMID((i + MID) % N32);
    int const iPRE((i + PRE) % N32);
    int const iLAG((i + LAG) % N32);
    uint32_t r(mix2(u32_[idx(iMOD)] ^ u32_[idx(iMID)] ^ u32_[idx(iPRE)]));
    u32_[idx(iMID)] += r;
    r += iMOD;
    u32_[idx(iLAG)] += r;
    u32_[idx(iMOD)] = r;
  }
  for (int j(0); j < N32; ++i, ++j)
  {
    int const iMOD(i % N32);
    int const iMID((i + MID) % N32);
    int const iPRE((i + PRE) % N32);
    int const iLAG((i + LAG) % N32);
    uint32_t r(mix3(u32_[idx(iMOD)] + u32_[idx(iMID)] + u32_[idx(iPRE)]));
    u32_[idx(iMID)] ^= r;
    r -= iMOD;
    u32_[idx(iLAG)] ^= r;
    u32_[idx(iMOD)] = r;
  }
  init();
}

template <int MEXP>
void
dsfmt<MEXP>::init()
{
  uint64v2_t const FIX(uint64v2_t(0, H >> (64 - 8 * A))
                       ^ ((((HH >> B) & swap64(MSK)) ^ (HH >> C)) | HH));

  for (int i(0); i <= N; ++i)
  {
    u_[i] &= LL;
    u_[i] |= HH;
  }

  uint64v2_t const work2((u_[N] ^ FIX) & PCV);
  simd_empty();
  uint64_t work(work2[0] ^ work2[1]);
  for (int i(32); i > 0; i >>= 1)
  {
    work ^= work >> i;
  }
  if ((work & 1) != 1)
  {
    u_[N][0] ^= 1;
  }

  gen_rand_all();
}

template <int MEXP>
void
dsfmt<MEXP>::gen_rand_all()
{
  uint64v2_t l(u_[N]);
  uint64v2_t r(u_[N - 1]);
  for (int i(0); i < S; ++i)
  {
    twist<MEXP>(l, r, u_[i], u_[i - S + N]);
    u_[i] = r;
  }
  for (int i(S); i < N; ++i)
  {
    twist<MEXP>(l, r, u_[i], u_[i - S    ]);
    u_[i] = r;
  }
  u_[N] = l;
  i_ = 0;
}

#define TLIB_DSFMT_PARAM(m, s, a, b, c, d, m1, m0, p1, p0)      \
    template <> int const dsfmt<m>::S(s);                       \
    template <> int const dsfmt<m>::A(a);                       \
    template <> int const dsfmt<m>::B(b);                       \
    template <> int const dsfmt<m>::C(c);                       \
    template <> int const dsfmt<m>::D(d);                       \
    template <> uint64v2_t const dsfmt<m>::MSK(m1, m0);         \
    template <> uint64v2_t const dsfmt<m>::PCV(p1, p0)

TLIB_DSFMT_PARAM(607, 3, 1, 7, 24, 33,
                 0xFDFFFFB7FFFFFFFFULL, 0xFFCFEEF7FDFFFFFFULL,
                 0x0005196200000000ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(1279, 7, 1, 4, 8, 33,
                 0xF7FFBCBFF5FFFFFFULL, 0xFBAFFBDE7BFFEF7FULL,
                 0x000D303EE3092A2BULL, 0x0007F1DE08000001ULL);

TLIB_DSFMT_PARAM(2281, 7, 1, 3, 8, 31,
                 0xF77FDFDFF7FFFFFFULL, 0xFE9FE9FFE7BFFDFDULL,
                 0x0000E00000000000ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(4423, 37, 1, 5, 16, 31,
                 0xDABFDB9FFFFFB77FULL, 0xFBFF7FFFFFFFFFFFULL,
                 0x000DE72AD8BBB330ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(11213, 65, 3, 7, 16, 37,
                 0xFBF7FF7FFBEF3DF7ULL, 0xFFDBFDBFDFBB7FFEULL,
                 0x00032A9A00000000ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(19937, 155, 1, 7, 16, 29,
                 0xFFFF6FEBFFFFFFEEULL, 0x57FBFFFDFFFF575FULL,
                 0x000EC8F3D0B00000ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(44497, 183, 3, 7, 16, 9,
                 0xFFFFFFFFD7EFF6FFULL, 0xFEFFFCFDEFFEF7FFULL,
                 0x0001930400000000ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(86243, 682, 3, 4, 16, 43,
                 0xF3EFFFFFFFD6FBBEULL, 0xFFFFFDFAFF3FFEFFULL,
                 0x000C0671C63A820DULL, 0x000B8F71B0000001ULL);

TLIB_DSFMT_PARAM(132049, 574, 5, 4, 8, 15,
                 0xFFFFEFFFFBD35FFEULL, 0xDFFFFFFEDF53FFFFULL,
                 0x000395E065400000ULL, 0x0000000000000001ULL);

TLIB_DSFMT_PARAM(216091, 1068, 3, 1, 16, 15,
                 0xDFBBF7FB9FFDFFABULL, 0xFFFFFFFFFFEBFFBEULL,
                 0x000CADF580000000ULL, 0x0000000000000001ULL);

#undef TLIB_DSFMT_PARAM

template class dsfmt<607>;
template class dsfmt<1279>;
template class dsfmt<2281>;
template class dsfmt<4423>;
template class dsfmt<11213>;
template class dsfmt<19937>;
template class dsfmt<44497>;
template class dsfmt<86243>;
template class dsfmt<132049>;
template class dsfmt<216091>;

}
}
