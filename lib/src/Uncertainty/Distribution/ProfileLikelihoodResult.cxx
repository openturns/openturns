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
#include "openturns/ChiSquare.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Text.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProfileLikelihoodResult)

static const Factory<ProfileLikelihoodResult> Factory_ProfileLikelihoodResult;


ProfileLikelihoodResult::ProfileLikelihoodResult()
 : LikelihoodResult()
 {}

ProfileLikelihoodResult::ProfileLikelihoodResult(const Distribution & distribution,
                                                 const Distribution & parameterDistribution,
                                                 const Scalar logLikelihood,
                                                const Function & profileLikelihoodFunction)
 : LikelihoodResult(distribution, parameterDistribution, logLikelihood)
 , profileLikelihoodFunction_(profileLikelihoodFunction)
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

Scalar ProfileLikelihoodResult::getThreshold() const
{
  const Scalar cAlpha = ChiSquare(1.0).computeQuantile(confidenceLevel_)[0];
  const Scalar xi = getDistribution().getParameter()[2];
  const Scalar fxi = profileLikelihoodFunction_(Point(1, xi))[0];
  const Scalar threshold = fxi - 0.5 * cAlpha;
  return threshold;
}

Interval ProfileLikelihoodResult::getXiConfidenceInterval() const
{
  const Scalar lb = -2.0;
  const Scalar ub = 2.0;
  const Scalar flb = profileLikelihoodFunction_(Point(1, lb))[0];
  const Scalar fub = profileLikelihoodFunction_(Point(1, ub))[0];
  const Scalar xi = getDistribution().getParameter()[2];
  const Scalar fxi = profileLikelihoodFunction_(Point(1, xi))[0];
  const Scalar threshold = getThreshold();

  // we look for the intersections on the horizontal line y=threshold
  Brent solver(ResourceMap::GetAsScalar("ProfileLikelihoodResult-AbsolutePrecision"),
               ResourceMap::GetAsScalar("ProfileLikelihoodResult-RelativePrecision"));
  const Scalar xil = solver.solve(profileLikelihoodFunction_, threshold, lb, xi, flb, fxi);
  const Scalar xiu = solver.solve(profileLikelihoodFunction_, threshold, xi, ub, fxi, fub);
  return Interval(xil, xiu);
}

Graph ProfileLikelihoodResult::drawProfileLikelihood() const
{
  const Interval ci(getXiConfidenceInterval());
  const Scalar delta = ci.getVolume();
  const Scalar xMin = ci.getLowerBound()[0] - 0.15 * delta;
  const Scalar xMax = ci.getUpperBound()[0] + 0.15 * delta;
  const Scalar threshold = getThreshold();
  const Scalar xi = getDistribution().getParameter()[2];
  const Scalar fxi = profileLikelihoodFunction_(Point(1, xi))[0];

  Graph result(profileLikelihoodFunction_.draw(xMin, xMax));
  result.setLegends({"likelihood"});
  result.setXTitle("shape parameter $\\xi$");
  result.setYTitle("profile log-likelihood value");
  result.setTitle("profile likelihood");

  const Interval bbox(result.getDrawable(0).getBoundingBox());
  const Scalar dx = 0.03 * bbox.getMarginal(0).getVolume();
  const Scalar dy = 0.03 * bbox.getMarginal(1).getVolume();

  // add threshold horizontal line
  Point dataX = {xMin, xMax};
  Point dataY = {threshold, threshold};
  Curve hLine(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {xMin};
  dataY = {threshold + dy};
  Text elt(dataX, dataY, {OSS() << "thr="<< threshold}, "right");
  elt.setColor("black");
  result.add(elt);

  // add fxi horizontal line
  dataX = {xMin, xMax};
  dataY = {fxi, fxi};
  hLine = Curve(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {xMin};
  dataY = {fxi + dy};
  elt = Text(dataX, dataY, {OSS() << "$L(\\hat{\\xi})$=" << fxi}, "right");
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
  elt = Text(dataX, dataY, {OSS() << "lb="<< ci.getLowerBound()[0]}, "right");
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

  // add xi vertical line
  dataX = {xi, xi};
  dataY = {bbox.getLowerBound()[1], bbox.getUpperBound()[1]};
  hLine = Curve(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  result.add(hLine);

  dataX = {xi + dx};
  dataY = {bbox.getLowerBound()[1]};
  elt = Text(dataX, dataY, {OSS() << "$\\hat{\\xi}$=" << ci.getUpperBound()[0]}, "right");
  elt.setColor("black");
  elt.setRotation(90.0);
  result.add(elt);
  result.setLegendPosition("topright");
  return result;
}

String ProfileLikelihoodResult::__repr__() const
{
  return OSS() << LikelihoodResult::__repr__()
    << " confidenceLevel_=" << confidenceLevel_;
}

Function ProfileLikelihoodResult::getProfileLikelihoodFunction() const
{
  return profileLikelihoodFunction_;
}

/* Method save() stores the object through the StorageManager */
void ProfileLikelihoodResult::save(Advocate & adv) const
{
  LikelihoodResult::save(adv);
  adv.saveAttribute("profileLikelihoodFunction_", profileLikelihoodFunction_);
  adv.saveAttribute("confidenceLevel_", confidenceLevel_);
}

/* Method load() reloads the object from the StorageManager */
void ProfileLikelihoodResult::load(Advocate & adv)
{
  LikelihoodResult::load(adv);
  adv.loadAttribute("profileLikelihoodFunction_", profileLikelihoodFunction_);
  adv.loadAttribute("confidenceLevel_", confidenceLevel_);
}

END_NAMESPACE_OPENTURNS
