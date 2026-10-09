//                                               -*- C++ -*-
/**
 *  @brief Fast Gauss-Jacobi quadrature via polished tridiagonal eigensolver
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FastJacobi.hxx"
#include "openturns/FastGaussQuadrature.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastJacobi
 *
 * Fast Gauss-Jacobi quadrature via the polished tridiagonal eigensolver
 * for small rules, and the Hale-Townsend asymptotic expansions
 * (interior Hahn expansion, boundary Bessel expansion, Newton refinement
 * in theta) for large rules, dispatched on the
 * FastJacobi-AsymptoticThreshold ResourceMap key (default 100).
 *
 * The symmetric recurrence for orthonormal Jacobi polynomials with
 * weight (1-x)^alpha (1+x)^beta on [-1,1] is:
 *   gamma_j = (beta^2 - alpha^2) / ((2j+alpha+beta)(2j+alpha+beta+2))
 *   b_j^2 = 4j(j+alpha)(j+beta)(j+alpha+beta) / ((2j+alpha+beta)^2*(2j+alpha+beta+1)*(2j+alpha+beta-1))
 * with b_0 = 0.
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 *
 * Reference: Hale, N. and Townsend, A. (2013).
 *            Fast and accurate computation of Gauss-Legendre and
 *            Gauss-Jacobi quadrature nodes and weights.
 *            SIAM J. Sci. Comput. 35(2), A652-A674.
 */
/**
 * @namespace FastJacobi details
 *
 * Asymptotic Hale-Townsend path (interior Hahn expansion + boundary Bessel
 * expansion, Newton refinement in theta with x = cos(theta)).
 * Reimplemented from the equations of Hale, N. and Townsend, A. (2013).
 * Fast and accurate computation of Gauss-Legendre and Gauss-Jacobi
 * quadrature nodes and weights. SIAM J. Sci. Comput. 35(2), A652-A674.
 */

// Number of terms in the interior Hahn expansion, (3.22)
static const UnsignedInteger JacobiInteriorTerms = 20;
// Boundary nodes refined per side by the Bessel expansion. Oracle sweep
// (mpmath reference, weight error relative to maximum, bar 5e-13): counts
// 4 to 12 are equivalent in the operating regime (worst 8.7e-15), while 16
// and above degrade moderate-n rules by overwriting accurate interior
// values. Cost differences are one-time milliseconds per cached rule, hence
// no cost basis to move; 10 is kept mid-plateau as the campaign-validated
// value.
static const UnsignedInteger JacobiBoundaryNodes = 10;

// Zeros of J_a by McMahon starts (NIST 10.21.19) + Newton refinement
static Point BesselZeros(const Scalar a,
                         const UnsignedInteger npts)
{
  Point zeros(npts);
  const Scalar mu = 4.0 * a * a;
  for (UnsignedInteger k = 1; k <= npts; ++k)
  {
    const Scalar beta = (k + 0.5 * a - 0.25) * M_PI;
    Scalar z = beta - (mu - 1.0) / (8.0 * beta)
      - 4.0 * (mu - 1.0) * (7.0 * mu - 31.0) / (3.0 * std::pow(8.0 * beta, 3));
    for (UnsignedInteger iter = 0; iter < 10; ++iter)
    {
      const Scalar step = SpecFunc::BesselJ(a, z) / SpecFunc::BesselJDerivative(a, z);
      z -= step;
      if (std::abs(step) <= 1.0e-13 * (1.0 + std::abs(z))) break;
    }
    zeros[k - 1] = z;
  }
  return zeros;
}

// Ascending 2nd-kind Chebyshev points on [0, c] with barycentric weights;
// differentiation/integration matrices below act from t[0] == 0
static void ChebyshevGrid(const Scalar c,
                          const UnsignedInteger N,
                          Point & t,
                          Point & v)
{
  t = Point(N);
  v = Point(N, 1.0);
  for (UnsignedInteger i = 0; i < N; ++i)
    t[i] = 0.5 * c * (std::sin(-0.5 * M_PI + M_PI * i / (N - 1)) + 1.0);
  for (UnsignedInteger i = 1; i < N; i += 2)
    v[i] = -1.0;
  v[0] *= 0.5;
  v[N - 1] *= 0.5;
}

static Matrix ChebyshevDifferentiationMatrix(const Point & t)
{
  const UnsignedInteger n = t.getSize();
  Point c(n, 1.0);
  c[0] = 2.0;
  c[n - 1] = 2.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    if (i % 2 == 1) c[i] = -c[i];
  Matrix D(n, n);
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = 0; j < n; ++j)
    {
      if (i == j) continue;
      D(i, j) = (c[i] / c[j]) / (t[i] - t[j]);
    }
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    Scalar rowSum = 0.0;
    for (UnsignedInteger j = 0; j < n; ++j)
      rowSum += D(i, j);
    D(i, i) = -rowSum;
  }
  return D;
}

static Matrix ChebyshevIntegrationMatrix(const Point & t)
{
  const UnsignedInteger n = t.getSize();
  const Matrix D(ChebyshevDifferentiationMatrix(t));
  Matrix sub(n - 1, n - 1);
  for (UnsignedInteger i = 0; i < n - 1; ++i)
    for (UnsignedInteger j = 0; j < n - 1; ++j)
      sub(i, j) = D(i + 1, j + 1);
  Matrix identity(n - 1, n - 1);
  for (UnsignedInteger i = 0; i < n - 1; ++i)
    identity(i, i) = 1.0;
  const Matrix invSub(sub.solveLinearSystem(identity));
  Matrix C(n, n);
  for (UnsignedInteger i = 0; i < n - 1; ++i)
    for (UnsignedInteger j = 0; j < n - 1; ++j)
      C(i + 1, j + 1) = invSub(i, j);
  return C;
}

