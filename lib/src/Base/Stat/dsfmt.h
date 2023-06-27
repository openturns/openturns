/*
 * tutils/dsfmt.h
 *
 * C++ header file for
 * double precision SIMD oriented Fast Mersenne Twister (dSFMT)
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
 * Distributed under the BSD License, see LICENSE.txt.
 */

#ifndef TUTILS_DSFMT_H
#define TUTILS_DSFMT_H

#include <cstring>
#include <stdint.h>
#include "simd.h"
#include "openturns/OTprivate.hxx"
#include "openturns/OTconfig.hxx"

namespace tutils
{
  namespace detail
  {

    template <int MEXP>
      class dsfmt
      {
      public:
        static int const N = MEXP / 104;
        static int const S;
        static int const A;
        static int const B;
        static int const C;
        static int const D;
        static uint64v2_t const MSK;
        static uint64v2_t const PCV;

        explicit dsfmt(uint32_t seed = 0) { init(seed); }
        dsfmt(uint32_t const* seed, uint32_t size) { init(seed, size); }

        void init(uint32_t seed);
        void init(uint32_t const* seed, int size);

        double gen_close1_open2() {
          if (i_ == 2 * N) {
            gen_rand_all();
          }
          return reinterpret_cast<double*>(u_)[i_++];
        }
        double gen() { return gen_close1_open2() - 1.0; }

        /* Added by Regis LEBRUN, OpenTURNS project */
        uint32_t igen(const uint32_t n) {
          if (i_ == 2 * N) {
            gen_rand_all();
          }
          uint64_t val = reinterpret_cast<uint64_t*>(u_)[i_++];
          return val % n;
        }

        uint32_t get_state_length_32(void) { return 4 * (N + 1); }
        void get_state(OT::UnsignedInteger * state) {
          // We don't know the size of an OT::UnsignedInteger, it can be 32 or 64 bits. We decide to
          // store only 32 bits values, even if it can handle 64 bits.
          for (OT::UnsignedInteger i = 0; i <= (uint32_t)(N); i++)
            {
              uint32_t words[4];
              memcpy(&words[0], &u_[i], 16);
#ifdef DSFMT_BIG_ENDIAN
              state[4 * i    ] = words[1];
              state[4 * i + 1] = words[0];
              state[4 * i + 2] = words[3];
              state[4 * i + 3] = words[2];
#else
              state[4 * i    ] = words[0];
              state[4 * i + 1] = words[1];
              state[4 * i + 2] = words[2];
              state[4 * i + 3] = words[3];
#endif
            }
        }

        void set_state(OT::UnsignedInteger * state) {
          // We don't know the size of an OT::UnsignedInteger, it can be 32 or 64 bits. We decide to
          // store only 32 bits values, even if it can handle 64 bits.
          for (OT::UnsignedInteger i = 0; i <= (uint32_t)(N); i++)
            {
              uint32_t words[4];
#ifdef DSFMT_BIG_ENDIAN
              words[1] = (uint32_t)(state[4 * i    ]);
              words[0] = (uint32_t)(state[4 * i + 1]);
              words[3] = (uint32_t)(state[4 * i + 2]);
              words[2] = (uint32_t)(state[4 * i + 3]);
#else
              words[0] = (uint32_t)(state[4 * i    ]);
              words[1] = (uint32_t)(state[4 * i + 1]);
              words[2] = (uint32_t)(state[4 * i + 2]);
              words[3] = (uint32_t)(state[4 * i + 3]);
#endif
              memcpy(static_cast<void*>(&u_[i]), &words[0], 16);
            }
        }

        uint32_t get_index(void) { return (uint32_t)(i_); }
        void set_index(uint32_t const index) { i_ = (int)(index); }

      private:
        uint64v2_t u_[N + 1];
        int i_;

        void init();
        void gen_rand_all();
      };

  }

  typedef detail::dsfmt<607> dsfmt607;
  typedef detail::dsfmt<1279> dsfmt1279;
  typedef detail::dsfmt<2281> dsfmt2281;
  typedef detail::dsfmt<4423> dsfmt4423;
  typedef detail::dsfmt<11213> dsfmt11213;
  typedef detail::dsfmt<19937> dsfmt19937;
  typedef detail::dsfmt<44497> dsfmt44497;
  typedef detail::dsfmt<86243> dsfmt86243;
  typedef detail::dsfmt<132049> dsfmt132049;
  typedef detail::dsfmt<216091> dsfmt216091;

}

#endif
