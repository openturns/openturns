#include "openturns/SpecFunc.hxx"
#include "openturns/incgam.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace GammaFunctions
{
#define explow -300
#define exphigh 300
void incgam(const Scalar a,
            const Scalar x,
            Scalar & p,
            Scalar & q,
            SignedInteger & ierr)
{
  // -------------------------------------------------------------
  // Calculation of the incomplete gamma functions ratios P(a,x)
  // and Q(a,x).
  // -------------------------------------------------------------
  // Inputs:
  //   a ,    argument of the functions
  //   x ,    argument of the functions
  // Outputs:
  //   p,     function P(a,x)
  //   q,     function Q(a,x)
  //   ierr , error flag
  //          ierr = 0, computation successful
  //          ierr = 1, overflow/underflow problems. The function values
  //          (P(a,x) and Q(a,x)) are set to zero.
  // ----------------------------------------------------------------------
  // Authors:
  //  Amparo Gil    (U. Cantabria, Santander, Spain)
  //                 e-mail: amparo.gil@unican.es
  //  Javier Segura (U. Cantabria, Santander, Spain)
  //                 e-mail: javier.segura@unican.es
  //  Nico M. Temme (CWI, Amsterdam, The Netherlands)
  //                 e-mail: nico.temme@cwi.nl
  // -------------------------------------------------------------
  //  References: "Efficient and accurate algorithms for
  //  the computation and inversion of the incomplete gamma function ratios",
  //  A. Gil, J. Segura and N.M. Temme, submitted to SIAM J Sci Comput
  // -------------------------------------------------------------------
  Scalar lnx = -1.0;
  ierr = 0;
  if (a > alpha(x))
  {
    const Scalar dp = dompart(a, x, false);
    if (dp < 0.0)
    {
      ierr = 1;
      p = 0.0;
      q = 0.0;
      return;
    }
    if ((x  <  0.3 * a) || (a < 12)) p = ptaylor(a, x, dp);
    else p = pqasymp(a, x, dp, true);
    q = 1.0 - p;
    return;
  } // a > alpha(x)
  if (x < SpecFunc::MinScalar) lnx = SpecFunc::LogMinScalar;
  else lnx = log(x);
  if (a < -SpecFunc::MinScalar / lnx)
  {
    q = 0.0;
    p = 1.0;
    return;
  }
  if (x < 1.0)
  {
    const Scalar dp = dompart(a, x, true);
    if (dp < 0.0)
    {
      ierr = 1;
      q = 0.0;
      p = 0.0;
      return;
    }
    q = qtaylor(a, x, dp);
    p = 1.0 - q;
    return;
  } // x < 1.0
  const Scalar dp = dompart(a, x, false);
  if (dp < 0.0)
  {
    ierr = 1;
    p = 0.0;
    q = 0.0;
    return;
  }
  if ((x > 2.35 * a) || (a < 12)) q = qfraction(a, x, dp);
  else q = pqasymp(a, x, dp, false);
  p = 1.0 - q;
} // incgam

void invincgam(const Scalar a,
               const Scalar p,
               const Scalar q,
               Scalar & xr,
               SignedInteger & ierr)
{
  // -------------------------------------------------------------
  // invincgam computes xr in the equations P(a, xr) = p and Q(a, xr) = q
  // with a as a given positive parameter.
  // In most cases,  we invert the equation with min(p, q)
  // -------------------------------------------------------------
  // Inputs:
  //   a ,     argument of the functions
  //   p,      function P(a, x)
  //   q,      function Q(a, x)
  // Outputs:
  //   xr   ,  soluction of the equations P(a, xr) = p and Q(a, xr) = q
  //          with a as a given positive parameter.
  //   ierr ,  error flag
  //          ierr = 0,   computation successful
  //          ierr = -1,  overflow problem in the computation of one of the
  //                   gamma factors before starting the Newton iteration.
  //                   The initial approximation to the root is given
  //                   as output.
  //          ierr = -2,  the number of iterations in the Newton method
  //                   reached the upper limit N = 15. The last value
  //                   obtained for the root is given as output.
  // ------------------------------------------------------------------
  ierr = 0;
  Bool pcase = true;
  Scalar porq = p;
  Scalar s = -1.0;
  if (p > 0.5)
  {
    pcase = false;
    porq = q;
    s = 1.0;
  } // p > 0.5
  const Scalar logr = (1.0 / a) * (log(p) + SpecFunc::LogGamma(a + 1.0));
  Scalar x = 0.0;
  Bool m0 = true;
  Scalar eta = 0.0;
  if (logr < log(0.2 * (1.0 + a)))
  {
    const Scalar r = exp(logr);
    const Scalar ap1 = a + 1.0;
    const Scalar ap12 = ap1 * ap1;
    const Scalar ap13 = ap1 * ap12;
    const Scalar ap14 = ap12 * ap12;
    const Scalar ap2 = a + 2.0;
    const Scalar ap22 = ap2 * ap2;
    const Scalar ck1 = 1.0 / ap1;
    const Scalar ck2 = 0.5 * (3.0 * a + 5.0) / (ap12 * ap2);
    const Scalar ck3 = (31.0 + a * (33.0 + 8.0 * a)) / (3.0 * (ap13 * ap2 * (a + 3.0)));
    const Scalar ck4 = (2888.0 + a * (5661.0 + a * (3971.0 + a * (1179.0 + 125.0 * a)))) / (24.0 * (ap14 * ap22 * (a + 3.0) * (a + 4.0)));
    x = r * (1.0 + r * (ck1 + r * (ck2 + r * (ck3 + r * ck4))));
  } // logr < log(0.2 * (1 + a))
  else if ((q < std::min(0.02, exp(-1.5 * a) / SpecFunc::Gamma(a))) && (a < 10.0))
  {
    const Scalar b = 1.0 - a;
    eta = sqrt(-2.0 / a * log(q * gamstar(a) * SpecFunc::SQRT2PI / sqrt(a)));
    x = a * lambdaeta(eta);
    const Scalar L = log(x);
    const Scalar r = 1.0 / x;
    if ((a > 0.12) || (x > 5.0))
    {
      const Scalar ck0 = L - 1.0;
      const Scalar ck1 = 1.0 + 1.5 * b + L * (-b - 1.0 + 0.5 * L);
      const Scalar ck2 = -2.0 + (-4.0 - 11.0 / 6.0 * b) * b + (2.0 + (4.0 + b) * b + (-1.5 * b - 1.0 + L / 3.0) * L) * L;
      const Scalar ck3 = 6.0 + (13.5 + (10.0 + 25.0 / 12.0 * b) * b) * b + (-6.0 + (-14.0 + (-9.5 - b) * b) * b + (3.0 + (7.0 + 3.0 * b) * b + (-1.0 - 11.0 / 6.0 * b + 0.25 * L) * L) * L) * L;
      x += -L + b * r * (ck0 + r * (ck1 + r * (ck2 + r * ck3)));
    } // (a > 0.12) || (x > 5.0)
    else x += -L + b * r * (L - 1.0);
  }
  else if (std::abs(porq - 0.5) < 1.0e-5) x = a - 1.0 / 3.0 + (8.0 / 405.0 + 184.0 / 25515.0 / a) / a;
  else if (std::abs(a - 1.0) < 1.0e-4) x = (pcase ? -log1p(-p) : -log(q));
  else if (a < 1.0) x = (pcase ? exp((1.0 / a) * (log(porq) + SpecFunc::LogGamma(a + 1.0))) : exp((1.0 / a) * (log1p(-porq) + SpecFunc::LogGamma(a + 1.0))));
  else
  {
    // a >= 1.0
    m0 = false;
    const Scalar r = inverfc(2.0 * porq);
    eta = s * r / sqrt(a * 0.5);
    eta += (eps1(eta) + (eps2(eta) + eps3(eta) / a) / a) / a;
    x = a * lambdaeta(eta);
  } // a >= 1.0
  Scalar t = 1.0;
  UnsignedInteger n = 1;
  // Implementation of the high order Newton-like method;
  while ((t > SpecFunc::ScalarEpsilon) && (n < 15))
  {
    Scalar dx = 0.0;
    Scalar r = 0.0;
    if (m0)
    {
      const Scalar dlnr = (1.0 - a) * log(x) + x + SpecFunc::LogGamma(a);
      if (dlnr > SpecFunc::LogMaxScalar)
      {
        n = 20;
        ierr = -1;
        continue;
      } // dlnr > log(giant)
      r = exp(dlnr);
    }
    else r = x * gamstar(a) / (sqrt(a) * SpecFunc::ISQRT2PI * exp(-0.5 * a * eta * eta));
    SignedInteger ierrf;
    Scalar px = -1.0;
    Scalar qx = -1.0;
    incgam(a, x, px, qx, ierrf);
    r = (pcase ? -r * (px - p) : r * (qx - q));
    if (a <= 0.05) dx = r;
    else
    {
      // a > 0.05
      const Scalar ck1 = (x - a + 1.0) / (2.0 * x);
      if (a <= 0.1) dx = r * (1.0 + r * ck1);
      else
      {
        // a > 0.1
        const Scalar ck2 = (1.0 + (-3.0 + 2.0 * a) * a + (4.0 - 4.0 * a + 2.0 * x) * x) / (6.0 * x * x);
        dx = r * (1.0 + r * (ck1 + r * ck2));
      } // a > 0.1
    } // a > 0.05
    x += dx;
    t = std::abs(dx / x);
    ++n;
  } // (t > 1.0e-15) && (n <  15)
  if (n == 15) ierr = -2;
  xr = x;
} // invincgam

Scalar exmin1(const Scalar x)
{
  // computes (exp(x)-1)/x;
  if (std::abs(x) < 3.65e-8) return 1.0 + 0.5 * x;
  return expm1(x) / x;
} // exmin1

Scalar lnec(const Scalar x)
{
  // x > -1; lnec: = ln1: = ln(1+x)-x
  if (std::abs(x) < 1.3e-5) return x * x * (-0.5 + x * (1.0 / 3.0 - 0.25 * x));
  return log1p(x) - x;
} // lnec

Scalar alpha(const Scalar x)
{
  if (x > 0.25) return x + 0.25;
  if (x >= SpecFunc::MinScalar) return -0.6931 / log(x);
  return -0.6931 / SpecFunc::LogMinScalar;
} // alpha

Scalar dompart(const Scalar a,
               const Scalar x,
               const Bool qt)
{
  // dompart is approx. of  x^a * exp(-x) / gamma(a+1) ;
  Scalar r = -1.0;
  const Scalar lnx = log(x);
  if (a <= 1.0) r = -x + a * lnx;
  else
  {
    if (x == a) r = 0.0;
    else
    {
      const Scalar la = x / a;
      r = a * (1.0 - la + log(la));
    }
    r += -0.5 * log(6.2832 * a);
  }
  const Scalar dp = r < explow ? 0.0 : exp(r);
  if (qt) return dp;
  if ((a < 3.0) || (x < 0.2)) return exp(a * lnx - x) / SpecFunc::Gamma(a + 1.0);
  const Scalar mu = (x - a) / a;
  const Scalar c = lnec(mu);
  if ((a * c) > SpecFunc::LogMaxScalar) return -100.0;
  return exp(a * c) / (sqrt(a) * SpecFunc::SQRT2PI * gamstar(a));
} // dompart

Scalar chepolsum(const Scalar x,
                 const Point & a)
{
  //{a[0]/2+a[1]T1(x)+...a[n]Tn(x); series of Chebychev polynomials}
  const UnsignedInteger n = a.getDimension() - 1;
  if (n == 0) return 0.5 * a[0];
  if (n == 1) return 0.5 * a[0] + x * a[1];
  const Scalar tx = x + x;
  Scalar r = a[n];
  Scalar h = a[n - 1] + r * tx;
  for (UnsignedInteger k = n - 2; k >= 1; --k)
  {
    const Scalar s = r;
    r = h;
    h = a[k] + r * tx - s;
  }
  return 0.5 * a[0] - r + h * x;
} // chepolsum

Scalar auxgam(const Scalar x)
{
  // function g in 1/gamma(x+1) = 1+x*(x-1)*g(x), -1 <= x <= 1
  if (x < 0.0) return -(1.0 + (1.0 + x) * (1.0 + x) * auxgam(1.0 + x)) / (1.0 - x);
  Point dr(18);
  dr[0] =  -1.013609258009865776949;
  dr[1] =  0.784903531024782283535e-1;
  dr[2] =  0.67588668743258315530e-2;
  dr[3] =  -0.12790434869623468120e-2;
  dr[4] =  0.462939838642739585e-4;
  dr[5] =  0.43381681744740352e-5;
  dr[6] =  -0.5326872422618006e-6;
  dr[7] =  0.172233457410539e-7;
  dr[8] =  0.8300542107118e-9;
  dr[9] =  -0.10553994239968e-9;
  dr[10] =  0.39415842851e-11;
  dr[11] =  0.362068537e-13;
  dr[12] =  -0.107440229e-13;
  dr[13] =  0.5000413e-15;
  dr[14] =  -0.62452e-17;
  dr[15] =  -0.5185e-18;
  dr[16] =  0.347e-19;
  dr[17] =  -0.9e-21;
  return chepolsum(2.0 * x - 1.0, dr);
} // auxgam

Scalar gamstar(const Scalar x)
{
  // gamstar(x) = exp(SpecFunc::GammaCorrection(x)), x > 0; or
  // gamma(x)/(exp(-x+(x-0.5)*ln(x))/sqrt(2pi)
  if (x >= 3.0) return exp(SpecFunc::GammaCorrection(x));
  if (x > 0.0) return SpecFunc::Gamma(x) / (exp(-x + (x - 0.5) * log(x)) * SpecFunc::SQRT2PI);
  return SpecFunc::MaxScalar;
} // gamstar

Scalar fractio(const Scalar x,
               const Point & r,
               const Point & s)
{
  const UnsignedInteger n = r.getSize() - 1;
  Scalar a = r[n];
  Scalar b = 1.0;
  for (SignedInteger k = n - 1; k >= 0; --k)
  {
    a = a * x + r[k];
    b = b * x + s[k];
  }
  return a / b;
} // fractio

Scalar pqasymp(const Scalar a,
               const Scalar x,
               const Scalar dp,
               const Bool p)
{
  if (dp == 0.0) return (p ? 0.0 : 1.0);
  const SignedInteger s = p ? -1 : 1;
  const Scalar mu = x / a - 1.0;
  Scalar y = -lnec(mu);
  Scalar eta = y < 0.0 ? 0.0 : sqrt(2.0 * y);
  y *= a;
  Scalar v = sqrt(std::abs(y));
  if (mu < 0.0)
  {
    eta = -eta;
    v = -v;
  }
  const Scalar u = 0.5 * SpecFunc::ErfC(s * v);
  v = s * exp(-y) * saeta(a, eta) / (SpecFunc::SQRT2PI * sqrt(a));
  return u + v;
} // FUNCTION pqasymp;

Scalar saeta(const Scalar a,
             const Scalar eta)
{
  Point fm(27);
  fm[0] = 1.0;
  fm[1] = -1.0 / 3.0;
  fm[2] = 1.0 / 12.0;
  fm[3] = -2.0 / 135.0;
  fm[4] = 1.0 / 864.0;
  fm[5] = 1.0 / 2835.0;
  fm[6] = -139.0 / 777600.0;
  fm[7] = 1.0 / 25515.0;
  fm[8] = -571.0 / 261273600.0;
  fm[9] = -281.0 / 151559100.0;
  fm[10] = 8.29671134095308601e-7;
  fm[11] = -1.76659527368260793e-7;
  fm[12] = 6.70785354340149857e-9;
  fm[13] = 1.02618097842403080e-8;
  fm[14] = -4.38203601845335319e-9;
  fm[15] = 9.14769958223679023e-10;
  fm[16] = -2.55141939949462497e-11;
  fm[17] = -5.83077213255042507e-11;
  fm[18] = 2.43619480206674162e-11;
  fm[19] = -5.02766928011417559e-12;
  fm[20] = 1.10043920319561347e-13;
  fm[21] = 3.37176326240098538e-13;
  fm[22] = -1.39238872241816207e-13;
  fm[23] = 2.85348938070474432e-14;
  fm[24] = -5.13911183424257258e-16;
  fm[25] = -1.97522882943494428e-15;
  fm[26] =  8.09952115670456133e-16;
  Point bm(27);
  bm[25] = fm[26];
  bm[24] = fm[25];
  for (UnsignedInteger m = 24; m > 0; --m) bm[m - 1] = fm[m] + (m + 1) * bm[m + 1] / a;
  Scalar s = bm[0];
  Scalar t = s;
  Scalar y = eta;
  for (UnsignedInteger m = 1; m < 25; ++m)
  {
    if (std::abs(t / s) <= SpecFunc::ScalarEpsilon) break;
    t = bm[m] * y;
    s += t;
    y *= eta;
  }
  return s / (1.0 + bm[1] / a);
} // saeta

Scalar qfraction(const Scalar a,
                 const Scalar x,
                 const Scalar dp)
{
  if (dp == 0.0) return 0.0;
  Scalar p = 0.0;
  Scalar q = (x - 1.0 - a) * (x + 1.0 - a);
  Scalar r = 4.0 * (x + 1.0 - a);
  Scalar s = 1.0 - a;
  Scalar ro = 0.0;
  Scalar t = 1.0;
  Scalar g = 1.0;
  while (std::abs(t / g) >= SpecFunc::ScalarEpsilon)
  {
    p += s;
    q += r;
    r += 8.0;
    s += 2.0;
    const Scalar tau = p * (1.0 + ro);
    ro = tau / (q - tau);
    t *= ro;
    g += t;
  }
  return (a / (x + 1.0 - a)) * g * dp;
} // qfraction

Scalar qtaylor(const Scalar a,
               const Scalar x,
               const Scalar dp)
{
  if (dp == 0.0) return 0.0;
  const Scalar lnx = log(x);
  Scalar r = a * lnx;
  Scalar q(r * exmin1(r));   // {q = x^a - 1}
  Scalar s(a * (1.0 - a) * auxgam(a)); // {s = 1 - 1 / Gamma(1 + a)}
  q *= 1.0 - s;
  Scalar u(s - q);               // {u = 1 - x^a/Gamma(1+a)}
  Scalar p = a * x;
  Scalar t = 1.0;
  Scalar v = 1.0;
  q = a + 1.0;
  r = a + 3.0;
  while (std::abs(t / v) > SpecFunc::ScalarEpsilon)
  {
    p += x;
    q += r;
    r += 2.0;
    t *= -p / q;
    v += t;
  }
  v *= a * (1.0 - s) * exp((a + 1.0) * lnx) / (a + 1.0);
  return u + v;
} // qtaylor

Scalar ptaylor(const Scalar a,
               const Scalar x,
               const Scalar dp)
{
  if (dp == 0.0) return 0.0;
  Scalar p = 1.0;
  Scalar c = 1.0;
  Scalar r = a;
  while (c > p * SpecFunc::ScalarEpsilon)
  {
    r += 1.0;
    c *= x / r;
    p += c;
  }
  return p * dp;
} // ptaylor

Scalar eps1(const Scalar eta)
{
  if (std::abs(eta) < 1.0)
  {
    Point ak(5);
    ak[0] = -3.333333333438e-1;
    ak[1] = -2.070740359969e-1;
    ak[2] = -5.041806657154e-2;
    ak[3] = -4.923635739372e-3;
    ak[4] = -4.293658292782e-5;
    Point bk(5);
    bk[0] =  1.000000000000e+0;
    bk[1] =  7.045554412463e-1;
    bk[2] =  2.118190062224e-1;
    bk[3] =  3.048648397436e-2;
    bk[4] =  1.605037988091e-3;
    return ratfun(eta, ak, bk);
  }
  return log(eta / (lambdaeta(eta) - 1.0)) / eta;
} // eps1

Scalar eps2(const Scalar eta)
{
  if (eta < -5.0)
  {
    const Scalar x = eta * eta;
    const Scalar lnmeta = log(-eta);
    return (12.0 - x - 6.0 * (lnmeta * lnmeta)) / (12.0 * x * eta);
  }
  if (eta < -2.0)
  {
    Point ak(5);
    ak[0] = -1.72847633523e-2;
    ak[1] = -1.59372646475e-2;
    ak[2] = -4.64910887221e-3;
    ak[3] = -6.06834887760e-4;
    ak[4] = -6.14830384279e-6;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 7.64050615669e-1;
    bk[2] = 2.97143406325e-1;
    bk[3] = 5.79490176079e-2;
    bk[4] = 5.74558524851e-3;
    return ratfun(eta, ak, bk);
  }
  if (eta  <  2.0)
  {
    Point ak(5);
    ak[0] = -1.72839517431e-2;
    ak[1] = -1.46362417966e-2;
    ak[2] = -3.57406772616e-3;
    ak[3] = -3.91032032692e-4;
    ak[4] = 2.49634036069e-6;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 6.90560400696e-1;
    bk[2] = 2.49962384741e-1;
    bk[3] = 4.43843438769e-2;
    bk[4] = 4.24073217211e-3;
    return ratfun(eta, ak, bk);
  }
  if (eta  <  1000.0)
  {
    Point ak(5);
    ak[0] = 9.99944669480e-1;
    ak[1] = 1.04649839762e+2;
    ak[2] = 8.57204033806e+2;
    ak[3] = 7.31901559577e+2;
    ak[4] = 4.55174411671e+1;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 1.04526456943e+2;
    bk[2] = 8.23313447808e+2;
    bk[3] = 3.11993802124e+3;
    bk[4] = 3.97003311219e+3;
    return ratfun(1.0 / eta, ak, bk) / (-12.0 * eta);
  }
  return -1.0 / (12.0 * eta);
} // eps2

Scalar eps3(const Scalar eta)
{
  if (eta < -8.0)
  {
    const Scalar x = eta * eta;
    const Scalar y = log(-eta) / eta;
    return (-30.0 + eta * y * (6.0 * x * y * y - 12.0 + x)) / (12.0 * eta * x * x);
  }
  if (eta < -4.0)
  {
    Point ak(5);
    ak[0] = 4.95346498136e-2;
    ak[1] = 2.99521337141e-2;
    ak[2] = 6.88296911516e-3;
    ak[3] = 5.12634846317e-4;
    ak[4] = -2.01411722031e-5;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 7.59803615283e-1;
    bk[2] = 2.61547111595e-1;
    bk[3] = 4.64854522477e-2;
    bk[4] = 4.03751193496e-3;
    return ratfun(eta, ak, bk) / (eta * eta);
  }
  if (eta < -2.0)
  {
    Point ak(5);
    ak[0] = 4.52313583942e-3;
    ak[1] = 1.20744920113e-3;
    ak[2] = -7.89724156582e-5;
    ak[3] = -5.04476066942e-5;
    ak[4] = -5.35770949796e-6;
    Point bk(5);
    bk[0] =  1.00000000000e+0;
    bk[1] =  9.12203410349e-1;
    bk[2] =  4.05368773071e-1;
    bk[3] =  9.01638932349e-2;
    bk[4] =  9.48935714996e-3;
    return ratfun(eta, ak, bk);
  }
  if (eta < 2.0)
  {
    Point ak(5);
    ak[0] = 4.39937562904e-3;
    ak[1] = 4.87225670639e-4;
    ak[2] = -1.28470657374e-4;
    ak[3] = 5.29110969589e-6;
    ak[4] = 1.57166771750e-7;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 7.94435257415e-1;
    bk[2] = 3.33094721709e-1;
    bk[3] = 7.03527806143e-2;
    bk[4] = 8.06110846078e-3;
    return ratfun(eta, ak, bk);
  }
  if (eta < 10.0)
  {
    Point ak(5);
    ak[0] = -1.14811912320e-3;
    ak[1] = -1.12850923276e-1;
    ak[2] = 1.51623048511e+0;
    ak[3] = -2.18472031183e-1;
    ak[4] = 7.30002451555e-2;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 1.42482206905e+1;
    bk[2] = 6.97360396285e+1;
    bk[3] = 2.18938950816e+2;
    bk[4] = 2.77067027185e+2;
    return ratfun(1.0 / eta, ak, bk) / (eta * eta);
  }
  if (eta < 100.0)
  {
    Point ak(5);
    ak[0] = -1.45727889667e-4;
    ak[1] = -2.90806748131e-1;
    ak[2] = -1.33085045450e+1;
    ak[3] = 1.99722374056e+2;
    ak[4] = -1.14311378756e+1;
    Point bk(5);
    bk[0] = 1.00000000000e+0;
    bk[1] = 1.39612587808e+2;
    bk[2] = 2.18901116348e+3;
    bk[3] = 7.11524019009e+3;
    bk[4] = 4.55746081453e+4;
    return ratfun(1.0 / eta, ak, bk) / (eta * eta);
  }
  return -log(eta) / (12.0 * eta * eta * eta);
} // eps3

Scalar lambdaeta(const Scalar eta)
{
  // lambdaeta is the positive number satisfying;
  // eta^2/2 = lambda-1-ln(lambda);
  // with sign(lambda-1) = sign(eta);
  if (eta == 0.0) return 1.0;
  const Scalar z = 1.0 + 0.5 * eta * eta;
  return exp(-z - SpecFunc::LambertW(-exp(-z), eta < 0.0));
} // lambdaeta

Scalar invq(const Scalar x)
{
  //  Abramowitx & Stegun 26.2.23;
  Scalar t = sqrt(-2 * log(x));
  return t - (2.515517 + t * (0.802853 + t * 0.010328)) / (1.0 + t * (1.432788 + t * (0.189269 + t * 0.001308)));
} // invq

Scalar inverfc(const Scalar x)
{
  if (x > 1.0) return -inverfc(2.0 - x);
  const Scalar y0 = 0.70710678 * invq(0.5 * x);
  const Scalar f = SpecFunc::ErfC(y0) - x;
  const Scalar y02 = y0 * y0;
  const Scalar fp = -M_2_SQRTPI * exp(-y02);
  const Scalar c1 = -1.0 / fp;
  const Scalar c2 = y0;
  const Scalar c3 = (4.0 * y02 + 1.0) / 3.0;
  const Scalar c4 = y0 * (12.0 * y02 + 7.0) / 6.0;
  const Scalar c5 = (8.0 * y02 + 7.0) * (12.0 * y02 + 1.0) / 30.0;
  const Scalar r = f * c1;
  const Scalar h = r * (1.0 + r * (c2 + r * (c3 + r * (c4 + r * c5))));
  return y0 + h;
} // inverfc

Scalar ratfun(const Scalar x,
              const Point & ak,
              const Point & bk)
{
  return (ak[0] + x * (ak[1] + x * (ak[2] + x * (ak[3] + x * ak[4])))) / (bk[0] + x * (bk[1] + x * (bk[2] + x * (bk[3] + x * bk[4]))));
} // ratfun

} // GammaFunctions

END_NAMESPACE_OPENTURNS