static Scalar BarycentricInterpolate(const Scalar theta,
                                     const Point & t,
                                     const Point & f,
                                     const Point & v)
{
  const UnsignedInteger n = t.getSize();
  Scalar numerator = 0.0;
  Scalar denominator = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    const Scalar d = theta - t[i];
    if (std::abs(d) < 1.0e-15) return f[i];
    const Scalar w = v[i] / d;
    numerator += w * f[i];
    denominator += w;
  }
  return numerator / denominator;
}

// Exact binomial coefficient for small arguments (kmax <= 30)
static UnsignedInteger SmallBinomial(const UnsignedInteger n,
                                     const UnsignedInteger k)
{
  UnsignedInteger result = 1;
  const UnsignedInteger kk = std::min(k, n - k);
  for (UnsignedInteger i = 1; i <= kk; ++i)
    result = result * (n - kk + i) / i;
  return result;
}

// Accurate J_a(z + t) by Taylor expansion about z (NIST 10.6.7)
static Scalar BesselTaylor(const Scalar t,
                           const Scalar z,
                           const Scalar a)
{
  if (!(std::abs(t) < 1.0) || (t == 0.0)) return SpecFunc::BesselJ(a, z + t);
  const UnsignedInteger kmax = std::min(static_cast<UnsignedInteger>(std::ceil(std::abs(std::log(SpecFunc::ScalarEpsilon) / std::log(std::abs(t))))), static_cast<UnsignedInteger>(30));
  // Taylor coefficients from the Bessel recurrence on orders a-kmax..a+kmax
  Point AA(kmax + 1, 0.0);
  AA[0] = SpecFunc::BesselJ(a, z);
  Scalar factorial = 1.0;
  for (UnsignedInteger k = 1; k <= kmax; ++k)
  {
    Scalar column = 0.0;
    Scalar sign = 1.0;
    for (UnsignedInteger ell = 0; ell <= k; ++ell)
    {
      column += sign * SmallBinomial(k, ell) * SpecFunc::BesselJ(a + 2.0 * static_cast<Scalar>(ell) - static_cast<Scalar>(k), z);
      sign = -sign;
    }
    factorial *= k;
    AA[k] = column / std::pow(2.0, static_cast<Scalar>(k)) / factorial;
  }
  Scalar result = 0.0;
  Scalar power = 1.0;
  for (UnsignedInteger k = 0; k <= kmax; ++k)
  {
    result += AA[k] * power;
    power *= t;
  }
  return result;
}

// Tables of the boundary-expansion higher coefficients, interpolated
// barycentrically at evaluation thetas
struct JacobiBoundaryTables
{
  Point tGrid;
  Point tB1;
  Point A2;
  Point tB2;
  Point A3;
  Point baryV;
};

