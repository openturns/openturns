//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the Student T
 *         CDF and quantile
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>

#include "openturns/StudentFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/GaussKronrodRule.hxx"
#include "openturns/Log.hxx"
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_BOOST
#define BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/non_central_t.hpp>
#endif

BEGIN_NAMESPACE_OPENTURNS

namespace StudentFunctions
{

/********************************************************************************************************************************/
/* Normalized Student distribution, i.e. with a PDF equals to (1 + x^2 / nu)^(-(1 + nu) / 2) / (sqrt(nu) . Beta(1 / 2, nu / 2)) */
/********************************************************************************************************************************/
/* CDF */
Scalar StudentCDF(const Scalar nu,
                  const Scalar x,
                  const Bool tail)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  if (x == 0.0) return 0.5;
  if (nu == 1.0) return (tail ? 0.5 - (std::atan(x) * M_1_PI) : 0.5 + (std::atan(x) * M_1_PI));
  const Scalar x2 = x * x;
  if (nu == 2.0) return (tail ? 0.5 - 0.5 * (x / std::sqrt(2.0 + x2)) : 0.5 + 0.5 * (x / std::sqrt(2.0 + x2)));
  if (nu == 3.0) return (tail ? 0.5 - (std::atan(x / std::sqrt(3.0)) * M_1_PI + x * std::sqrt(3.0) / (M_PI * (3.0 + x2))) : 0.5 + (std::atan(x / std::sqrt(3.0)) * M_1_PI + x * std::sqrt(3.0) / (M_PI * (3.0 + x2))));
  if (nu == 4.0) return (tail ? 0.5 - (0.5 * x * (x2 + 6.0) * std::pow(4.0 + x2, -1.5)) : 0.5 + (0.5 * x * (x2 + 6.0) * std::pow(4.0 + x2, -1.5)));
  if (nu == 5.0) return (tail ? 0.5 - (std::atan(x / std::sqrt(5.0)) * M_1_PI + x * std::sqrt(5.0) * (3.0 * x2 + 25.0) / (3.0 * M_PI * std::pow(5.0 + x2, 2))) : 0.5 + (std::atan(x / std::sqrt(5.0)) * M_1_PI + x * std::sqrt(5.0) * (3.0 * x2 + 25.0) / (3.0 * M_PI * std::pow(5.0 + x2, 2))));
  if (nu == 6.0) return (tail ? 0.5 - (0.25 * x * (135.0 + x2 * (30.0 + 2.0 * x2)) * std::pow(6.0 + x2, -2.5)) : 0.5 + (0.25 * x * (135.0 + x2 * (30.0 + 2.0 * x2)) * std::pow(6.0 + x2, -2.5)));
  if (nu == 7.0) return (tail ? 0.5 - (std::atan(x / std::sqrt(7.0)) * M_1_PI + x * std::sqrt(7.0) * (1617.0 + x2 * (280.0 + 15.0 * x2)) / (15.0 * M_PI * std::pow(7.0 + x2, 3))) : 0.5 + (std::atan(x / std::sqrt(7.0)) * M_1_PI + x * std::sqrt(7.0) * (1617.0 + x2 * (280.0 + 15.0 * x2)) / (15.0 * M_PI * std::pow(7.0 + x2, 3))));
#ifdef OPENTURNS_HAVE_BOOST
  return (tail ? boost::math::cdf(boost::math::complement(boost::math::students_t(nu), x)) : boost::math::cdf(boost::math::students_t(nu), x)) ;
#else
#ifdef USE_NEW_ALGO
  // First, try to use a Cornish-Fisher expansion
  if (nu > 1e3)
  {
    const Scalar inu = 1.0 / nu;
    // Compute the last corrective term first in order to check if the Cornish-Fisher approximation is good enough
    // ~ \phi(x)/x+x\phi(x)[c1/nu + ... + c4/nu^4]
    // -> \epsilon/x = x * c4 /nu^4 -> nu=(x^2*c4/\epsilon)^{1/4}
    const Scalar c4 = (21.0 / 2048.0 + (61.0 / 6144.0 + (-71.0 / 30720.0 + (-313.0 / 30720.0 + (-2141.0 / 92160.0 + (445.0 / 18432.0 + (-25.0 / 6144.0 + x2 / 6144.0) * x2) * x2) * x2) * x2) * x2) * x2) * x2;
    const Scalar lastContribution = std::abs(c4 * inu * inu * inu * inu * x);
    Scalar normalPDF = DistFunc::dNormal(x);
    Scalar normalCCDF = DistFunc::pNormal(std::abs(x), true);
    if (normalCCDF > SpecFunc::ScalarEpsilon * normalPDF * lastContribution)
    {
      const Scalar c1 = 0.25 * (1.0 + x2);
      const Scalar c2 = (-3.0 + x2 * (-5.0 + x2 * (-7.0 + 3.0 * x2))) / 96.0;
      const Scalar c3 = (-15.0 + x2 * (-3.0 + x2 * (6.0 + x2 * (14.0 + x2 * (-11.0 + x2))))) / 384.0;
      const Scalar correction = std::abs(x) * (inu * (c1 + inu * (c2 + inu * (c3 + inu * c4))));
      const Scalar value = normalCCDF + normalPDF * correction;
      return (((x >= 0.0) == tail) ? value : 0.5 + (0.5 - value));
    }
  }
#endif
  Scalar value = 0.0;
  if (2.0 * x2 > nu) value = 0.5 * SpecFunc::RegularizedIncompleteBeta(0.5, 0.5 * nu, x2 / (x2 + nu), true);
  else value = 0.5 * SpecFunc::RegularizedIncompleteBeta(0.5 * nu, 0.5, nu / (x2 + nu), false);
  return (((x < 0.0) == tail) ? 0.5 + (0.5 - value) : value);
#endif // OPENTURNS_HAVE_BOOST
}

/* The algorithm is based on the following article:
   William T. Shaw, "New methods for simulating the Student T-distribution - direct use of the inverse cumulative distribution"
   eprint http://eprints.maths.ox.ac.uk/184/1/tdist.pdf
*/
Scalar StudentQuantile(const Scalar nu,
                       const Scalar p,
                       const Bool tail)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  if (p == 0.5) return 0.0;
  const Scalar u = std::min(p, 0.5 + (0.5 - p));
  if (nu == 1.0)
  {
    Scalar value = 0.0;
    if (std::abs(u) < 0.025373628595705897178)
    {
      const Scalar u2 = u * u;
      value = (-0.31830988618379067153 + (1.0471975511965977462 + (0.68902837067332933726 + (0.64766070854027820799 + 0.63921549794217821540 * u2) * u2) * u2) * u2) / u;
    }
    else value = std::tan((u - 0.5) * M_PI);
    return (tail == (p < 0.5) ? -value : value);
  }
  if (nu == 2.0)
  {
    const Scalar alpha = 2.0 * u - 1.0;
    const Scalar value = alpha * std::sqrt(2.0 / (0.5 + (0.5 - alpha * alpha)));
    return (tail == (p < 0.5) ? -value : value);
  }
  if (nu == 4.0)
  {
    const Scalar alphaSqrt = 2.0 * std::sqrt(p * (0.5 + (0.5 - p)));
    const Scalar value = 2.0 * std::sqrt((std::cos(std::acos(alphaSqrt) / 3.0) / alphaSqrt - 0.5) - 0.5);
    // Warning! Here the test is different from the other ones
    return (tail == (p > 0.5) ? -value : value);
  }
#ifdef OPENTURNS_HAVE_BOOST
  const Scalar cprob = std::min(std::max(p, std::sqrt(SpecFunc::MinScalar)), 0.5 + (0.5 - SpecFunc::ScalarEpsilon));
  return tail ? boost::math::quantile(boost::math::complement(boost::math::students_t(nu), cprob)) : boost::math::quantile(boost::math::students_t(nu), cprob);
