//                                               -*- C++ -*-
/**
 *  @brief The test file of class Normal for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  // UnsignedInteger pointNumber(251);

  try
  {
    Normal dist1(1.0, 0.5);
    Graph graph;
    fullprint << "dist1:" << Distribution(dist1) << std::endl;
    Distribution result;
    result = dist1 + 2.0;
    fullprint << "dist1+2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_plus_2.png");
    result = dist1 - 2.0;
    fullprint << "dist1-2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_moins_2.png");
    result = dist1 * 2.0;
    fullprint << "dist1*2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_fois_2.png");
    result = dist1 / 2.0;
    fullprint << "dist1/2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_div_2.png");

    result = dist1.cos();
    fullprint << "cos(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_cos.png");
    result = dist1.sin();
    fullprint << "sin(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_sin.png");
    result = dist1.tan();
    fullprint << "tan(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_tan.png");

    Uniform dist0(-1.0, 1.0);
    result = dist0.acos();
    fullprint << "acos(dist0):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist0_acos.png");
    result = dist0.asin();
    fullprint << "asin(dist0):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist0_asin.png");
    result = dist0.atan();
    fullprint << "atan(dist0):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist0_atan.png");

    result = dist1.cosh();
    fullprint << "cosh(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_cosh.png");
    result = dist1.sinh();
    fullprint << "sinh(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_sinh.png");
    result = dist1.tanh();
    fullprint << "tanh(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_tanh.png");

    LogNormal distG1(1.0, 1.0, 1.0);
    result = distG1.acosh();
    fullprint << "acosh(distG1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("distG1_acosh.png");
    result = dist1.asinh();
    fullprint << "asinh(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_asinh.png");
    result = dist0.atanh();
    fullprint << "atanh(dist0):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist0_atanh.png");

    result = dist1.exp();
    fullprint << "exp(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_exp.png");
    result = distG1.log();
    fullprint << "log(distG1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("distG1_log.png");
    result = distG1.ln();
    fullprint << "ln(distG1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("distG1_ln.png");

    result = dist1.pow(SignedInteger(3));
    fullprint << "dist1^3:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_pow.png");
    result = distG1.pow(2.5);
    fullprint << "dist1^2.5:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("distG1_pow.png");
    result = dist1.inverse();
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(3);
    fullprint << "inverse(dist1):" << result << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_inverse.png");
    result = dist1.sqr();
    fullprint << "sqr(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_sqr.png");
    result = distG1.sqrt();
    fullprint << "sqrt(distG1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("distG1_sqrt.png");
    result = dist1.cbrt();
    fullprint << "cbrt(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_cbrt.png");
    result = dist1.abs();
    fullprint << "abs(dist1):" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_abs.png");

    Normal dist2(-2.0, 1.0);
    result = dist1 + dist2;
    fullprint << "dist1+dist2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_plus_dist2.png");
    result = dist1 - dist2;
    fullprint << "dist1-dist2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_moins_dist2.png");
    result = dist1 * dist2;
    fullprint << "dist1*dist2:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_fois_dist2.png");
    result = dist1 / dist2;
    PlatformInfo::SetNumericalPrecision(3);
    fullprint << "dist1/dist2:" << result << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);
    // graph = result.drawPDF(1024);
    // graph.draw("dist1_div_dist2.png");

    result = LogNormal() * LogNormal();
    fullprint << "logn*logn:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("logn_fois_logn.png");
    result = LogUniform() * LogUniform();
    fullprint << "logu*logu:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("logu_fois_logu.png");
    result = LogUniform() * LogNormal();
    fullprint << "logu*logn:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("logu_fois_logn.png");
    result = LogNormal() * LogUniform();
    fullprint << "logn*logu:" << result << std::endl;
    // graph = result.drawPDF(1024);
    // graph.draw("logn_fois_logu.png");
    // Ticket #917
    // X+Y
    result = WeibullMin() + Exponential();
    fullprint << "result=" << result << std::endl;
    fullprint << "cdf(1.0)=" << result.computeCDF(1.0) << std::endl;
    // -X+Y->bug using operators, as -X returns a smart pointer,
    // which is promoted into a Scalar by +!
    //result = (WeibullMin() * (-1.0)) + Exponential();
    //fullprint << "result=" << result << std::endl;
    //fullprint << "cdf(1.0)=" << result.computeCDF(1.0) << std::endl;
    // so we do it by hand
    {
      Collection<Distribution> coll(2);
      coll[0] = WeibullMin();
      coll[1] = Exponential();
      Point weights(2);
      weights[0] = -1.0;
      weights[1] =  1.0;
      result = RandomMixture(coll, weights);
      fullprint << "result=" << result << std::endl;
      fullprint << "cdf(1.0)=" << result.computeCDF(1.0) << std::endl;
    }
    // X-Y
    result = WeibullMin() - Exponential();
    fullprint << "result=" << result << std::endl;
    fullprint << "cdf(1.0)=" << result.computeCDF(1.0) << std::endl;
    // -X-Y->bug using operators, as -X returns a smart pointer,
    // which is promoted into a Scalar by +!
    //result = WeibullMin() * (-1.0) - Exponential();
    //fullprint << "result=" << result << std::endl;
    //fullprint << "cdf(-1.0)=" << result.computeCDF(-1.0) << std::endl;
    // so we do it by hand
    {
      Collection<Distribution> coll(2);
      coll[0] = WeibullMin();
      coll[1] = Exponential();
      Point weights(2);
      weights[0] = -1.0;
      weights[1] = -1.0;
      result = RandomMixture(coll, weights);
      fullprint << "result=" << result << std::endl;
      fullprint << "cdf(-1.0)=" << result.computeCDF(-1.0) << std::endl;
    }
    // Bug when there is a single atom after simplification:
    // the nonzero constants were not taken into account
    {
      result = Poisson(5.0) + 1.0;
      fullprint << "result=" << result << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