static JacobiBoundaryTables BuildBoundaryTables(const Scalar alpha,
                                                const Scalar beta,
                                                const Scalar thetaMax,
                                                const UnsignedInteger n)
{
  const Scalar A = 0.25 - alpha * alpha;
  const Scalar B = 0.25 - beta * beta;
  const Scalar c = std::max(thetaMax, 0.5);
  UnsignedInteger N = 10;
  if (n < 30) N = static_cast<UnsignedInteger>(std::ceil(40.0 - n));
  else if (c > M_PI / 2.0 - 0.5) N = 15;
  JacobiBoundaryTables tables;
  ChebyshevGrid(c, N, tables.tGrid, tables.baryV);
  const Point & t = tables.tGrid;
  const Matrix D(ChebyshevDifferentiationMatrix(t));
  const Matrix C(ChebyshevIntegrationMatrix(t));
  const UnsignedInteger Npts = t.getSize();
  Point g(Npts);
  Point gp(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    if (i == 0)
    {
      g[i] = 0.0;
      gp[i] = -A / 6.0 - 0.5 * B;
      continue;
    }
    g[i] = A * (1.0 / std::tan(t[i] / 2.0) - 2.0 / t[i]) - B * std::tan(t[i] / 2.0);
    gp[i] = A * (2.0 / (t[i] * t[i]) - 0.5 / std::pow(std::sin(t[i] / 2.0), 2)) - 0.5 * (0.25 - beta * beta) / std::pow(std::cos(t[i] / 2.0), 2);
  }
  Point B0(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    B0[i] = (i == 0) ? 0.25 * (-A / 6.0 - 0.5 * B) : 0.25 * g[i] / t[i];
  const Scalar A10 = alpha * (A + 3.0 * B) / 24.0;
  Point A1(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A1[i] = 0.125 * gp[i] - (1.0 + 2.0 * alpha) / 2.0 * B0[i] - g[i] * g[i] / 32.0 - A10;
  // T-fraction part, Taylor form at small t, exact form above t > 0.5
  Point fcos(Npts);
  Point f(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    fcos[i] = B / std::pow(2.0 * std::cos(t[i] / 2.0), 2);
    const Scalar t2 = t[i] * t[i];
    f[i] = -A * (1.0 / 12.0 + t2 / 240.0 + t2 * t2 / 6048.0 + t2 * t2 * t2 / 172800.0
                  + t2 * t2 * t2 * t2 / 5322240.0 + 691.0 * t2 * t2 * t2 * t2 * t2 / 118879488000.0
                  + t2 * t2 * t2 * t2 * t2 * t2 / 5748019200.0);
  }
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    if (t[i] <= 0.5) continue;
    const Scalar ti = t[i];
    f[i] = A * (1.0 / (ti * ti) - 1.0 / (2.0 * std::pow(std::sin(ti / 2.0), 2)));
  }
  for (UnsignedInteger i = 0; i < Npts; ++i)
    f[i] -= fcos[i];
  // A1p/t with the t = 0 limit
  Point A1p(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
      s += D(i, j) * A1[j];
    A1p[i] = s;
  }
  Point A1p_t(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A1p_t[i] = (i == 0) ? (-A / 720.0 - A * A / 576.0 - A * B / 96.0 - B * B / 64.0 - B / 48.0 + alpha * (A / 720.0 + B / 48.0)) : A1p[i] / t[i];
  Point I(Npts, 0.0);
  Point J(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar si = 0.0;
    Scalar sj = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
    {
      si += C(i, j) * A1p_t[j];
      sj += C(i, j) * (f[j] * A1[j]);
    }
    I[i] = si;
    J[i] = sj;
  }
  Point tB1(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    tB1[i] = (i == 0) ? 0.0 : -0.5 * A1p[i] - (0.5 + alpha) * I[i] + 0.5 * J[i];
  Point B1(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    B1[i] = (i == 0) ? (A / 720.0 + A * A / 576.0 + A * B / 96.0 + B * B / 64.0 + B / 48.0 + alpha * (A * A / 576.0 + B * B / 64.0 + A * B / 96.0) - alpha * alpha * (A / 720.0 + B / 48.0)) : tB1[i] / t[i];
  Point K(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
      s += C(i, j) * (f[j] * tB1[j]);
    K[i] = s;
  }
  Point DtB1(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
      s += D(i, j) * tB1[j];
    DtB1[i] = s;
  }
  Point A2(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A2[i] = 0.5 * DtB1[i] - (0.5 + alpha) * B1[i] - 0.5 * K[i];
  const Scalar A2zero = A2[0];
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A2[i] -= A2zero;
  Point A2p(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
      s += D(i, j) * A2[j];
    A2p[i] = s;
  }
  const Scalar A2pZero = A2p[0];
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A2p[i] -= A2pZero;
  // Extrapolated A2p/t at t = 0
  Scalar wSum = 0.0;
  for (UnsignedInteger i = 1; i < Npts; ++i)
  {
    Scalar w = M_PI / 2.0 - t[i];
    if (i % 2 == 1) w = -w;
    if (i == Npts - 1) w *= 0.5;
    wSum += w;
  }
  Point A2p_t(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A2p_t[i] = A2p[i] / t[i];
  Scalar extrap = 0.0;
  for (UnsignedInteger i = 1; i < Npts; ++i)
  {
    Scalar w = M_PI / 2.0 - t[i];
    if (i % 2 == 1) w = -w;
    if (i == Npts - 1) w *= 0.5;
    extrap += w * A2p_t[i];
  }
  A2p_t[0] = extrap / wSum;
  Point CA2p_t(Npts, 0.0);
  Point CfA2(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s1 = 0.0;
    Scalar s2 = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
    {
      s1 += C(i, j) * A2p_t[j];
      s2 += C(i, j) * (f[j] * A2[j]);
    }
    CA2p_t[i] = s1;
    CfA2[i] = s2;
  }
  Point tB2(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    tB2[i] = -0.5 * A2p[i] - (0.5 + alpha) * CA2p_t[i] + 0.5 * CfA2[i];
  Point B2(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
    B2[i] = tB2[i] / t[i];
  extrap = 0.0;
  for (UnsignedInteger i = 1; i < Npts; ++i)
  {
    Scalar w = M_PI / 2.0 - t[i];
    if (i % 2 == 1) w = -w;
    if (i == Npts - 1) w *= 0.5;
    extrap += w * B2[i];
  }
  B2[0] = extrap / wSum;
  Point K2(Npts, 0.0);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
      s += C(i, j) * (f[j] * tB2[j]);
    K2[i] = s;
  }
  Point A3(Npts);
  for (UnsignedInteger i = 0; i < Npts; ++i)
  {
    Scalar s = 0.0;
    for (UnsignedInteger j = 0; j < Npts; ++j)
      s += D(i, j) * tB2[j];
    A3[i] = 0.5 * s - (0.5 + alpha) * B2[i] - 0.5 * K2[i];
  }
  const Scalar A3zero = A3[0];
  for (UnsignedInteger i = 0; i < Npts; ++i)
    A3[i] -= A3zero;
  tables.tGrid = t;
  tables.tB1 = tB1;
  tables.A2 = A2;
  tables.tB2 = tB2;
  tables.A3 = A3;
  tables.baryV = Point(Npts);
  tables.baryV[0] = 0.5;
  for (UnsignedInteger i = 1; i < Npts - 1; ++i)
    tables.baryV[i] = (i % 2 == 1) ? -1.0 : 1.0;
  tables.baryV[Npts - 1] = ((Npts - 1) % 2 == 1) ? -0.5 : 0.5;
  return tables;
}

// Interior Hahn expansion with M terms, (3.22)
static Bool EvaluateInteriorAsymptotics(const UnsignedInteger n,
                                        const Scalar alpha,
                                        const Scalar beta,
                                        const Point & theta,
                                        Point & vals,
                                        Point & ders)
{
  const UnsignedInteger M = JacobiInteriorTerms;
  const UnsignedInteger nt = theta.getSize();
  Matrix alphaMat(M, nt);
  for (UnsignedInteger m = 0; m < M; ++m)
    for (UnsignedInteger i = 0; i < nt; ++i)
      alphaMat(m, i) = 0.5 * (2.0 * n + alpha + beta + 1.0 + m) * theta[i] - 0.5 * (alpha + 0.5) * M_PI;
  Matrix cosA(M, nt);
  Matrix sinA(M, nt);
  for (UnsignedInteger m = 0; m < M; ++m)
    for (UnsignedInteger i = 0; i < nt; ++i)
    {
      cosA(m, i) = std::cos(alphaMat(m, i));
      sinA(m, i) = std::sin(alphaMat(m, i));
    }
  Matrix sinT(M, nt);
  Matrix cosT(M, nt);
  for (UnsignedInteger m = 0; m < M; ++m)
    for (UnsignedInteger i = 0; i < nt; ++i)
    {
      sinT(m, i) = std::sin(theta[i]);
      cosT(m, i) = std::cos(theta[i]);
    }
  Matrix cosA2(M, nt);
  Matrix sinA2(M, nt);
  for (UnsignedInteger m = 0; m < M; ++m)
    for (UnsignedInteger i = 0; i < nt; ++i)
    {
      cosA2(m, i) = cosA(m, i) * cosT(m, i) + sinA(m, i) * sinT(m, i);
      sinA2(m, i) = sinA(m, i) * cosT(m, i) - cosA(m, i) * sinT(m, i);
    }
  // SC(m, i) = (0.5*csc(theta[i]/2))^m
  Matrix SC(M, nt);
  for (UnsignedInteger i = 0; i < nt; ++i)
  {
    SC(0, i) = 1.0;
    const Scalar base = 0.5 / std::sin(theta[i] / 2.0);
    for (UnsignedInteger m = 1; m < M; ++m)
      SC(m, i) = SC(m - 1, i) * base;
  }
  Point cosTsc(nt);
  for (UnsignedInteger i = 0; i < nt; ++i)
    cosTsc[i] = 0.5 / std::cos(theta[i] / 2.0);
  // Coefficient tables PHI (degree n) and PHI2 (degree n - 1)
  Matrix PHI(M, M);
  Matrix PHI2(M, M);
  for (UnsignedInteger pass = 0; pass < 2; ++pass)
  {
    const UnsignedInteger nn = (pass == 0) ? n : n - 1;
    Point P1(M);
    P1[0] = 1.0;
    Scalar cum = 1.0;
    for (UnsignedInteger j = 0; j < M - 1; ++j)
    {
      cum *= (0.5 + alpha + j) * (0.5 - alpha + j) / ((j + 1.0) * (2.0 * nn + alpha + beta + j + 2.0));
      P1[j + 1] = cum;
    }
    for (UnsignedInteger j = 2; j < M; j += 4)
      P1[j] = -P1[j];
    for (UnsignedInteger j = 3; j < M; j += 4)
      P1[j] = -P1[j];
    Matrix P2(M, M);
    for (UnsignedInteger i = 0; i < M; ++i)
      P2(i, i) = 1.0;
    for (UnsignedInteger ell = 0; ell < M; ++ell)
    {
      cum = 1.0;
      for (UnsignedInteger j = 0; j + ell + 1 < M; ++j)
      {
        cum *= (0.5 + beta + j) * (0.5 - beta + j) / ((j + 1.0) * (2.0 * nn + alpha + beta + j + ell + 2.0));
        P2(ell + 1 + j, ell) = cum;
      }
    }
    Matrix & PHItarget = (pass == 0) ? PHI : PHI2;
    for (UnsignedInteger m = 0; m < M; ++m)
      for (UnsignedInteger ell = 0; ell < M; ++ell)
        PHItarget(m, ell) = P1[ell] * P2(m, ell);
  }
  vals = Point(nt, 0.0);
  Point S2vals(nt, 0.0);
  for (UnsignedInteger m = 0; m < M; ++m)
  {
    for (UnsignedInteger i = 0; i < nt; ++i)
    {
      Scalar dS1 = 0.0;
      Scalar dS2odd = 0.0;
      Scalar dS12 = 0.0;
      Scalar dS22odd = 0.0;
      for (UnsignedInteger ell = 0; ell <= m; ell += 2)
      {
        dS1 += PHI(m, ell) * SC(ell, i) * cosA(m, i);
        dS12 += PHI2(m, ell) * SC(ell, i) * cosA2(m, i);
      }
      for (UnsignedInteger ell = 1; ell <= m; ell += 2)
      {
        dS2odd += PHI(m, ell) * SC(ell, i) * sinA(m, i);
        dS22odd += PHI2(m, ell) * SC(ell, i) * sinA2(m, i);
      }
      vals[i] += dS1 + dS2odd;
      S2vals[i] += dS12 + dS22odd;
    }
    for (UnsignedInteger mm = 0; mm <= m; ++mm)
      for (UnsignedInteger i = 0; i < nt; ++i)
        SC(mm, i) *= cosTsc[i];
  }
  // Front constant: log-ratio series + Stirling factors. The series
  // contracts for moderate parameters; cap the iterations and signal
  // failure so the caller falls back instead of hanging.
  Scalar dsa = 0.5 * alpha * alpha / n;
  Scalar dsb = 0.5 * beta * beta / n;
  Scalar dsab = 0.25 * (alpha + beta) * (alpha + beta) / n;
  Scalar ds = dsa + dsb - dsab;
  Scalar s = ds;
  UnsignedInteger j = 1;
  Scalar dsold = std::abs(ds);
  UnsignedInteger guard = 0;
  while ((s != 0.0) && (std::abs(ds / s) + dsold > SpecFunc::ScalarEpsilon / 10.0))
  {
    if (++guard > 10000) return false;
    dsold = std::abs(ds / s);
    ++j;
    const Scalar tmp = -(j - 1.0) / (j + 1.0) / n;
    dsa = tmp * dsa * alpha;
    dsb = tmp * dsb * beta;
    dsab = 0.5 * tmp * dsab * (alpha + beta);
    ds = dsa + dsb - dsab;
    s = s + ds;
  }
  const Scalar p2 = std::exp(s) * std::sqrt(2.0 * M_PI) * std::sqrt((n + alpha) * (n + beta) / (2.0 * n + alpha + beta)) / (2.0 * n + alpha + beta + 1.0);
  const Scalar stirlingG[10] = {1.0, 1.0 / 12.0, 1.0 / 288.0, -139.0 / 51840.0, -571.0 / 2488320.0, 163879.0 / 209018880.0, 5246819.0 / 75246796800.0, -534703531.0 / 902961561600.0, -4483131259.0 / 86684309913600.0, 432261921612371.0 / 514904800886784000.0};
  // Stirling-like factor f(z) = sum_k g_k / z^k at n+a, n+b, 2n+a+b
  Scalar fNa = 0.0;
  Scalar fNb = 0.0;
  Scalar fNab = 0.0;
  {
    Scalar powA = 1.0;
    Scalar powB = 1.0;
    Scalar powAB = 1.0;
    for (UnsignedInteger k = 0; k < 10; ++k)
    {
      fNa += stirlingG[k] * powA;
      fNb += stirlingG[k] * powB;
      fNab += stirlingG[k] * powAB;
      powA /= (n + alpha);
      powB /= (n + beta);
      powAB /= (2.0 * n + alpha + beta);
    }
  }
  const Scalar C = p2 * (fNa * fNb / fNab) * 2.0 / M_PI;
  const Scalar C2 = C * (alpha + beta + 2.0 * n) * (alpha + beta + 1.0 + 2.0 * n) / (4.0 * (alpha + n) * (beta + n));
  ders = Point(nt, 0.0);
  for (UnsignedInteger i = 0; i < nt; ++i)
  {
    const Scalar v = C * vals[i];
    const Scalar vv = C2 * S2vals[i];
    Scalar dd = (n * (alpha - beta - (2.0 * n + alpha + beta) * std::cos(theta[i])) * v + 2.0 * (n + alpha) * (n + beta) * vv) / (2.0 * n + alpha + beta) / std::sin(theta[i]);
    const Scalar denom = 1.0 / (std::pow(std::sin(theta[i] / 2.0), alpha + 0.5) * std::pow(std::cos(theta[i] / 2.0), beta + 0.5));
    vals[i] = v * denom;
    ders[i] = dd * denom;
  }
  return true;
}

// Boundary Bessel expansion with higher collocation terms
static Bool EvaluateBoundaryAsymptotics(const UnsignedInteger n,
                                        const Scalar alpha,
                                        const Scalar beta,
                                        const Point & theta,
                                        const JacobiBoundaryTables & tables,
                                        const Bool final,
                                        Point & vals,
                                        Point & ders)
{
  const UnsignedInteger nt = theta.getSize();
  const Scalar rho = n + 0.5 * (alpha + beta + 1.0);
  const Scalar rho2 = n + 0.5 * (alpha + beta - 1.0);
  const Scalar A = 0.25 - alpha * alpha;
  const Scalar B = 0.25 - beta * beta;
  vals = Point(nt);
  ders = Point(nt);
  for (UnsignedInteger i = 0; i < nt; ++i)
  {
    const Scalar t = theta[i];
    const Scalar Ja = SpecFunc::BesselJ(alpha, rho * t);
    const Scalar Jb = SpecFunc::BesselJ(alpha + 1.0, rho * t);
    const Scalar Jbb = SpecFunc::BesselJ(alpha + 1.0, rho2 * t);
    const Scalar Jab = final ? BesselTaylor(-t, rho * t, alpha) : SpecFunc::BesselJ(alpha, rho2 * t);
    const Scalar gt = A * (1.0 / std::tan(t / 2.0) - 2.0 / t) - B * std::tan(t / 2.0);
    const Scalar gtdx = A * (2.0 / (t * t) - 0.5 / std::pow(std::sin(t / 2.0), 2)) - 0.5 * B / std::pow(std::cos(t / 2.0), 2);
    const Scalar tB0 = 0.25 * gt;
    const Scalar A10 = alpha * (A + 3.0 * B) / 24.0;
    const Scalar A1 = gtdx / 8.0 - (1.0 + 2.0 * alpha) / 8.0 * gt / t - gt * gt / 32.0 - A10;
    const Scalar tB1t = BarycentricInterpolate(t, tables.tGrid, tables.tB1, tables.baryV);
    const Scalar A2t = BarycentricInterpolate(t, tables.tGrid, tables.A2, tables.baryV);
    const Scalar tB2t = BarycentricInterpolate(t, tables.tGrid, tables.tB2, tables.baryV);
    const Scalar A3t = BarycentricInterpolate(t, tables.tGrid, tables.A3, tables.baryV);
    const Scalar v = Ja + Jb * tB0 / rho + Ja * A1 / (rho * rho) + Jb * tB1t / (rho * rho * rho) + Ja * A2t / (rho * rho * rho * rho) + Jb * tB2t / std::pow(rho, 5) + Ja * A3t / std::pow(rho, 6);
    const Scalar v2 = Jab + Jbb * tB0 / rho2 + Jab * A1 / (rho2 * rho2) + Jbb * tB1t / (rho2 * rho2 * rho2) + Jab * A2t / (rho2 * rho2 * rho2 * rho2) + Jbb * tB2t / std::pow(rho2, 5) + Jab * A3t / std::pow(rho2, 6);
    Scalar ds = 0.5 * alpha * alpha / n;
    Scalar s = ds;
    UnsignedInteger jj = 1;
    UnsignedInteger guard = 0;
    while ((s != 0.0) && (std::abs(ds / s) > SpecFunc::ScalarEpsilon / 10.0))
    {
      if (++guard > 10000) return false;
      ++jj;
      ds = -(jj - 1.0) / (jj + 1.0) / n * (ds * alpha);
      s = s + ds;
    }
    const Scalar p2 = std::exp(s) * std::sqrt((n + alpha) / n) * std::pow(n / rho, alpha);
    const Scalar stirlingG[10] = {1.0, 1.0 / 12.0, 1.0 / 288.0, -139.0 / 51840.0, -571.0 / 2488320.0, 163879.0 / 209018880.0, 5246819.0 / 75246796800.0, -534703531.0 / 902961561600.0, -4483131259.0 / 86684309913600.0, 432261921612371.0 / 514904800886784000.0};
    Scalar fNa = 0.0;
    Scalar fN = 0.0;
    {
      Scalar powA = 1.0;
      Scalar powN = 1.0;
      for (UnsignedInteger k = 0; k < 10; ++k)
      {
        fNa += stirlingG[k] * powA;
        fN += stirlingG[k] * powN;
        powA /= (n + alpha);
        powN /= n;
      }
    }
    const Scalar Cc = p2 * (fNa / fN) / std::sqrt(2.0);
    const Scalar valstmp = Cc * v;
    const Scalar denom = std::pow(std::sin(t / 2.0), alpha + 0.5) * std::pow(std::cos(t / 2.0), beta + 0.5);
    vals[i] = std::sqrt(t) * valstmp / denom;
    const Scalar C2 = Cc * n / (n + alpha) * std::pow(rho / rho2, alpha);
    Scalar dd = (n * (alpha - beta - (2.0 * n + alpha + beta) * std::cos(t)) * valstmp + 2.0 * (n + alpha) * (n + beta) * C2 * v2) / (2.0 * n + alpha + beta);
    ders[i] = dd * (std::sqrt(t) / (denom * std::sin(t)));
  }
  return true;
}

// Newton refinement of a theta block with an evaluator callback
struct JacobiThetaEvaluator
{
  virtual ~JacobiThetaEvaluator() {}
  virtual Bool evaluate(const Point & theta, Point & vals, Point & ders) const = 0;
};

struct JacobiInteriorEvaluator: public JacobiThetaEvaluator
{
  JacobiInteriorEvaluator(const UnsignedInteger n,
                          const Scalar alpha,
                          const Scalar beta)
    : n_(n), alpha_(alpha), beta_(beta) {}
  Bool evaluate(const Point & theta, Point & vals, Point & ders) const override
  {
    return EvaluateInteriorAsymptotics(n_, alpha_, beta_, theta, vals, ders);
  }
  UnsignedInteger n_;
  Scalar alpha_;
  Scalar beta_;
};

struct JacobiBoundaryEvaluator: public JacobiThetaEvaluator
{
  JacobiBoundaryEvaluator(const UnsignedInteger n,
                          const Scalar alpha,
                          const Scalar beta,
                          const JacobiBoundaryTables & tables,
                          const Bool final)
    : n_(n), alpha_(alpha), beta_(beta), tables_(tables), final_(final) {}
  Bool evaluate(const Point & theta, Point & vals, Point & ders) const override
  {
    return EvaluateBoundaryAsymptotics(n_, alpha_, beta_, theta, tables_, final_, vals, ders);
  }
  UnsignedInteger n_;
  Scalar alpha_;
  Scalar beta_;
  const JacobiBoundaryTables & tables_;
  Bool final_;
};

static Scalar MaxAbsStep(const Point & step,
                         const Point & subset,
                         const UnsignedInteger n)
{
  Scalar worst = 0.0;
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    const UnsignedInteger i = static_cast<UnsignedInteger>(subset[k]);
    worst = std::max(worst, std::abs(step[i]));
  }
  return worst;
}

static Bool NewtonRefineTheta(Point & theta,
                              const JacobiThetaEvaluator & evaluator,
                              const Point & subset,
                              const Scalar tolerance)
{
  const UnsignedInteger nt = theta.getSize();
  Point vals(nt);
  Point ders(nt);
  Point step(nt);
  for (UnsignedInteger iter = 0; iter < 10; ++iter)
  {
    if (!evaluator.evaluate(theta, vals, ders)) return false;
    for (UnsignedInteger i = 0; i < nt; ++i)
      step[i] = vals[i] / ders[i];
    for (UnsignedInteger i = 0; i < nt; ++i)
      theta[i] += step[i];
    if (MaxAbsStep(step, subset, subset.getSize()) <= tolerance) break;
  }
  return true;
}

// Interior block over the full rule (boundary entries overwritten later)
static Bool RefineInteriorBlock(const UnsignedInteger n,
                                const Scalar alpha,
                                const Scalar beta,
                                Point & x,
                                Point & w)
{
  const Scalar rho = 2.0 * n + alpha + beta + 1.0;
  Point tt(n);
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    const Scalar K = (2.0 * (n - k) + alpha - 0.5) * M_PI / rho;
    tt[k] = K + ((0.25 - alpha * alpha) / std::tan(0.5 * K) - (0.25 - beta * beta) * std::tan(0.5 * K)) / (rho * rho);
  }
  Point t1;
  for (UnsignedInteger k = 0; k < n; ++k)
    if (tt[k] <= M_PI / 2.0) t1.add(tt[k]);
  // The last JacobiBoundaryNodes entries are refined by the Bessel boundary
  // block and overwritten later, so they are excluded from the interior
  // Newton refinement. Guard small blocks where
  // t1.getSize() - JacobiBoundaryNodes would underflow (UnsignedInteger).
  Point idx1;
  if (t1.getSize() > JacobiBoundaryNodes)
  {
    UnsignedInteger pos = t1.getSize();
    for (UnsignedInteger k = 0; k < t1.getSize(); ++k)
      if (t1[k] < t1[t1.getSize() - JacobiBoundaryNodes])
      {
        pos = k;
        break;
      }
    for (UnsignedInteger k = 0; k <= pos && k < t1.getSize(); ++k)
      idx1.add(k);
  }
  else
  {
    for (UnsignedInteger k = 0; k < t1.getSize(); ++k)
      idx1.add(k);
  }
  const JacobiInteriorEvaluator evaluator1(n, alpha, beta);
  if (!NewtonRefineTheta(t1, evaluator1, idx1, std::sqrt(SpecFunc::ScalarEpsilon) / 100.0)) return false;
  Point vals1(t1.getSize());
  Point ders1(t1.getSize());
  if (!evaluator1.evaluate(t1, vals1, ders1)) return false;
  for (UnsignedInteger k = 0; k < t1.getSize(); ++k)
    t1[k] += vals1[k] / ders1[k];
  evaluator1.evaluate(t1, vals1, ders1);
  Point x1(t1.getSize());
  Point w1(t1.getSize());
  for (UnsignedInteger k = 0; k < t1.getSize(); ++k)
  {
    x1[k] = std::cos(t1[k]);
    w1[k] = 1.0 / (ders1[k] * ders1[k]);
  }
  // Second half, mirrored with swapped parameters. Same guard as above:
  // the first JacobiBoundaryNodes entries belong to the boundary block.
  const UnsignedInteger n2 = n - t1.getSize();
  Point t2(n2);
  for (UnsignedInteger k = 0; k < n2; ++k)
    t2[k] = M_PI - tt[k];
  Point idx2;
  if (n2 > JacobiBoundaryNodes)
  {
    UnsignedInteger pos = 0;
    for (UnsignedInteger k = 0; k < n2; ++k)
      if (t2[k] > t2[JacobiBoundaryNodes - 1])
      {
        pos = k;
        break;
      }
    for (UnsignedInteger k = pos; k < n2; ++k)
      idx2.add(k);
  }
  else
  {
    // Small block: refine everything, boundary overwrite still applies
    for (UnsignedInteger k = 0; k < n2; ++k)
      idx2.add(k);
  }
  const JacobiInteriorEvaluator evaluator2(n, beta, alpha);
  if (!NewtonRefineTheta(t2, evaluator2, idx2, std::sqrt(SpecFunc::ScalarEpsilon) / 100.0)) return false;
  Point vals2(n2);
  Point ders2(n2);
  if (!evaluator2.evaluate(t2, vals2, ders2)) return false;
  for (UnsignedInteger k = 0; k < n2; ++k)
    t2[k] += vals2[k] / ders2[k];
  evaluator2.evaluate(t2, vals2, ders2);
  x = Point(n);
  w = Point(n);
  for (UnsignedInteger k = 0; k < n2; ++k)
  {
    x[k] = -std::cos(t2[k]);
    w[k] = 1.0 / (ders2[k] * ders2[k]);
  }
  for (UnsignedInteger k = 0; k < t1.getSize(); ++k)
  {
    x[n2 + k] = x1[k];
    w[n2 + k] = w1[k];
  }
  return true;
}

// Right boundary block, ascending nodes near +1
static Bool RefineBoundaryBlock(const UnsignedInteger n,
                                const Scalar alpha,
                                const Scalar beta,
                                const UnsignedInteger npts,
                                Point & x,
                                Point & w)
{
  Point jk(BesselZeros(alpha, std::min(npts, static_cast<UnsignedInteger>(30))));
  if (npts > 30)
  {
    const Scalar mu = 4.0 * alpha * alpha;
    Point tail(npts - jk.getSize());
    for (UnsignedInteger k = jk.getSize() + 1; k <= npts; ++k)
    {
      const Scalar a8 = 8.0 * (k + 0.5 * alpha - 0.25) * M_PI;
      tail[k - jk.getSize() - 1] = 0.125 * a8 - (mu - 1.0) / a8 - 4.0 * (mu - 1.0) * (7.0 * mu - 31.0) / 3.0 / std::pow(a8, 3) - 32.0 * (mu - 1.0) * (83.0 * mu * mu - 982.0 * mu + 3779.0) / 15.0 / std::pow(a8, 5);
    }
    Point full(npts);
    for (UnsignedInteger k = 0; k < jk.getSize(); ++k)
      full[k] = jk[k];
    for (UnsignedInteger k = 0; k < tail.getSize(); ++k)
      full[jk.getSize() + k] = tail[k];
    jk = full;
  }
  const Scalar rho = n + 0.5 * (alpha + beta + 1.0);
  Point t(npts);
  Scalar thetaMax = 0.0;
  for (UnsignedInteger k = 0; k < npts; ++k)
  {
    const Scalar phik = jk[k] / rho;
    t[k] = phik + ((alpha * alpha - 0.25) * (1.0 - phik / std::tan(phik)) / (2.0 * phik) - 0.25 * (alpha * alpha - beta * beta) * std::tan(0.5 * phik)) / (rho * rho);
    thetaMax = std::max(thetaMax, t[k]);
  }
  const JacobiBoundaryTables tables(BuildBoundaryTables(alpha, beta, thetaMax, n));
  const JacobiBoundaryEvaluator evaluator(n, alpha, beta, tables, false);
  Point all;
  for (UnsignedInteger k = 0; k < npts; ++k)
    all.add(k);
  if (!NewtonRefineTheta(t, evaluator, all, std::sqrt(SpecFunc::ScalarEpsilon) / 200.0)) return false;
  const JacobiBoundaryEvaluator finalEvaluator(n, alpha, beta, tables, true);
  Point vals(npts);
  Point ders(npts);
  if (!finalEvaluator.evaluate(t, vals, ders)) return false;
  for (UnsignedInteger k = 0; k < npts; ++k)
    t[k] += vals[k] / ders[k];
  if (!finalEvaluator.evaluate(t, vals, ders)) return false;
  x = Point(npts);
  w = Point(npts);
  for (UnsignedInteger k = 0; k < npts; ++k)
  {
    x[npts - 1 - k] = std::cos(t[k]);
    w[npts - 1 - k] = 1.0 / (ders[k] * ders[k]);
  }
  return true;
}

// Full asymptotic rule: interior everywhere, Bessel boundary overwrite
static Bool ComputeNodesAndWeightsAsymptotic(const UnsignedInteger n,
                                             const Scalar alpha,
                                             const Scalar beta,
                                             Scalar * nodes,
                                             Scalar * weights)
{
  Point x;
  Point w;
  // Asymptotic expansions are only accurate for large n; small rules fall
  // back to the polished eigensolver via the caller
  if (n <= 20) return false;
  if (!RefineInteriorBlock(n, alpha, beta, x, w)) return false;
  Point xb;
  Point wb;
  if (!RefineBoundaryBlock(n, alpha, beta, JacobiBoundaryNodes, xb, wb)) return false;
  for (UnsignedInteger k = 0; k < JacobiBoundaryNodes; ++k)
  {
    x[n - JacobiBoundaryNodes + k] = xb[k];
    w[n - JacobiBoundaryNodes + k] = wb[k];
  }
  Point xb2;
  Point wb2;
  if (alpha != beta)
  {
    if (!RefineBoundaryBlock(n, beta, alpha, JacobiBoundaryNodes, xb2, wb2)) return false;
  }
  else
  {
    xb2 = xb;
    wb2 = wb;
  }
  for (UnsignedInteger k = 0; k < JacobiBoundaryNodes; ++k)
  {
    x[k] = -xb2[JacobiBoundaryNodes - 1 - k];
    w[k] = wb2[JacobiBoundaryNodes - 1 - k];
  }
  Scalar total = 0.0;
  for (UnsignedInteger k = 0; k < n; ++k)
    total += w[k];
  if (!(total > 0.0)) return false;
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    nodes[k] = x[k];
    weights[k] = w[k] / total;
  }
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    if (!std::isfinite(nodes[k])) return false;
    if (!std::isfinite(weights[k])) return false;
    if (!(weights[k] >= 0.0)) return false;
    if ((k > 0) && !(nodes[k] > nodes[k - 1])) return false;
  }
  return true;
}

namespace FastJacobi
{
  void ComputeNodesAndWeights(const UnsignedInteger n,
                              const Scalar alpha,
                              const Scalar beta,
                              Scalar * nodes,
                              Scalar * weights)
  {
    if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (alpha <= -1.0) throw InvalidArgumentException(HERE) << "Error: alpha must be > -1";
    if (beta <= -1.0) throw InvalidArgumentException(HERE) << "Error: beta must be > -1";
    if (n == 1)
    {
      nodes[0] = (beta - alpha) / (alpha + beta + 2.0);
      weights[0] = 1.0;
      return;
    }

    // Symmetric recurrence coefficients for orthonormal Jacobi polynomials
    const Scalar ab = alpha + beta;
    Point gamma(n);
    Point b(n, 0.0);
    gamma[0] = (beta - alpha) / (ab + 2.0);
    for (UnsignedInteger j = 1; j < n; ++j)
    {
      const Scalar s = static_cast<Scalar>(j);
      const Scalar t = 2.0 * s + ab;
      gamma[j] = (beta * beta - alpha * alpha) / (t * (t + 2.0));
      // Canceled form for j == 1: s + ab == t - 1, so the general formula
      // suffers a 0/0 cancellation when ab == -1 and loses accuracy when
      // ab is close to -1; the canceled form is exact for all ab
      if (j == 1)
        b[j] = 2.0 / t * std::sqrt((s + alpha) * (s + beta) / (t + 1.0));
      else
        b[j] = 2.0 / t * std::sqrt(s * (s + alpha) * (s + beta) * (s + ab) / ((t + 1.0) * (t - 1.0)));
    }

    // Large rules use the Hale-Townsend asymptotic path, small ones the
    // polished eigensolver; threshold from the ResourceMap.
    // Above the threshold the relative accuracy is better than 5e-13 and
    // the asymptotic path is faster.
    // Validity: the Hale-Townsend expansions assume moderate exponents.
    // Chebfun jacpts warns for MAX(ALPHA, BETA) > 5 when asymmetric, and the
    // mpmath oracle shows symmetric cases need the same guard ((6, 6) fails
    // at n=100 with 1.5e-12, (30, 30) fails even at n=1000 with 1e-05).
    // Gate validated to 5e-13: max <= 5 for n < 200, max <= 10 for n >= 200;
    // larger exponents fall back to the polished solver.
    const UnsignedInteger asymptoticThreshold = ResourceMap::GetAsUnsignedInteger("FastJacobi-AsymptoticThreshold");
    const Scalar maxAB = std::max(alpha, beta);
    const Scalar allowedMax = (n >= 200 ? 10.0 : 5.0);
    if ((n >= asymptoticThreshold) && (maxAB <= allowedMax))
    {
      if (ComputeNodesAndWeightsAsymptotic(n, alpha, beta, nodes, weights)) return;
      // fall through to the polished eigensolver on asymptotic failure
    }
    FastGaussQuadrature::PolishedSolve(gamma.data(), b.data(), n, nodes, weights);
  }
} // namespace FastJacobi

END_NAMESPACE_OPENTURNS
