//                                               -*- C++ -*-
/**
 *  @brief Kolmogorov distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/KolmogorovFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/IdentityMatrix.hxx"

#include "kolmogorov.h"
#include <numeric>

BEGIN_NAMESPACE_OPENTURNS

namespace KolmogorovFunctions
{
// translated from scipy code that implement the [simard2011] selection strategy
// https://github.com/scipy/scipy/blob/main/scipy/stats/_ksstats.py

Scalar _log_nfactorial_div_n_pow_n(const UnsignedInteger n)
{
  const Point _STIRLING_COEFFS =
  {
    -2.955065359477124183e-2, 6.4102564102564102564e-3,
      -1.9175269175269175269e-3, 8.4175084175084175084e-4,
      -5.952380952380952381e-4, 7.9365079365079365079e-4,
      -2.7777777777777777778e-3, 8.3333333333333333333e-2
    };
  // Computes n! / n**n
  //   = (n-1)! / n**(n-1)
  // Uses Stirling's approximation, but removes n*log(n) up-front to
  // avoid subtractive cancellation.
  //    = log(n)/2 - n + log(sqrt(2pi)) + sum B_{2j}/(2j)/(2j-1)/n**(2j-1)
  const Scalar rn = 1.0 / n;
  return std::log(1.0 * n) / 2.0 - n + std::log(2.0 * M_PI) / 2.0 + rn * UniVariatePolynomial(_STIRLING_COEFFS).operator()(rn / n);
}

Scalar _select_and_clip_prob(const Scalar cdfprob, const Scalar sfprob, const Bool cdf)
{
  const Scalar p = cdf ? cdfprob : sfprob;
  return SpecFunc::Clip01(p);
}

const int _E128 = 128;
const Scalar _EP128 = std::ldexp(1.0, _E128);
const Scalar _EM128 = std::ldexp(1.0, -1 * _E128);
const Scalar _PI_SQUARED = M_PI * M_PI;
const Scalar _PI_FOUR = _PI_SQUARED * _PI_SQUARED;
const Scalar _PI_SIX = _PI_SQUARED * _PI_FOUR;
const Scalar _MIN_LOG = -708;
const Scalar _SQRT3 = std::sqrt(3.0);

Scalar _kolmogn_DMTW(const UnsignedInteger n, const Scalar d, const Bool cdf = true)
{
  if (d >= 1.0)
    return _select_and_clip_prob(1.0, 0.0, cdf);
  const Scalar nd = n * d;
  if (nd <= 0.5)
    return _select_and_clip_prob(0.0, 1.0, cdf);
  const UnsignedInteger k = std::ceil(nd);
  const Scalar h = k - nd;
  const UnsignedInteger m = 2 * k - 1;

  Matrix H(m, m);

  // Initialize: v is first column (and last row) of H
  //  v[j] = (1-h^(j+1)/(j+1)!  (except for v[-1])
  //  w[j] = 1/(j)!
  // q = k-th row of H (actually i!/n^i*H^i)
  Point v(m);
  for(UnsignedInteger j = 0; j < m; ++j)
    v[j] = 1.0 - std::pow(h, j + 1.0);
  Point w(m);
  Scalar fac = 1.0;
  for(UnsignedInteger j = 1; j <= m; ++j)
  {
    w[j - 1] = fac;
    fac /= j;  // This might underflow.  Isn't a problem.
    v[j - 1] *= fac;
  }
  const Scalar tt = std::pow(std::max(2 * h - 1.0, 0.0), m * 1.0) - 2.0 * std::pow(h, 1.0 * m);
  v[m - 1] = (1.0 + tt) * fac;
  for (UnsignedInteger i = 1; i < m; ++ i)
    for (UnsignedInteger j = 0; j < m - i + 1; ++ j)
      H(i - 1 + j, i) = w[j];
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    H(i, 0) = v[i];
    H(m - 1, i) = v[m - i - 1];
  }
  Matrix Hpwr = IdentityMatrix(m);
  UnsignedInteger nn = n;
  UnsignedInteger expnt = 0;  // Scaling of Hpwr
  UnsignedInteger Hexpnt = 0;  // Scaling of H
  while (nn > 0)
  {
    if (nn % 2)
    {
      Hpwr = Hpwr * H;
      expnt += Hexpnt;
    }
    H = H * H;
    Hexpnt *= 2;
    // Scale as needed.
    if (std::abs(H(k - 1, k - 1)) > _EP128)
    {
      H = (1.0 / _EP128) * H;
      Hexpnt += _E128;
    }
    nn /= 2;
  }
  Scalar p = Hpwr(k - 1, k - 1);
  // Multiply by n!/n^n
  for (UnsignedInteger i = 1; i < n + 1; ++ i)
  {
    p = i * p / n;
    if (std::abs(p) < _EM128)
    {
      p *= _EP128;
      expnt -= _E128;
    }
  }

  // unscale
  if (expnt != 0)
    p = std::ldexp(p, expnt);

  return _select_and_clip_prob(p, 1.0 - p, cdf);
}

Indices _pomeranz_compute_j1j2(const UnsignedInteger i,
                               const UnsignedInteger n,
                               const UnsignedInteger ll,
                               const UnsignedInteger ceilf,
                               const UnsignedInteger roundf)
{
  // Compute the endpoints of the interval for row i.
  SignedInteger j1 = 0;
  SignedInteger j2 = 0;
  if (i == 0)
  {
    j1 = -static_cast<SignedInteger>(ll) - ceilf - 1;
    j2 = ll + ceilf - 1;
  }
  else
  {
    // i + 1 = 2*ip1div2 + ip1mod2
    const UnsignedInteger ip1div2 = (i + 1) / 2;
    const UnsignedInteger ip1mod2 = (i + 1) % 2;
    if (ip1mod2 == 0)  // i is odd
    {
      if (ip1div2 == n + 1)
      {
        j1 = n - ll - ceilf - 1;
        j2 = n + ll + ceilf - 1;
      }
      else
      {
        j1 = ip1div2 - 1 - ll - roundf - 1;
        j2 = ip1div2 + ll - 1 + ceilf - 1;
      }
    }
    else
    {
      j1 = ip1div2 - 1 - ll - 1;
      j2 = ip1div2 + ll + roundf - 1;
    }
  }
  return {static_cast<UnsignedInteger>(std::max(j1 + 2, 0L)), static_cast<UnsignedInteger>(std::min(j2, static_cast<SignedInteger>(n)))};
}

Point convolve(const Point & u, const Point & v)
{
  const UnsignedInteger n1 = u.getSize();
  const UnsignedInteger n2 = v.getSize();
  Point w(n1 + n2 - 1);
  for (UnsignedInteger i = 0; i < n1; ++ i)
    for (UnsignedInteger j = 0; j < n2; ++ j)
      w[i + j] += u[i] * v[j];
  return w;
}

Scalar _kolmogn_Pomeranz(const UnsignedInteger n, const Scalar x, const Bool cdf = true)
{
  const Scalar t = n * x;
  const UnsignedInteger ll = std::floor(t);
  const Scalar f = 1.0 * (t - ll);  // fractional part of t
  const Scalar g = std::min(f, 1.0 - f);
  const UnsignedInteger ceilf = (f > 0.0) ? 1 : 0;
  const UnsignedInteger roundf = (f > 0.5) ? 1 : 0;
  const UnsignedInteger npwrs = 2 * (ll + 1);   // Maximum number of powers needed in convolutions
  Point gpower(npwrs);  // gpower = (g/n)^m/m!
  Point twogpower(npwrs);  // twogpower = (2g/n)^m/m!
  Point onem2gpower(npwrs);  // onem2gpower = ((1-2g)/n)^m/m!
  //  gpower etc are *almost* Poisson probs, just missing normalizing factor.

  gpower[0] = 1.0;
  twogpower[0] = 1.0;
  onem2gpower[0] = 1.0;
  UnsignedInteger expnt = 0;
  const Scalar g_over_n = g / n;
  const Scalar two_g_over_n = 2 * g / n;
  const Scalar one_minus_two_g_over_n = (1 - 2 * g) / n;

  for (UnsignedInteger m = 1; m < npwrs; ++ m)
  {
    gpower[m] = gpower[m - 1] * g_over_n / m;
    twogpower[m] = twogpower[m - 1] * two_g_over_n / m;
    onem2gpower[m] = onem2gpower[m - 1] * one_minus_two_g_over_n / m;
  }

  Point V0(npwrs);
  Point V1(npwrs);

  V1[0] = 1.0;  // first row
  UnsignedInteger V0s = 0;
  UnsignedInteger V1s = 0;  // start indices of the two rows

  Indices j1j2 = _pomeranz_compute_j1j2(0, n, ll, ceilf, roundf);
  UnsignedInteger j1 = j1j2[0];
  UnsignedInteger j2 = j1j2[1];

  for (UnsignedInteger i = 1; i < 2 * n + 2; ++ i)
  {
    // Preserve j1, V1, V1s, V0s from last iteration
    UnsignedInteger k1 = j1;
    std::swap(V0, V1);
    std::swap(V0s, V1s);
    V1 = Point(npwrs);
    j1j2 = _pomeranz_compute_j1j2(i, n, ll, ceilf, roundf);
    j1 = j1j2[0];
    j2 = j1j2[1];
    Point pwrs;
    if ((i == 1) || (i == 2 * n + 1))
      pwrs = gpower;
    else
      pwrs = (i % 2) ? twogpower : onem2gpower;
    const UnsignedInteger ln2 = j2 - k1 + 1;
    if (ln2 > 0)
    {
      Point u(ln2);
      std::copy(V0.begin() + k1 - V0s, V0.begin() + k1 - V0s + ln2, u.begin());
      Point v(ln2);
      std::copy(pwrs.begin(), pwrs.begin() + ln2, v.begin());
      const Point conv = convolve(u, v);
      const UnsignedInteger conv_start = j1 - k1;  // First index to use from conv
      const UnsignedInteger conv_len = j2 - j1 + 1;  // Number of entries to use from conv
      std::copy(conv.begin() + conv_start, conv.begin() + conv_start + conv_len, V1.begin());
      // Scale to avoid underflow.
      const Scalar v1max = *max_element(std::begin(V1), std::end(V1));
      if ((v1max > 0.0) && (v1max  < _EM128))
      {
        V1 *= _EP128;
        expnt -= _E128;
      }
      V1s = V0s + j1 - k1;
    }
  }

  // multiply by n!
  Scalar ans = V1[n - V1s];
  for (UnsignedInteger m = 1; m < n + 1; ++ m)
  {
    if (std::abs(ans) > _EP128)
    {
      ans *= _EM128;
      expnt += _E128;
    }
    ans *= m;
  }
  // Undo any intermediate scaling
  if (expnt != 0)
    ans = std::ldexp(ans, expnt);
  ans = _select_and_clip_prob(ans, 1.0 - ans, cdf);
  return ans;
}

Scalar _kolmogn_PelzGood(const UnsignedInteger n, const Scalar x, const Bool cdf = true)
{
  // Computes the Pelz-Good approximation to Prob(Dn <= x) with 0<=x<=1.

  // Start with Li-Chien, Korolyuk approximation:
  //  Prob(Dn <= x) ~ K0(z) + K1(z)/sqrt(n) + K2(z)/n + K3(z)/n**1.5
  // where z = x*sqrt(n).
  // Transform each K_(z) using Jacobi theta functions into a form suitable
  // for small z.
  // Pelz-Good (1976). [6]

  if (x <= 0.0)
    return _select_and_clip_prob(0.0, 1.0, cdf);
  if (x >= 1.0)
    return _select_and_clip_prob(1.0, 0.0, cdf);

  const Scalar z = std::sqrt(1.0 * n) * x;
  const Scalar zsquared = z * z;
  const Scalar zthree = std::pow(z, 3.0);
  const Scalar zfour = std::pow(z, 4.0);
  const Scalar zsix = std::pow(z, 6.0);
  const Scalar qlog = -_PI_SQUARED / 8.0 / zsquared;
  if (qlog < _MIN_LOG)  // z ~ 0.041743441416853426
    return _select_and_clip_prob(0.0, 1.0, cdf);

  Scalar q = std::exp(qlog);

  // Coefficients of terms in the sums for K1, K2 and K3
  const Scalar k1a = -zsquared;
  const Scalar k1b = _PI_SQUARED / 4;

  const Scalar k2a = 6.0 * zsix + 2 * zfour;
  const Scalar k2b = (2.0 * zfour - 5 * zsquared) * _PI_SQUARED / 4.0;
  const Scalar k2c = _PI_FOUR * (1 - 2.0 * zsquared) / 16.0;

  const Scalar k3d = _PI_SIX * (5 - 30 * zsquared) / 64.0;
  const Scalar k3c = _PI_FOUR * (-60 * zsquared + 212 * zfour) / 16.0;
  const Scalar k3b = _PI_SQUARED * (135 * zfour - 96 * zsix) / 4.0;
  const Scalar k3a = -30 * zsix - 90 * std::pow(z, 8.0);

  Point K0to3(4);
  // Use a Horner scheme to evaluate sum c_i q^(i^2)
  // Reduces to a sum over odd integers.
  UnsignedInteger maxk = std::ceil(16 * z / M_PI);
  for (UnsignedInteger k = maxk - 1; k > 0; -- k)
  {
    const UnsignedInteger m = 2 * k - 1;
    const UnsignedInteger msquared = m * m;
    const UnsignedInteger mfour = msquared * msquared;
    const UnsignedInteger msix = msquared * mfour;
    const Scalar qpower = std::pow(q, 8.0 * k);
    const Point coeffs = {1.0,
                          k1a + k1b * msquared,
                          k2a + k2b*msquared + k2c * mfour,
                          k3a + k3b*msquared + k3c*mfour + k3d * msix
                         };
    K0to3 *= qpower;
    K0to3 += coeffs;
  }
  K0to3 *= q;
  K0to3 *= SpecFunc::SQRT2PI;

  // z**10 > 0 as z > 0.04
  K0to3[0] /= z;
  K0to3[1] /= 6 * zfour;
  K0to3[2] /= 72 * std::pow(z, 7.0);
  K0to3[3] /= 6480 * std::pow(z, 10.0);

  // Now do the other sum over the other terms, all integers k
  // K_2:  (pi^2 k^2) q^(k^2),
  // K_3:  (3pi^2 k^2 z^2 - pi^4 k^4)*q^(k^2)
  // Don't expect much subtractive cancellation so use direct calculation
  q = std::exp(-_PI_SQUARED / 2 / zsquared);
  Scalar k2extra = 0.0;
  const Scalar sqrt3z = _SQRT3 * z;
  Scalar k3extra = 0.0;
  for (UnsignedInteger k = maxk - 1; k > 0; -- k)
  {
    const Scalar ksquared = k * k;
    const Scalar qpwers = std::pow(q, ksquared);
    k2extra += ksquared * qpwers;
    const Scalar kspi = M_PI * k;
    k3extra += (sqrt3z + kspi) * (sqrt3z + kspi) * ksquared * qpwers;
  }
  k2extra *= _PI_SQUARED * SpecFunc::SQRT2PI / (-36 * zthree);
  k3extra *= _PI_SQUARED * SpecFunc::SQRT2PI / (216 * zsix);
  K0to3[2] += k2extra;
  K0to3[3] += k3extra;
  for (UnsignedInteger k = 0; k < K0to3.getSize(); ++ k)
    K0to3[k] /= std::pow(n * 1.0, k / 2.0);

  if (!cdf)
  {
    K0to3 *= -1.0;
    K0to3[0] += 1.0;
  }
  const Scalar Ksum = std::accumulate(K0to3.begin(), K0to3.end(), 0.0);
  return Ksum;
}

Scalar _kolmogn(const UnsignedInteger n, const Scalar x, const Bool cdf)
{
  // if np.isnan(n):
  // return n  // Keep the same type of nan
  // if int(n) != n or n <= 0:
  // return np.nan
  if (x >= 1.0)
    return _select_and_clip_prob(1.0, 0.0, cdf);
  if (x <= 0.0)
    return _select_and_clip_prob(0.0, 1.0, cdf);
  const Scalar t = n * x;
  Scalar prob = -1.0;
  if (t <= 1.0)  // Ruben-Gambino: 1/2n <= x <= 1/n
  {
    if (t <= 0.5)
      return _select_and_clip_prob(0.0, 1.0, cdf);
    if (n <= 140)
    {
      prob = 1.0;
      for (UnsignedInteger i = 1; i <= n; ++ i)
        prob *= (1.0 * i) * (1.0 / n) * (2.0 * t - 1.0);
    }
    else
      prob = std::exp(_log_nfactorial_div_n_pow_n(n) + n * std::log(2 * t - 1));
    return _select_and_clip_prob(prob, 1.0 - prob, cdf);
  }
  if (t >= n - 1)  // Ruben-Gambino
  {
    prob = 2.0 * std::pow(1.0 - x, 1.0 * n);
    return _select_and_clip_prob(1 - prob, prob, cdf);
  }
  if (x >= 0.5)  // Exact: 2 * smirnov
  {
    prob = 2 * special::cephes::smirnov(n, x);
    return _select_and_clip_prob(1.0 - prob, prob, cdf);
  }
  const Scalar nxsquared = t * x;
  if (n <= 140)
  {
    if (nxsquared <= 0.754693)
    {
      prob = _kolmogn_DMTW(n, x, true);
      return _select_and_clip_prob(prob, 1.0 - prob, cdf);
    }
    if (nxsquared <= 4)
    {
      prob = _kolmogn_Pomeranz(n, x, true);
      return _select_and_clip_prob(prob, 1.0 - prob, cdf);
      // Now use Miller approximation of 2*smirnov
      prob = 2 * special::cephes::smirnov(n, x);
      return _select_and_clip_prob(1.0 - prob, prob, cdf);
    }
  }
  // Split CDF and SF as they have different cutoffs on nxsquared.
  if (!cdf)
  {
    if (nxsquared >= 370.0)
      return 0.0;
    if (nxsquared >= 2.2)
    {
      prob = 2 * special::cephes::smirnov(n, x);
      return SpecFunc::Clip01(prob);
    }
    // Fall through and compute the SF as 1.0-CDF
  }
  Scalar cdfprob = -1.0;
  if (nxsquared >= 18.0)
    cdfprob = 1.0;
  else if ((n <= 100000) && (n * std::pow(x, 1.5) <= 1.4))
    cdfprob = _kolmogn_DMTW(n, x, true);
  else
    cdfprob = _kolmogn_PelzGood(n, x, true);
  return _select_and_clip_prob(cdfprob, 1.0 - cdfprob, cdf);
}

} // KolmogorovFunctions

END_NAMESPACE_OPENTURNS