#else
#ifdef USE_NEW_ALGO
  // Central part
  const Scalar delta = 1.0 / nu;
  const Scalar normalizationFactor = std::sqrt(nu * M_PI) * std::exp(SpecFunc::LogGamma(0.5 * nu) - SpecFunc::LogGamma(0.5 * (nu + 1.0)));
  const Scalar v = (p - 0.5) * normalizationFactor;
  const Scalar c30 = 0.11362104808202311779e-7 + (-0.10994648871905821641e-6 + (0.52754948010031397619e-6 + (-0.16579190541298212282e-5 + (0.37966880029665235514e-5 + (-0.66596982230496113818e-5 + (0.91156850995515265275e-5 + (-0.96136067004897554437e-5 + (0.72052743951206692720e-5 + (-0.23729574256549482204e-5 + (-0.31083841717988836362e-5 + (0.72241563770271714564e-5 + (-0.88503346702785761842e-5 + (0.81494033025679967378e-5 + (-0.61299787607422707781e-5 + (0.38918328140793891542e-5 + (-0.21177061849789713922e-5 + (0.99481889094179105397e-6 + (-0.40449311472598426761e-6 + (0.14225421139331034806e-6 + (-0.43122948806183507233e-7 + (0.11198634674338365791e-7 + (-0.24684919226898448232e-8 + (0.45586959658322983050e-9 + (-0.69253251498584479106e-10 + (0.84307519236004815165e-11 + (-0.79095838183517833726e-12 + (0.53696817752523318081e-13 + (-0.23480084614199964747e-14 + (0.49659938970935851773e-16 + 0.19701319568021683118e-83 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
  if (c30 * std::pow(std::abs(v), 30) < SpecFunc::Precision)
  {
    const Scalar v2 = v * v;
    const Scalar c1 = 0.16666666666666666667e0 + 0.16666666666666666667e0 * delta;
    const Scalar c2 = 0.58333333333333333333e-1 + (0.66666666666666666667e-1 + 0.83333333333333333333e-2 * delta) * delta;
    const Scalar c3 = 0.25198412698412698413e-1 + (0.26785714285714285714e-1 + (0.17857142857142857143e-2 + 0.19841269841269841270e-3 * delta) * delta) * delta;
    const Scalar c4 = 0.12039792768959435626e-1 + (0.10559964726631393298e-1 + (-0.11078042328042328042e-2 + (0.37477954144620811287e-3 + 0.27557319223985890653e-5 * delta) * delta) * delta) * delta;
    const Scalar c5 = 0.61039211560044893378e-2 + (0.38370059724226390893e-2 + (-0.16095979637646304313e-2 + (0.59458674042007375341e-3 + (-0.62705427288760622094e-4 + 0.25052108385441718775e-7 * delta) * delta) * delta) * delta) * delta;
    const Scalar c6 = 0.32177478835464946576e-2 + (0.10898206731540064873e-2 + (-0.12579159844784844785e-2 + (0.69084207973096861986e-3 + (-0.16376804137220803887e-3 + (0.15401265401265401265e-4 + 0.16059043836821614599e-9 * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c7 = 0.17438262298340009980e-2 + (0.33530976880017885309e-4 + (-0.76245135440323932387e-3 + (0.64513046951456342991e-3 + (-0.24947258047043099953e-3 + (0.49255746366361445727e-4 + (-0.39851014346715404916e-5 + 0.76471637318198164759e-12 * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c8 = 0.96472747321388644237e-3 + (-0.31101086326318780412e-3 + (-0.36307660358786885787e-3 + (0.51406605788341121363e-3 + (-0.29133414466938067350e-3 + (0.90867107935219902229e-4 + (-0.15303004486655377567e-4 + (0.10914179173496789432e-5 + 0.28114572543455207632e-14 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c9 = 0.54229262813129686486e-3 + (-0.36942667800009661203e-3 + (-0.10230378073700412687e-3 + (0.35764655430568632777e-3 + (-0.28690924218514613987e-3 + (0.12645437628698076975e-3 + (-0.33202652391372058698e-4 + (0.48903045291975346210e-5 + (-0.31239569599829868045e-6 + 0.82206352466243297170e-17 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c10 = 0.30873303081359101129e-3 + (-0.32537004938571011330e-3 + (0.43550551405434728655e-4 + (0.21464548012307279066e-3 + (-0.24866783037387793908e-3 + (0.14689614712949377285e-3 + (-0.53558768075354021202e-4 + (0.12193465978325997301e-4 + (-0.15992939851465476095e-5 + (0.92645939464804105906e-7 + 0.19572941063391261231e-19 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c11 = 0.17759647804672470704e-3 + (-0.25535863863970254416e-3 + (0.11096883532369592643e-3 + (0.10245143385318167742e-3 + (-0.19299737813074419180e-3 + (0.14967238384542144940e-3 + (-0.71397916945779067573e-4 + (0.22340804256056967439e-4 + (-0.45025708360340229558e-5 + (0.53317862670086688269e-6 + (-0.28285516204934114990e-7 + 0.38681701706306840377e-22 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c12 = 0.10304494161207094302e-3 + (-0.18870337947507879042e-3 + (0.13111867117178691920e-3 + (0.24142361947767894831e-4 + (-0.13381350551156149560e-3 + (0.13730787745727972950e-3 + (-0.82983259907885451298e-4 + (0.33532084099933722245e-4 + (-0.92267370235976158080e-5 + (0.16716613200640343655e-5 + (-0.18065780614881715429e-6 + (0.88471846978918480156e-8 + 0.64469502843844733962e-25 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c13 = 0.60225008003409982305e-4 + (-0.13446378338878729098e-3 + (0.12598693847856027229e-3 + (-0.24590085473754607964e-4 + (-0.80299081016436116016e-4 + (0.11482249262723200489e-3 + (-0.86686905022801866130e-4 + (0.43609942443795689198e-4 + (-0.15373457330466647584e-4 + (0.37852139693751540035e-5 + (-0.62383416384412893096e-6 + (0.62064429966766230519e-7 + (-0.28243405937805525220e-8 + 0.91836898637955461484e-28 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c14 = 0.35418325565277953269e-4 + (-0.93539064666727070094e-4 + (0.10944539484512486371e-3 + (-0.50697922600088918528e-4 + (-0.37072450319611740043e-4 + (0.87596232666609754365e-4 + (-0.82848204898007245194e-4 + (0.50788700804564126157e-4 + (-0.22068398246898720913e-4 + (0.69246032720547932818e-5 + (-0.15457039388956089765e-5 + (0.23391857848945430160e-6 + (-0.21577431344124575639e-7 + (0.91752074323779275911e-9 + 0.11309962886447716932e-30 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c15 = 0.20941876127554053895e-4 + (-0.63982320984468180920e-4 + (0.89476217782852851948e-4 + (-0.61139401127641540572e-4 + (-0.53533915472133463276e-5 + (0.60092507103170186035e-4 + (-0.73116739352278641607e-4 + (0.54064352860717542135e-4 + (-0.28252385872037662336e-4 + (0.10858001403563361875e-4 + (-0.30773127066866958235e-5 + (0.62914443600476829315e-6 + (-0.88098412226584032081e-7 + (0.75797246590958718870e-8 + (-0.30260395874299918486e-9 + 0.12161250415535179496e-33 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c16 = 0.12440676964611940273e-4 + (-0.43227811212828780986e-4 + (0.70220395983921690289e-4 + (-0.61710480704301234682e-4 + (0.15725654644255476988e-4 + (0.35359168269646105688e-4 + (-0.59705715039685462092e-4 + (0.53283122436065111649e-4 + (-0.32980546454767555846e-4 + (0.15124084472974760560e-4 + (-0.52275481468058844855e-5 + (0.13532478852492074758e-5 + (-0.25548550443536494162e-6 + (0.33313281803022382511e-7 + (-0.26868931811498915393e-8 + (0.10112530549820428339e-9 + 0.11516335620771950281e-36 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c17 = 0.74211365422405435880e-5 + (-0.28933653345888529746e-4 + (0.53504983616749018458e-4 + (-0.56733447346013552297e-4 + (0.28046995125144914249e-4 + (0.15046953874432646849e-4 + (-0.44797205572871768076e-4 + (0.48974195262422243935e-4 + (-0.35626672354897940259e-4 + (0.19154758965665053933e-4 + (-0.78594245320243336015e-5 + (0.24736906403032496435e-5 + (-0.59010219823058050328e-6 + (0.10357491081664883410e-6 + (-0.12643342283016852693e-7 + (0.96013308009708694978e-9 + (-0.34189008978763638042e-10 + 0.96775929586318909921e-40 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c18 = 0.44431685013520881494e-5 + (-0.19225728937686126470e-4 + (0.39866342691238713685e-4 + (-0.49168720718451249294e-4 + (0.33775289421222619037e-4 + (-0.31087617911211617812e-6 + (-0.30179218224879386155e-4 + (0.42079421138111979198e-4 + (-0.35960405573411444377e-4 + (0.22412174312718158972e-4 + (-0.10713400205607912977e-4 + (0.39886149460845449983e-5 + (-0.11542192605244155692e-5 + (0.25556230486781413954e-6 + (-0.41938614038099184801e-7 + (0.48146397593234635691e-8 + (-0.34555062015776742157e-9 + (0.11678439676013086698e-10 + 0.72654601791530713154e-43 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c19 = 0.26689550078709988424e-5 + (-0.12701371273794086192e-4 + (0.29187128312258641044e-4 + (-0.40887625651814964910e-4 + (0.34931364376163753961e-4 + (-0.10941760706981209265e-4 + (-0.17095949246117664777e-4 + (0.33684354049709423542e-4 + (-0.34115982472168278269e-4 + (0.24496276291828666349e-4 + (-0.13463974509890800010e-4 + (0.58150160113439943742e-5 + (-0.19853509812636133521e-5 + (0.53231546936576302638e-6 + (-0.11005170539457305057e-6 + (0.16966333616170516023e-7 + (-0.18390721265526858957e-8 + (0.12516003776882922472e-9 + (-0.40260203854332223970e-11 + 0.49024697565135433977e-46 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c20 = 0.16079468293566755077e-5 + (-0.83519181950909056223e-5 + (0.21068264047704844251e-4 + (-0.32967663694395873249e-4 + (0.33192227255609240600e-4 + (-0.17505725315921772768e-4 + (-0.62507158539472975653e-5 + (0.24809896176762296083e-4 + (-0.30494472688029092428e-4 + (0.25201640916265454033e-4 + (-0.15787641075728493217e-4 + (0.77993169714226206621e-5 + (-0.30798093966460254524e-5 + (0.97237595059774941332e-6 + (-0.24310038260091801130e-6 + (0.47165155130938945170e-7 + (-0.68593186343403724861e-8 + (0.70445800732998961076e-9 + (-0.45594868193886099691e-10 + (0.13994385619395025871e-11 + 0.29893108271424045108e-49 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c21 = 0.97131634673789398878e-6 + (-0.54708225462996645305e-5 + (0.15031612868346010381e-4 + (-0.25948684686355881128e-4 + (0.29836299490113887428e-4 + (-0.20841746475330982938e-4 + (0.21041249868451872146e-5 + (0.16284262391941984951e-4 + (-0.25642616763698460772e-4 + (0.24523094611425921414e-4 + (-0.17422194113333550583e-4 + (0.97452558818057754024e-5 + (-0.43860223502643760704e-5 + (0.15981068447331646635e-5 + (-0.46976961660947401631e-6 + (0.11009357544923694910e-6 + (-0.20131669532027190606e-7 + (0.27718538806172187051e-8 + (-0.27054082137873267281e-9 + (0.16696448943448797325e-10 + (-0.49008459619564683563e-12 + 0.16552108677421951887e-52 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c22 = 0.58816866045106563105e-6 + (-0.35721527099333012576e-5 + (0.10620496275112834190e-4 + (-0.20030890513892794267e-4 + (0.25769570856134526479e-4 + (-0.21796848545887314032e-4 + (0.80432398190338473790e-5 + (0.86890833863663663181e-5 + (-0.20140605182206560763e-4 + (0.22622743926783082774e-4 + (-0.18204948546547989395e-4 + (0.11449675542311518867e-4 + (-0.58093764821307956918e-5 + (0.24063727883622600125e-5 + (-0.81496902854646487096e-6 + (0.22430060731696070044e-6 + (-0.49498828597482276521e-7 + (0.85628682699988764342e-8 + (-0.11197312381501881491e-8 + (0.10414592635951676503e-9 + (-0.61430478648550010010e-11 + (0.17279382812933012608e-12 + 0.83596508471828039833e-56 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c23 = 0.35694623785962092945e-6 + (-0.23261540519033892935e-5 + (0.74419178472078609044e-5 + (-0.15216318398635566250e-4 + (0.21588373630493424187e-4 + (-0.21126970163845042742e-4 + (0.11845455304496076792e-4 + (0.23609880626344786979e-5 + (-0.14516984266273695216e-4 + (0.19775888656621845116e-4 + (-0.18086107912040198044e-4 + (0.12736537507049307512e-4 + (-0.72266519718919009453e-5 + (0.33644119808366374648e-5 + (-0.12928827313050406050e-5 + (0.40943264368765462217e-6 + (-0.10600909150611481864e-6 + (0.22114965040891172594e-7 + (-0.36311096679932346748e-8 + (0.45222524763612305957e-9 + (-0.40179389558336936254e-10 + (0.22699473155292863497e-11 + (-0.61300713659856468819e-13 + 0.38666285139605938868e-59 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c24 = 0.21706145346345576459e-6 + (-0.15113095478470218871e-5 + (0.51776320957662205422e-5 + (-0.11403661131246235169e-4 + (0.17651639712419593863e-4 + (-0.19451656683614343464e-4 + (0.13890838328443818024e-4 + (-0.25727313272785381811e-5 + (-0.91965090082447991628e-5 + (0.16312044822824170784e-4 + (-0.17120331841076803075e-4 + (0.13482036950914841652e-4 + (-0.85057705076024007563e-5 + (0.44120031517730367803e-5 + (-0.19013595509454534312e-5 + (0.68222313948188219820e-6 + (-0.20303517093341515889e-6 + (0.49655697374466441223e-7 + (-0.98257414937286547754e-8 + (0.15356901309348980984e-8 + (-0.18261072140700804068e-9 + (0.15532593734902944723e-10 + (-0.84209778634328029841e-12 + (0.21870365650245415074e-13 + 0.16439747083165790335e-62 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c25 = 0.13224235211744936908e-6 + (-0.97997851350371097919e-6 + (0.35800640027359557107e-5 + (-0.84480240475059725402e-5 + (0.14148068359250587228e-4 + (-0.17244580119470605765e-4 + (0.14584645223195765026e-4 + (-0.61429327229030144895e-5 + (-0.44783230402391903591e-5 + (0.12562837136392852172e-4 + (-0.15443365608705532966e-4 + (0.13627399430067013885e-4 + (-0.95259580946524691274e-5 + (0.54692132382223148275e-5 + (-0.26191959740227063354e-5 + (0.10521721543189930798e-5 + (-0.35438400651509059576e-6 + (0.99538596530834004426e-7 + (-0.23075877907941536799e-7 + (0.43441556712522510653e-8 + (-0.64795864328105212055e-9 + (0.73731529915903144880e-10 + (-0.60158855648069050720e-11 + (0.31353477917777829365e-12 + (-0.78433290281573252839e-14 + 0.64469596404571726805e-66 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c26 = 0.80705592796482722655e-7 + (-0.63437141448789667973e-6 + (0.24620468370828364674e-5 + (-0.61960523928876340181e-5 + (0.11151856241067755755e-4 + (-0.14845004900719220437e-4 + (0.14307620482977119539e-4 + (-0.84853793010789473256e-5 + (-0.53744517576841498778e-6 + (0.88232406830407420128e-5 + (-0.13241570978518585723e-4 + (0.13179408729958335575e-4 + (-0.10194226086350057747e-4 + (0.64476996796559746850e-5 + (-0.34070253128267544011e-5 + (0.15180006154095517750e-5 + (-0.57168713761449777375e-6 + (0.18156038533459937192e-6 + (-0.48306691833643123879e-7 + (0.10648430194032302901e-7 + (-0.19122095699195631812e-8 + (0.27282465206711428022e-9 + (-0.29768356697676522564e-10 + (0.23340591071190063562e-11 + (-0.11712858328988494492e-12 + (0.28263287020243359521e-14 + 0.23392451525606577215e-69 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c27 = 0.49331711088852888976e-7 + (-0.41004368291703239281e-6 + (0.16850897961703399574e-5 + (-0.45047105587757996876e-5 + (0.86654922165692884461e-5 + (-0.12479329094478464909e-4 + (0.13388324258361694718e-4 + (-0.97936498514572374158e-5 + (0.25587045260909124072e-5 + (0.53280747221444300349e-5 + (-0.10721181802601462187e-4 + (0.12201152453443121452e-4 + (-0.10455670828281603489e-4 + (0.72629981428834119043e-5 + (-0.42115834110197720771e-5 + (0.20657788598905200051e-5 + (-0.86161321416614910843e-6 + (0.30565678902998530755e-6 + (-0.91884499221128689685e-7 + (0.23232167295343259066e-7 + (-0.48827798799360556781e-8 + (0.83839674785068315730e-9 + (-0.11465897079417236211e-9 + (0.12018375327114657495e-10 + (-0.90704383023344657506e-12 + (0.43891978494050177196e-13 + (-0.10229785550287397172e-14 + 0.78762463049180394663e-73 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c28 = 0.30198717233369596938e-7 + (-0.26470108300951418744e-6 + (0.11484217461677342633e-5 + (-0.32497913950022215997e-5 + (0.66506386195966268716e-5 + (-0.10285555161052384677e-4 + (0.12091874722611959811e-4 + (-0.10282092538544810074e-4 + (0.48262555421988436065e-5 + (0.22424493524723868069e-5 + (-0.80820796846961632989e-5 + (0.10796653917026920819e-4 + (-0.10296848856548453781e-4 + (0.78454106322971590499e-5 + (-0.49723665063048755405e-5 + (0.26692025174386628160e-5 + (-0.12235353625461559265e-5 + (0.48015929004768634100e-6 + (-0.16110110259511009705e-6 + (0.45993987610984026749e-7 + (-0.11082468980342148503e-7 + (0.22262384615144625788e-8 + (-0.36628003675664709291e-9 + (0.48106282932046855346e-10 + (-0.48521828528371470052e-11 + (0.35302160795771468331e-12 + (-0.16495114325484165433e-13 + (0.37178575828506531682e-15 + 0.24674957095607893065e-76 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    const Scalar c29 = 0.18511689126510260351e-7 + (-0.17068181783223545647e-6 + (0.77969745348337966326e-6 + (-0.23283626828310709094e-5 + (0.50491740099013299826e-5 + (-0.83365039291660230244e-5 + (0.10619684437634706088e-4 + (-0.10159658374625481776e-4 + (0.63391094420835344202e-5 + (-0.33689109880222793626e-6 + (-0.54986527825374819902e-5 + (0.90931526908756587315e-5 + (-0.97429399092296529333e-5 + (0.81477898036860525587e-5 + (-0.56292955614970245589e-5 + (0.32919777940121169458e-5 + (-0.16481984300967700802e-5 + (0.70993867832354502130e-6 + (-0.26326178871252171607e-6 + (0.83831263419606275090e-7 + (-0.22796773476702555683e-7 + (0.52479139309798472548e-8 + (-0.10097869367487702027e-8 + (0.15950303615432153340e-9 + (-0.20152759590530488405e-10 + (0.19590143225618303329e-11 + (-0.13759028734951606136e-12 + (0.62156462450904809840e-14 + (-0.13563652403372194113e-15 + 0.72106829618959360213e-80 * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta) * delta;
    return (tail ? -1.0 : 1.0) * v * (1.0 + v2 * (c1 + v2 * (c2 + v2 * (c3 + v2 * (c4 + v2 * (c5 + v2 * (c6 + v2 * (c7 + v2 * (c8 + v2 * (c9 + v2 * (c10 + v2 * (c11 + v2 * (c12 + v2 * (c13 + v2 * (c14 + v2 * (c15 + v2 * (c16 + v2 * (c17 + v2 * (c18 + v2 * (c19 + v2 * (c20 + v2 * (c21 + v2 * (c22 + v2 * (c23 + v2 * (c24 + v2 * (c25 + v2 * (c26 + v2 * (c27 + v2 * (c28 + v2 * (c29 + v2 * c30))))))))))))))))))))))))))))));
  }
  // Here we try to use an asymptotic tail expansion. We limit the expansion to the fifth term as the evaluation of the d coefficients becomes unstable for moderate to large nu
  //  const Scalar d5(-nu / (nu + 2.0) * (nu + 1.0) / (nu + 2.0) * (nu + 3.0) / (nu + 2.0) * (nu + 9.0) / (nu + 2.0) * (-0.1 + (0.725 + (-1.578125 + (0.46875 + (1.22890625 + (0.353125 + 0.02734375 * nu) * nu) * nu) * nu) * nu) * nu) / ((nu + 2.0) * (nu + 4.0) * (nu + 4.0) * (nu + 6.0) * (nu + 8.0) * (nu + 10.0)));
  const Scalar d4 = -nu / (nu + 2.0) * (nu + 1.0) / (nu + 2.0) * (nu + 7.0) / (nu + 2.0) * (0.16666666666666666667 + (-0.875 + (0.74479166666666666667 + (1.2109375 + (0.40104166666666666667 + 0.0390625 * nu) * nu) * nu) * nu) * nu) / ((nu + 2.0) * (nu + 4.0) * (nu + 4.0) * (nu + 6.0) * (nu + 8.0));
  const Scalar z = std::pow(std::sqrt(nu) * u * normalizationFactor, 1.0 / nu);
  const Scalar z2 = z * z;
  //  if (std::abs(d5 * std::pow(z2, 5)) < SpecFunc::Precision)
  if (std::abs(d4 * std::pow(z2, 4)) < SpecFunc::Precision)
  {
    const Scalar d1 = -0.5 * (nu + 1.0) / (nu + 2.0);
    const Scalar d2 = -0.125 * (nu / (nu + 2.0) * (nu + 1.0) / (nu + 2.0) * (nu + 3.0) / (nu + 4.0));
    const Scalar d3 = -nu / (nu + 2.0) * (nu + 1.0) / (nu + 2.0) * (nu + 5.0) / (nu + 2.0) * (-0.041666666666666666667 + (0.14583333333333333333 + 0.0625 * nu) * nu) / ((nu + 2.0) * (nu + 6.0));
    //      const Scalar d4(-nu / (nu + 2.0) * (nu + 1.0) / (nu + 2.0) * (nu + 7.0) / (nu + 2.0) * (0.16666666666666666667 + (-0.875 + (0.74479166666666666667 + (1.2109375 + (0.40104166666666666667 + 0.0390625 * nu) * nu) * nu) * nu) * nu) / ((nu + 2.0) * (nu + 4.0) * (nu + 4.0) * (nu + 6.0) * (nu + 8.0)));
    //      const Scalar value(std::sqrt(nu) / z * (1.0 + (d1 + (d2 + (d3 + (d4 + d5 * z2) * z2) * z2) * z2) * z2));
    const Scalar value = std::sqrt(nu) / z * (1.0 + (d1 + (d2 + (d3 + d4 * z2) * z2) * z2) * z2);
    return (tail == (p < 0.5) ? value : -value);
  }
#endif // USE_NEW_ALGO
  // Finally, if neither the central series nor the tail series apply, use the incomplete beta inverse function
  const Scalar cu = std::max(u, SpecFunc::MinScalar);
  const Scalar omega = std::sqrt(nu * (1.0 / SpecFunc::RegularizedIncompleteBetaInverse(0.5 * nu, 0.5, 2.0 * cu) - 1.0));
  return ((p > 0.5) == tail ? -omega : omega);
#endif // OPENTURNS_HAVE_BOOST
}

/* Random number generation
   We use a transformation method based on Gamma and Normal transformation:
   If N is Normal(0, 1) distributed and G is Gamma(nu / 2) distributed,
   sqrt(2 * nu) * N / sqrt(G) is distributed according to Student(nu)
*/
Scalar StudentRealization(const Scalar nu)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  const Scalar n = DistFunc::rNormal();
  Scalar g = -1.0;
  do
  {
    g = DistFunc::rGamma(0.5 * nu);
  }
  while (g == 0.0);
  return std::sqrt(0.5 * nu / g) * n;
}

/************************************************************************************************************/
/* Normalized NonCentralStudent distribution, i.e. with a PDF equals to (eq. 31.15 p.516 of the reference): */
/* exp(-delta^2 / 2) * (nu / (nu + x^2)) ^ ((nu + 1) / 2) / (sqrt(nu * Pi) * Gamma(nu / 2)) * SUM           */
/* where SUM = sum_0^inf Gamma((nu + k + 1) / 2) * omega^k / Gamma(k + 1)                                   */
/* and omega = x * delta * sqrt(2 / (nu + x^2))                                                             */
/* Reference:                                                                                               */
/* Norman L. Johnson, Samuel Kotz, N. Balakrishnan, "Continuous univariate distributions volume 2", second  */
/* edition, 1995, Wiley Inter-Science                                                                       */
/************************************************************************************************************/
/* CDF
   We use the algorithm described in:
   Viktor Witkovsky, "A Note on Computing Extreme Tail Probabilities of the Noncentral T Distribution with Large Noncentrality Parameter"
   Computational Statistics & Data Analysis, 43 (2003) pp 249-267
*/
Scalar NonCentralStudentCDF(const Scalar nu,
                            const Scalar delta,
                            const Scalar x,
                            const Bool tail)
{
  // Check nu
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu=" << nu << " should be strictly positive.";
  // Special case when |delta| << 1
  if (std::abs(delta) < 4.0 * SpecFunc::Precision * nu) return DistFunc::pStudent(nu, x, tail);
  // Very large nu
  if (nu > 1.0 / SpecFunc::Precision) return DistFunc::pNormal(x - nu);
  // Special case when |x| << 1
  if (std::abs(x) < SpecFunc::Precision) return DistFunc::pNormal(-delta, tail);
  // Small nu
  if (nu < 20.0)
  {
#ifdef OPENTURNS_HAVE_BOOST
    return (tail ? boost::math::cdf(boost::math::complement(boost::math::non_central_t(nu, delta), x)) : boost::math::cdf(boost::math::non_central_t(nu, delta), x));
#else
    return NonCentralStudentCDFAlt0(nu, delta, x, tail, SpecFunc::Precision, SpecFunc::MaximumIteration);
#endif
  }
  if (x < 0.0) return NonCentralStudentCDF(nu, -delta, -x, !tail);
  Bool computeTail = true;
  Bool useChiSquareTail = false;
  if (x < delta)
  {
    computeTail = false;
    useChiSquareTail = true;
  }

  /*******************************/
  /* Integration bounds and mode */
  /*******************************/
  Scalar lowerBound = 0.0;
  Scalar upperBound = 0.0;
  Scalar mode = 0.0;
  // const = -(log(2)+log(2*pi)/2)
  const Scalar const1 = -1.6120857137646180667900353000732139;
  // logRelTolBound = log(eps) eps = 2.220446049250313e-16
  const Scalar logRelTolBound = -3.604365338911715e+01;
  // zUpperBound > -norminv(eps(0)) =  3.847234634276879e+01
  const Scalar zUpperBound = 38.5;
  // tLower = log(1/(1-eps)) ~ eps
  const Scalar tUpper(7.208730677823431e+01); // tUpper = log(1/eps^2)
  const Scalar tLower(4.930380657631324e-32); // tLower = log(1/(1-eps^2)) ~ eps^2
  const Scalar nuMinus2 = std::max(1.0, (nu - 2.0));
  const Scalar xSquare = x * x;
  const Scalar halfNu = 0.5 * nu;

  // Estimate the position of the modus MOD of the FUNC
  mode = (x * std::sqrt(4 * nu * nuMinus2 + xSquare * (delta * delta + 4 * nuMinus2)) - delta * (xSquare + 2 * nu)) / (2 * (xSquare + nu));
  const Scalar dZ = std::min(0.5 * std::abs(mode + delta), 0.01);
  Point dMode(3);
  dMode[0] = mode - dZ;
  dMode[1] = mode;
  dMode[2] = mode + dZ;
  const Scalar theta = nu / xSquare;
  Point q(3);
  q[0] = theta * std::pow(dMode[0] + delta, 2);
  q[1] = theta * std::pow(dMode[1] + delta, 2);
  q[2] = theta * std::pow(dMode[2] + delta, 2);
  // Estimate the value of log(FUNC) around the mode point
  Point logFMode(3);
  logFMode[0] = const1 + 0.5 * (nuMinus2 * std::log(q[0] / nu) + nu - q[0] - dMode[0] * dMode[0]);
  logFMode[1] = const1 + 0.5 * (nuMinus2 * std::log(q[1] / nu) + nu - q[1] - dMode[1] * dMode[1]);
  logFMode[2] = const1 + 0.5 * (nuMinus2 * std::log(q[2] / nu) + nu - q[2] - dMode[2] * dMode[2]);
  // For given logRelTolBound estimate the logAbsoluteToleranceBound
  const Scalar logAbsoluteToleranceBound = logFMode[1] + logRelTolBound;
  // Estimate the integration limits by quadratic approximation
  const Scalar a = 0.5 * ((logFMode[0] - logFMode[1]) + (logFMode[2] - logFMode[1])) / (dZ * dZ);
  const Scalar b = 0.5 * (logFMode[0] - logFMode[2]) / dZ;
  const Scalar discriminantSqrt = std::sqrt(b * b + 4 * a * logRelTolBound);
  const Scalar denominator = 2 * a;
  Scalar lowerBound0 = std::max(-zUpperBound, (discriminantSqrt - b) / denominator);
  Scalar upperBound0 = std::max(-zUpperBound, std::min(zUpperBound, -(discriminantSqrt + b) / denominator));
  // Find zAbsoluteToleranceBound by solving: logAbsoluteToleranceBound = log(normpdf(z))
  const Scalar zAbsoluteToleranceBound = std::min(zUpperBound, std::sqrt(-1.8378770664093454835606594728112352 - 2 * logAbsoluteToleranceBound));
  if (!useChiSquareTail)
  {
    Scalar quantileUpper = 0.0;
    // Estimate quantile of chi^2 distribution
    // with nu degrees of freedom, see INGLOT (2010, Eqn. A.3)
    if (nu > 1) quantileUpper = std::max(0.0, nu + 2 * tUpper + 1.62 * std::sqrt(nu * tUpper) + 0.63012 * std::sqrt(nu) * std::log(tUpper) - 1.12032 * std::sqrt(nu) - 2.48 * std::sqrt(tUpper) - 0.65381 * std::log(tUpper) - 0.22872);
    else quantileUpper = 6.739648382445014e+01;
    const Scalar zQuantileUpper = std::sqrt((quantileUpper / nu) * xSquare) - delta;
    // Conservative estimate of the upper integration limit upperBound:
    // NORMPDF is sufficiently small OR CHI2CDF is close to 1.
    upperBound = std::min(zAbsoluteToleranceBound, zQuantileUpper);
    // For large nu we assume approximate symmetry of FUNC
    if ((nu > 1e4) && (mode > -zUpperBound) && (mode < zUpperBound)) lowerBound0 = mode - (upperBound - mode);
    lowerBound = std::max(-delta, lowerBound0);
  }
  else
  {
    Scalar quantileLower = 0.0;
    if (nu > 1) quantileLower = std::max(0.0, nu + 2 * tLower + 1.62 * std::sqrt(nu * tLower) + 0.63012 * std::sqrt(nu) * std::log(tLower) - 1.12032 * std::sqrt(nu) - 2.48 * std::sqrt(tLower) - 0.65381 * std::log(tLower) - 0.22872);
    else quantileLower = 0.0;
    const Scalar zQuantileLower = std::sqrt((quantileLower / nu) * xSquare) - delta;
    const Scalar lowerBound1 = std::max(-zAbsoluteToleranceBound, zQuantileLower);
    lowerBound = std::max(-delta, lowerBound1);
    if ((nu > 1e4) && (mode > -zUpperBound) && (mode < zUpperBound)) upperBound0 = mode + (mode - lowerBound);
    upperBound = upperBound0;
  }
  Scalar value = 0.0;
  // First, compute the Normal contribution
  if (useChiSquareTail) value = 0.5 * SpecFunc::ErfC(-lowerBound * 0.7071067811865475244);
  else value = 0.5 * SpecFunc::ErfC(upperBound * 0.7071067811865475244);
  // Second, compute the contribution of each subinterval by Gauss-Legendre integration with 15 nodes on 16 intervals
  const GaussKronrodRule rule(GaussKronrodRule::G7K15);
  const Scalar wg0 = rule.getZeroGaussWeight();
  const Point wg(rule.getOtherGaussWeights());
  const Point xg(rule.getOtherKronrodNodes());
  const UnsignedInteger iMax = 8;
  const Scalar dLowerBound = (mode - lowerBound) / iMax;
  const Scalar wLowerBound = 0.5 * dLowerBound;
  const Scalar dUpperBound = (mode - upperBound) / iMax;
  const Scalar wUpperBound = 0.5 * dUpperBound;
  const Scalar omega = nu / (2.0 * xSquare);
  for (UnsignedInteger i = 0; i < iMax; ++i)
  {
    // Ith interval at the left of the mode
    const Scalar ci = lowerBound + (i + 0.5) * dLowerBound;
    Scalar contributionLeft = wg0 * DistFunc::pGamma(halfNu, omega * std::pow(ci + delta, 2), useChiSquareTail) * DistFunc::dNormal(ci);
    // Ith interval at the right of the mode
    const Scalar xii = upperBound + (i + 0.5) * dUpperBound;
    Scalar contributionRight = wg0 * DistFunc::pGamma(halfNu, omega * std::pow(xii + delta, 2), useChiSquareTail) * DistFunc::dNormal(xii);
    for (UnsignedInteger j = 0; j < xg.getSize() / 2; ++j)
    {
      const Scalar zetaj = xg[2 * j + 1];
      // Contribution of the left interval
      const Scalar zj_m = ci - wLowerBound * zetaj;
      const Scalar zj_p = ci + wLowerBound * zetaj;
      contributionLeft += wg[j] * (DistFunc::pGamma(halfNu, omega * std::pow(zj_m + delta, 2), useChiSquareTail) * DistFunc::dNormal(zj_m) + DistFunc::pGamma(halfNu, omega * std::pow(zj_p + delta, 2), useChiSquareTail) * DistFunc::dNormal(zj_p));
      // Contribution of the right interval
      const Scalar zetaj_m = xii - wUpperBound * zetaj;
      const Scalar zetaj_p = xii + wUpperBound * zetaj;
      contributionRight += wg[j] * (DistFunc::pGamma(halfNu, omega * std::pow(zetaj_m + delta, 2), useChiSquareTail) * DistFunc::dNormal(zetaj_m) + DistFunc::pGamma(halfNu, omega * std::pow(zetaj_p + delta, 2), useChiSquareTail) * DistFunc::dNormal(zetaj_p));
    } // Loop over j, the integration points
    value += contributionLeft * wLowerBound - contributionRight * wUpperBound;
  } // Loop over i, the integration subintervals
  // Set the values of the CDF and CCDF
  return (computeTail == tail ? value : 0.5 + (0.5 - value));
}

/* CDF Alt0
   We use the algorithm described in:
   Denise Benton, K. Krishnamoorthy, "Computing discrete mixtures of continuous distributions: noncentral chisquare, noncentral t
   and the distribution of the square of the sample multiple correlation coefficient",
   Computational Statistics & Data Analysis, 43 (2003) pp 249-267
*/
Scalar NonCentralStudentCDFAlt0(const Scalar nu,
                                const Scalar delta,
                                const Scalar x,
                                const Bool tail,
                                const Scalar precision,
                                const UnsignedInteger maximumIteration)
{
  // Check nu
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu=" << nu << " should be strictly positive.";
  // Special case when |delta| << 1
  if (std::abs(delta / (4.0 * nu)) < precision) return DistFunc::pStudent(nu, x, tail);
  // Very large nu
  if (nu > 1.0 / precision) return DistFunc::pNormal(x - nu);
  // Special case when |x| << 1
  if (std::abs(x) < precision) return DistFunc::pNormal(-delta, tail);
  Scalar t = x;
  Scalar del = delta;
  // Must use the complementary function for negative arguments
  if (x < 0.0)
  {
    t = -t;
    del = -del;
  }
  // Some useful quantities
  const Scalar x2 = t * t;
  const Scalar xi = x2 / (nu + x2);
  const Scalar logXi = std::log(xi);
  const Scalar halfNu = 0.5 * nu;
  const Scalar halfDelta2 = 0.5 * del * del;
  const Scalar logHalfDelta2 = std::log(halfDelta2);
  // Starting index in the sum: integral part of halfDelta2 and insure that it is at least 1
  const UnsignedInteger k = std::max(1UL, static_cast<UnsignedInteger>(floor(halfDelta2)));
  // Index of the forward iterations
  UnsignedInteger kForward = k;
  // Index of the backward iterations
  UnsignedInteger kBackward = k;
  LOGDEBUG(OSS() << "kForward=" << kForward << "kBackward=" << kBackward);
  // Terms and factors of the summation.
  // The initialization corresponds to the terme of index k.
  const Scalar commonExponent = -halfDelta2 + k * logHalfDelta2;
  LOGDEBUG(OSS() << "commonExponent=" << commonExponent);
  Scalar pForward = 0.5 * std::exp(commonExponent - SpecFunc::LnGamma(k + 1));
  Scalar qForward = 0.5 * del / M_SQRT2 * std::exp(commonExponent - SpecFunc::LnGamma(k + 1.5));
  LOGDEBUG(OSS() << "pForward=" << pForward << ", qForward=" << qForward);
  Scalar betaPForward = DistFunc::pBeta(k + 0.5, halfNu, xi);
  Scalar betaQForward = DistFunc::pBeta(k + 1, halfNu, xi);
  LOGDEBUG(OSS() << "betaPForward=" << betaPForward << ", betaQForward=" << betaQForward);
  // The correction factors will be updated at the beginning of each iteration: it is the quantity to add to
  // the corresponding betaP and betaQ factors to go to the value associated with the current iteration.
  // They are thus initialized such that after one update, they will change the betaP and betaQ factors
  // to their values associated with the iteration (k-1):
  const Scalar commonFactor = (k - 0.5) * logXi + halfNu * std::log(nu / (nu + x2)) - SpecFunc::LnGamma(halfNu);
  LOGDEBUG(OSS() << "commonFactor=" << commonFactor);
  // correctionBetaPForward = Gamma(k - 1/2 + nu/2) / Gamma(k + 1/2) / Gamma(nu/2) * xi^(k - 1/2) * (1 - xi)^(nu/2)
  Scalar correctionBetaPForward = -std::exp(SpecFunc::LnGamma(k - 0.5 + halfNu) - SpecFunc::LnGamma(k + 0.5) + commonFactor);
  // correctionBetaPForward = Gamma(k + nu/2) / Gamma(k + 1) / Gamma(nu/2) * xi^k * (1 - xi)^(nu/2)
  Scalar correctionBetaQForward = -std::exp(SpecFunc::LnGamma(k + halfNu) - SpecFunc::LnGamma(k + 1) + commonFactor + 0.5 * logXi);
  LOGDEBUG(OSS() << "correctionBetaPForward=" << correctionBetaPForward << ", correctionBetaQForward=" << correctionBetaQForward);
  Scalar pBackward = pForward;
  Scalar qBackward = qForward;
  Scalar betaPBackward = betaPForward;
  Scalar betaQBackward = betaQForward;
  // The correction factors will be updated at the beginning of each iteration: it is the quantity to add to
  // the corresponding betaP and betaQ factors to go to the value associated with the current iteration.
  // They are thus initialized such that after one update, they will change the betaP and betaQ factors
  // to their values associated with the iteration (k+1):
  // correctionBetaPBackward = Gamma(k + 1/2 + nu/2) / Gamma(k + 3/2 + nu/2) / Gamma(nu/2) * xi^(k + 1/2) * (1 - xi)^(nu/2)
  Scalar correctionBetaPBackward = -correctionBetaPForward * xi * (k - 0.5 + halfNu) / (k + 0.5);
  Scalar correctionBetaQBackward = -correctionBetaQForward * xi * (k + halfNu) / (k + 1);
  LOGDEBUG(OSS() << "correctionBetaPBackward=" << correctionBetaPBackward << ", correctionBetaQBackward=" << correctionBetaQBackward);
  Scalar value = DistFunc::pNormal(-del) + pForward * betaPForward + qForward * betaQForward;
  Scalar contributionForward = 0.0;
  Scalar contributionBackward = 0.0;
  Scalar error = SpecFunc::MaxScalar;
  // At the beginning of the iteration, kForward and kBackward store the index of the last terms
  // that have already been accumulated. Each iteration must update the P and Q factors, as well
  // as the betaP and betaQ factors. For this last update, one must update the corresponding
  // correction factors. Thus, we proceed as follows:
  // First, update the correction factors for the betaP and betaQ factors
  // Second, update the betaP and betaQ factors
  // Third, update the P and Q factors
  // Fourth, accumulate the current contribution
  // It is the responsibility of the main iteration to update kForward and kBackward
#define FORWARD_ITERATION                                               \
    correctionBetaPForward *= xi * (kForward - 0.5 + halfNu) / (kForward + 0.5); \
    correctionBetaQForward *= xi * (kForward + halfNu) / (kForward + 1); \
    pForward *= halfDelta2 / (kForward + 1);                            \
    betaPForward += correctionBetaPForward;                             \
    qForward *= halfDelta2 / (kForward + 1.5);                          \
    betaQForward += correctionBetaQForward;                             \
    contributionForward = pForward * betaPForward + qForward * betaQForward; \
    value += contributionForward;

#define BACKWARD_ITERATION                                              \
    correctionBetaPBackward *= (kBackward + 0.5) / (xi * (kBackward - 0.5 + halfNu)); \
    correctionBetaQBackward *= (kBackward + 1) / (xi * (kBackward + halfNu)); \
    pBackward *= kBackward / halfDelta2;                                \
    betaPBackward += correctionBetaPBackward;                           \
    qBackward *= (kBackward + 0.5) / halfDelta2;                        \
    betaQBackward += correctionBetaQBackward;                           \
    contributionBackward = pBackward * betaPBackward + qBackward * betaQBackward; \
    value += contributionBackward;

  // Here, i is an UnsignedInteger as it is only a loop counter
  UnsignedInteger i = 1;

  const UnsignedInteger imax = std::min(k, maximumIteration);
  while((error > 0.0) && (i <= imax))
  {
    FORWARD_ITERATION;
    BACKWARD_ITERATION;
    error = contributionForward + contributionBackward;
    ++kForward;
    --kBackward;
    ++i;
  }
  // Do we have to perform further forward iterations?
  while ((error > 0.0) && (i <= maximumIteration))
  {
    FORWARD_ITERATION;
    error = contributionForward;
    ++kForward;
    ++i;
  }
#undef FORWARD_ITERATION
#undef BACKWARD_ITERATION
  if (error > precision * (std::abs(value) + precision)) LOGWARN(OSS() << "Warning: in NonCentralStudentAlt0(nu, delta, x), no convergence after " << i << " iterations. Error is " << error * value << " value is " << value << " for nu=" << nu << ", delta=" << delta << " and x=" << x);
  // Clip to [0,1] in order to get rid of small rounding error
  value = (value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value));
  // Check if we had to change the sign of the argument or if we are asked for the tail CDF
  if ((tail && (x > 0.0)) || (!tail && (x < 0.0))) value = 0.5 + (0.5 - value);
  return value;
}

/* PDF
   We use the relation between the PDF and the CDF in order to reduce the computation of the PDF to two computations of the CDF */
Scalar NonCentralStudentPDF(const Scalar nu,
                            const Scalar delta,
                            const Scalar x)
{
  // Check nu
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu=" << nu << " should be strictly positive.";
  // Early exit for delta == 0, central Student PDF
  if (std::abs(delta / (4.0 * nu)) < SpecFunc::Precision) return std::exp(SpecFunc::LnGamma(0.5 * nu + 0.5) - SpecFunc::LnGamma(0.5 * nu) - 0.5 * std::log(M_PI * nu) + (0.5 * nu + 0.5) * std::log(nu / (nu + x * x)));
  if (std::abs(x) < SpecFunc::Precision) return std::exp(SpecFunc::LnGamma(0.5 * nu + 0.5) - SpecFunc::LnGamma(0.5 * nu) - 0.5 * std::log(M_PI * nu) - 0.5 * delta * delta);
  return std::max(0.0, nu / x * (NonCentralStudentCDF(nu + 2, delta, x * std::sqrt(1.0 + 2.0 / nu)) - NonCentralStudentCDF(nu, delta, x)));
}

/************************************************************************************************************/
/* Normalized NonCentralStudent distribution, i.e. with a PDF equals to (eq. 31.15 p.516 of the reference): */
/* exp(-delta^2 / 2) * (nu / (nu + x^2)) ^ ((nu + 1) / 2) / (sqrt(nu * Pi) * Gamma(nu / 2)) * SUM           */
/* where SUM = sum_0^inf Gamma((nu + k + 1) / 2) * omega^k / Gamma(k + 1)                                   */
/* and omega = x * delta * sqrt(2 / (nu + x^2))                                                             */
/* In order to derive simple update formulas for the terms in the sum, we separate the odd indices from the */
/* even ones:                                                                                               */
/* SUM = SUM_ODD + SUM_EVEN, where:                                                                         */
/* SUM_ODD  = sum_0^inf Gamma(nu / 2 + k + 1) * omega * z^k / Gamma(2 * k + 2)                              */
/* SUM_EVEN = sum_O^inf Gamma((nu + 1) / 2 + k) * z^k / Gamma(2 * k + 1)                                    */
/* and z = omega^2                                                                                          */
/* The summation is done starting at the kM chosen as for the NonCentralStudentCDFAlt0 method.                    */
/* Reference:                                                                                               */
/* Norman L. Johnson, Samuel Kotz, N. Balakrishnan, "Continuous univariate distributions volume 2", second  */
/* edition, 1995, Wiley Inter-Science                                                                       */
/************************************************************************************************************/
Scalar NonCentralStudentPDFAlt0(const Scalar nu,
                                const Scalar delta,
                                const Scalar x,
                                const Scalar precision,
                                const UnsignedInteger maximumIteration)
{
  // Check nu
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu=" << nu << " should be strictly positive.";
  // Early exit for delta == 0, central Student PDF
  if (std::abs(delta / (4.0 * nu)) < precision) return std::exp(SpecFunc::LnGamma(0.5 * nu + 0.5) - SpecFunc::LnGamma(0.5 * nu) - 0.5 * std::log(M_PI * nu) + (0.5 * nu + 0.5) * std::log(nu / (nu + x * x)));
  // Case delta <> 0
#ifdef OPENTURNS_HAVE_BOOST
  (void) maximumIteration;
  return boost::math::pdf(boost::math::non_central_t(nu, delta), x);
#else
  const Scalar halfNu = 0.5 * nu;
  const Scalar halfNup1_2 = halfNu + 0.5;
  const Scalar logConstant = -0.5 * delta * delta - SpecFunc::LnGamma(halfNu) - 0.5 * std::log(M_PI * nu);
  // Early exit for x == 0
  if (std::abs(x) < precision) return std::exp(logConstant + SpecFunc::LnGamma(halfNup1_2));
  // For x <> 0
  const Scalar x2 = x * x;
  const Scalar w = 1.0 / (nu + x2);
  Scalar logFactor = logConstant + halfNup1_2 * std::log(nu * w);
  // Special treatment for very low value to avoid NaNs due to 0.Inf
  static const Scalar logPrecision(std::log(precision));
  if (logFactor < logPrecision)
  {
    Scalar value = 0.0;
    if (x < 0.0) value = nu / x * (NonCentralStudentCDF(nu + 2.0, delta, x * std::sqrt(1.0 + 2.0 / nu), false) - NonCentralStudentCDF(nu, delta, x, false));
    else value = -nu / x * (NonCentralStudentCDF(nu + 2.0, -delta, -x * std::sqrt(1.0 + 2.0 / nu), false) - NonCentralStudentCDF(nu, -delta, -x));
    return std::max(0.0, value);
  }

  const Scalar omega = delta * x * std::sqrt(2 * w);
  const Scalar z = omega * omega;
  // Start at even index that maximize the coefficient in the sum
  const Scalar halfDelta2 = 0.5 * delta * delta;
  // Starting index in the sum: integral part of halfDelta2 and insure that it is at least 1
  const UnsignedInteger k = std::max(1UL, static_cast<UnsignedInteger>(floor(halfDelta2)));
  // Loop forward and backward starting from k
  // Initialization
  const Scalar kLogZ = k * std::log(z);
  Scalar pForwardEven = std::exp(logFactor + SpecFunc::LnGamma(halfNup1_2 + k) - SpecFunc::LnGamma(2 * k + 1) + kLogZ);
  Scalar pForwardOdd = omega * std::exp(logFactor + SpecFunc::LnGamma(halfNu + k + 1) - SpecFunc::LnGamma(2 * k + 2) + kLogZ);
  Scalar pBackwardEven = pForwardEven;
  Scalar pBackwardOdd = pForwardOdd;
  Scalar value = pForwardOdd + pForwardEven;
  Scalar error = SpecFunc::MaxScalar;
  UnsignedInteger kForward = k;
  UnsignedInteger kBackward = k;
#define FORWARD_ITERATION                                               \
    pForwardOdd *= (halfNu + kForward + 1) * z / (2 * (kForward + 1) * (2 * kForward + 3)); \
    pForwardEven *= (halfNup1_2 + kForward) * z / (2 * (kForward + 1) * (2 * kForward + 1)); \
    value += pForwardOdd + pForwardEven;
#define BACKWARD_ITERATION                                              \
    pBackwardOdd *= 2 * kBackward * (2 * kBackward + 1) / (z * (halfNu + kBackward)); \
    pBackwardEven *= 2 * kBackward * (2 * kBackward - 1) / (z * (halfNup1_2 + kBackward - 1)); \
    value += pBackwardOdd + pBackwardEven;

  // Here, i is an UnsignedInteger as it is only a loop counter
  UnsignedInteger i = 1;
  const UnsignedInteger imax = std::min(k, maximumIteration);
  //        while((error > precision * (std::abs(value) + precision)) && (i <= imax))
  while((error > 0.0) && (i <= imax))
  {
    FORWARD_ITERATION;
    BACKWARD_ITERATION;
    error = pForwardOdd + pBackwardOdd + pForwardEven + pBackwardEven;
    ++kForward;
    --kBackward;
    ++i;
  }
  // Do we have to perform further forward iterations?
  //        while ((error > precision * (std::abs(value) + precision)) && (i <= MaximumIteration))
  while ((error > 0.0) && (i <= maximumIteration))
  {
    FORWARD_ITERATION;
    error = pForwardOdd + pForwardEven;
    ++kForward;
    ++i;
  }
#undef FORWARD_ITERATION
#undef BACKWARD_ITERATION
  if (error > precision * (std::abs(value) + precision)) LOGWARN(OSS() << "Warning: in NonCentralStudentPDFAlt0(nu, delta, x), no convergence after " << i << " iterations. Error is " << error * value << " value is " << value << " for nu=" << nu << ", delta=" << delta << " and x=" << x);
  // Clip to [0,+inf[ in order to get rid of small rounding error
  return (value <= 0.0 ? 0.0 : value);
#endif
}

/* Random number generation
   We use a transformation method based on Gamma and Normal transformation:
   If N is Normal(delta, 1) distributed and G is Gamma(nu / 2) distributed,
   sqrt(2 * nu) * N / sqrt(G) is distributed according to NonCentralStudent(nu, delta)
*/
Scalar NonCentralStudentRealization(const Scalar nu,
                                    const Scalar delta)
{
  const Scalar n = DistFunc::rNormal() + delta;
  Scalar g = -1.0;
  do
  {
    g = DistFunc::rGamma(0.5 * nu);
  }
  while (g == 0.0);
  return std::sqrt(0.5 * nu / g) * n;
}

} // StudentFunctions

END_NAMESPACE_OPENTURNS
