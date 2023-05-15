//                                               -*- C++ -*-
/**
 *  @brief Result of GEV profile likelihood
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
#include "openturns/ProfileLikelihoodResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Text.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProfileLikelihoodResult)

static const Factory<ProfileLikelihoodResult> Factory_ProfileLikelihoodResult;


ProfileLikelihoodResult::ProfileLikelihoodResult()
  : DistributionFactoryLikelihoodResult()
{}

ProfileLikelihoodResult::ProfileLikelihoodResult(const Distribution & distribution,
    const Distribution & parameterDistribution,
    const Scalar logLikelihood,
    const Function & profileLikelihoodFunction,
    const Scalar parameter)
  : DistributionFactoryLikelihoodResult(distribution, parameterDistribution, logLikelihood)
  , profileLikelihoodFunction_(profileLikelihoodFunction)
  , parameter_(parameter)
{}

ProfileLikelihoodResult * ProfileLikelihoodResult::clone() const
{
  return new ProfileLikelihoodResult(*this);
}

void ProfileLikelihoodResult::setConfidenceLevel(const Scalar confidenceLevel)
{
  confidenceLevel_ = confidenceLevel;
}

Scalar ProfileLikelihoodResult::getConfidenceLevel() const
{
  return confidenceLevel_;
}

UnsignedInteger ProfileLikelihoodResult::getParameterIndex() const
{
  const String pName(profileLikelihoodFunction_.getInputDescription()[0]);
  const UnsignedInteger pIndex = getParameterDistribution().getDescription().find(pName);
  if (pIndex >= getParameterDistribution().getDimension())
    throw InvalidArgumentException(HERE) << "Cannot find parameter " << pName;
  return pIndex;
}

Scalar ProfileLikelihoodResult::getThreshold() const
{
  // quantile of ChiSquare(1.0)
  const Scalar cAlpha = 2.0 * DistFunc::qGamma(0.5, confidenceLevel_);

  const Scalar fx = getLogLikelihood();
  const Scalar threshold = fx - 0.5 * cAlpha;
  return threshold;
}

Interval ProfileLikelihoodResult::getParameterConfidenceInterval() const
{
  const Scalar x = parameter_;
  const Scalar fx = getLogLikelihood();
  const Scalar threshold = getThreshold();

  // search for raw lower/upper bounds where the likelihood gets lower than the threshold
  const Scalar epsilon = ResourceMap::GetAsScalar("ProfileLikelihoodResult-StartingScaling");
  Scalar lb = x - epsilon;
  Scalar flb = profileLikelihoodFunction_(Point({lb}))[0];
  Scalar scaling = epsilon;
  while ((flb - threshold <= 0.0) == (fx - threshold <= 0.0))
  {
    if (!SpecFunc::IsNormal(flb))
      throw InvalidArgumentException(HERE) << "inf @" << lb;
    scaling *= 2.0;
    lb -= scaling;
    flb = profileLikelihoodFunction_(Point({lb}))[0];
  }
  scaling = epsilon;
  Scalar ub = x + epsilon;
  Scalar fub = profileLikelihoodFunction_(Point({ub}))[0];
  while ((fub - threshold <= 0.0) == (fx - threshold <= 0.0))
  {
    if (!SpecFunc::IsNormal(fub))
      throw InvalidArgumentException(HERE) << "inf @" << ub;
    scaling *= 2.0;
    ub += scaling;
    fub = profileLikelihoodFunction_(Point({ub}))[0];
  }

  // now look for the intersection with the threshold (with controlled precision this time)
  Brent solver(ResourceMap::GetAsScalar("ProfileLikelihoodResult-AbsolutePrecision"),
               ResourceMap::GetAsScalar("ProfileLikelihoodResult-RelativePrecision"));
  const Scalar xl = solver.solve(profileLikelihoodFunction_, threshold, lb, x, flb, fx);
  const Scalar xu = solver.solve(profileLikelihoodFunction_, threshold, x, ub, fx, fub);
  return Interval(xl, xu);
}

Graph ProfileLikelihoodResult::drawProfileLikelihoodFunction() const
{
  const Interval ci(getParameterConfidenceInterval());
  const Scalar delta = ci.getVolume();
  const Scalar ciMargin = ResourceMap::GetAsScalar("ProfileLikelihoodResult-ConfidenceIntervalMargin");
  const Scalar xMin = ci.getLowerBound()[0] - ciMargin * delta;
  const Scalar xMax = ci.getUpperBound()[0] + ciMargin * delta;
  const Scalar threshold = getThreshold();
  const Scalar x = parameter_;
  const Scalar fx = getLogLikelihood();

  Graph result(profileLikelihoodFunction_.draw(xMin, xMax));
  result.setLegends({"likelihood"});
  String inputVar(profileLikelihoodFunction_.getInputDescription()[0]);
  // use latex syntax
  if (inputVar == "xi")
    inputVar = "\\xi";
  else if (inputVar == "zm")
    inputVar = "z_m";
  result.setXTitle("$" + inputVar + "$");
  result.setYTitle("profile log-likelihood value");
  result.setTitle("profile likelihood");

  const Interval bbox(result.getDrawable(0).getBoundingBox());
  const Scalar textMargin = ResourceMap::GetAsScalar("ProfileLikelihoodResult-TextMargin");
  const Scalar dx = textMargin * bbox.getMarginal(0).getVolume();
  const Scalar dy = textMargin * bbox.getMarginal(1).getVolume();

  // add threshold horizontal line
  Point dataX = {xMin, xMax};
  Point dataY = {threshold, threshold};
  Curve hLine(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {xMin};
  dataY = {threshold + dy};
  Text elt(dataX, dataY, {OSS() << "thr=" << threshold}, "right");
  elt.setColor("black");
  result.add(elt);

  // add fx horizontal line
  dataX = {xMin, xMax};
  dataY = {fx, fx};
  hLine = Curve(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {xMin};
  dataY = {fx + dy};
  elt = Text(dataX, dataY, {OSS() << "$L(\\hat{" + inputVar + "})$=" << fx}, "right");
  elt.setColor("black");
  result.add(elt);

  // add lower bound vertical line
  dataX = {ci.getLowerBound()[0], ci.getLowerBound()[0]};
  dataY = {bbox.getLowerBound()[1], bbox.getUpperBound()[1]};
  hLine = Curve(dataX, dataY, OSS() << "CI @ " << getConfidenceLevel());
  hLine.setColor("red");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {ci.getLowerBound()[0] + dx};
  dataY = {bbox.getLowerBound()[1]};
  elt = Text(dataX, dataY, {OSS() << "lb=" << ci.getLowerBound()[0]}, "right");
  elt.setColor("red");
  elt.setRotation(90.0);
  result.add(elt);

  // add upper bound vertical line
  dataX = {ci.getUpperBound()[0], ci.getUpperBound()[0]};
  dataY = {bbox.getLowerBound()[1], bbox.getUpperBound()[1]};
  hLine = Curve(dataX, dataY);
  hLine.setColor("red");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {ci.getUpperBound()[0] + dx};
  dataY = {bbox.getLowerBound()[1]};
  elt = Text(dataX, dataY, {OSS() << "ub=" << ci.getUpperBound()[0]}, "right");
  elt.setColor("red");
  elt.setRotation(90.0);
  result.add(elt);

  // add x vertical line
  dataX = {x, x};
  dataY = {bbox.getLowerBound()[1], bbox.getUpperBound()[1]};
  hLine = Curve(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {x + dx};
  dataY = {bbox.getLowerBound()[1]};
  elt = Text(dataX, dataY, {OSS() << "$\\hat{" + inputVar + "}$=" << ci.getUpperBound()[0]}, "right");
  elt.setColor("black");
  elt.setRotation(90.0);
  result.add(elt);
  result.setLegendPosition("topright");
  return result;
}

String ProfileLikelihoodResult::__repr__() const
{
  return OSS() << DistributionFactoryLikelihoodResult::__repr__()
         << " confidenceLevel_=" << confidenceLevel_;
}

Function ProfileLikelihoodResult::getProfileLikelihoodFunction() const
{
  return profileLikelihoodFunction_;
}

/* Parameter value accessor */
Scalar ProfileLikelihoodResult::getParameter() const
{
  return parameter_;
}

/* Method save() stores the object through the StorageManager */
void ProfileLikelihoodResult::save(Advocate & adv) const
{
  DistributionFactoryLikelihoodResult::save(adv);
  adv.saveAttribute("profileLikelihoodFunction_", profileLikelihoodFunction_);
  adv.saveAttribute("confidenceLevel_", confidenceLevel_);
  adv.saveAttribute("parameter_", parameter_);
}

/* Method load() reloads the object from the StorageManager */
void ProfileLikelihoodResult::load(Advocate & adv)
{
  DistributionFactoryLikelihoodResult::load(adv);
  adv.loadAttribute("profileLikelihoodFunction_", profileLikelihoodFunction_);
  adv.loadAttribute("confidenceLevel_", confidenceLevel_);
  adv.loadAttribute("parameter_", parameter_);
}

END_NAMESPACE_OPENTURNS
