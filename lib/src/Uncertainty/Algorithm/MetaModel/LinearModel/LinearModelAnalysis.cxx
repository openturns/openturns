//                                               -*- C++ -*-
/**
 *  @brief The linear model analysis
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
#include "openturns/LinearModelAnalysis.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NormalityTest.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Text.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/VisualTest.hxx"
#include "openturns/FisherSnedecor.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearModelAnalysis)

static const Factory<LinearModelAnalysis> Factory_LinearModelAnalysis;

/* Default constructor */
LinearModelAnalysis::LinearModelAnalysis()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameter constructor */
LinearModelAnalysis::LinearModelAnalysis(const LinearModelResult & linearModelResult)
  : PersistentObject()
  , linearModelResult_(linearModelResult)
{
  const SignedInteger dof = linearModelResult_.getDegreesOfFreedom();
  if (dof <= 0)
    throw InvalidArgumentException(HERE) << "Cannot perform linear model analysis when DOF is null";
}

/* Virtual constructor */
LinearModelAnalysis * LinearModelAnalysis::clone() const
{
  return new LinearModelAnalysis(*this);
}


/* String converter */
String LinearModelAnalysis::__repr__() const
{
  return OSS(true)
         << "class=" << getClassName()
         << ", linearModelResult=" << linearModelResult_;
}


/* Method that returns the ANOVA table (ANalyse Of VAriance) */
String LinearModelAnalysis::__str__(const String & offset) const
{
  const Point estimates(linearModelResult_.getCoefficients());
  const UnsignedInteger basisSize = estimates.getSize();
  const Bool hasIntercept = linearModelResult_.hasIntercept();
  const Point standardErrors(linearModelResult_.getCoefficientsStandardErrors());
  const Point tscores(getCoefficientsTScores());
  const Point pValues(getCoefficientsPValues());
  const Description names(linearModelResult_.getCoefficientsNames());
  const Scalar sigma2 = linearModelResult_.getSampleResiduals().computeRawMoment(2)[0];
  const SignedInteger dof = linearModelResult_.getDegreesOfFreedom();
  const UnsignedInteger n = linearModelResult_.getSampleResiduals().getSize();
  const String separator(" | ");
  const String separatorEndLine(" |");
  size_t twidth = 0; // column title max width
  size_t lwidth = 0; // LHS number max width
  size_t awidth = 0;
  String st;

  OSS oss(true);
  oss.setPrecision(5);
  for (UnsignedInteger i = 0; i < names.getSize(); ++i)
  {
    twidth = std::max( twidth, names[i].size() );
    st = OSS() << offset << estimates[i];
    lwidth = std::max( lwidth, st.size() );
    st = OSS() << offset << standardErrors[i];
    lwidth = std::max( lwidth, st.size() );
    st = OSS() << offset << tscores[i];
    lwidth = std::max( lwidth, st.size() );
    st = OSS() << offset << pValues[i];
    lwidth = std::max( lwidth, st.size() );
  }
  awidth = twidth + 5 * separator.size() + 4 * lwidth - 1;

  oss << offset << linearModelResult_.getFormula() ;
  oss << offset << "\n\nCoefficients:\n"  ;
  oss << offset << String( twidth, ' ' ) << separator;
  st = "Estimate";
  oss << offset << st << String( lwidth - st.size(), ' ') << separator;
  st = "Std Error";
  oss << offset << st << String( lwidth - st.size(), ' ') << separator;
  st = "t value";
  oss << offset << st << String( lwidth - st.size(), ' ') << separator;
  st = "Pr(>|t|)";
  oss << offset << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << offset << "\n" <<  String( awidth, '-' ) << "\n";
  for (UnsignedInteger i = 0; i < pValues.getSize(); ++i)
  {
    st = names[i];
    oss << offset << st << String( twidth - st.size(), ' ') << separator;
    st = OSS() << estimates[i];
    oss << st << String( lwidth - st.size(), ' ') << separator;
    st = OSS() << standardErrors[i];
    oss << st << String( lwidth - st.size(), ' ') << separator;
    st = OSS() << tscores[i];
    oss << st << String( lwidth - st.size(), ' ') << separator;
    st = OSS() << pValues[i];
    oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
    oss << "\n";
  }
  oss << offset << String( awidth, '-' ) << "\n\n";
  oss << offset << "Residual standard error: " <<  std::sqrt(sigma2 * n / dof)  << " on " << dof << " degrees of freedom\n";

  // In case of only intercept in the basis, no more print
  if ((basisSize == 1) && (hasIntercept))
    return oss;
  oss << offset << "F-statistic: " << getFisherScore() << " , " << " p-value: " <<  getFisherPValue() << "\n";

  //  R-squared & Adjusted R-squared tests
  lwidth = 0;
  twidth = 20;
  const Scalar test1 = linearModelResult_.getRSquared();
  const Scalar test2 = linearModelResult_.getAdjustedRSquared();
  st = OSS() << test1;
  lwidth = std::max( lwidth, st.size() );
  st = OSS() << test2;
  lwidth = std::max( lwidth, st.size() );
  awidth = twidth + 2 * separator.size() + lwidth - 1;
  oss << offset << String( awidth, '-' ) << "\n";
  st = "Multiple R-squared";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = OSS() << test1;
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n";
  st = "Adjusted R-squared";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = OSS() << test2;
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n" << offset << String( awidth, '-' ) << "\n";

  // normality tests
  lwidth = 7; // width of "p-value"
  twidth = 20;
  const Scalar normalitytest1(getNormalityTestResultAndersonDarling().getPValue());
  const Scalar normalitytest2(getNormalityTestResultChiSquared().getPValue());
  const Scalar normalitytest3(getNormalityTestResultKolmogorovSmirnov().getPValue());
  const Scalar normalitytest4(getNormalityTestCramerVonMises().getPValue());
  st = OSS() << normalitytest1;
  lwidth = std::max( lwidth, st.size() );
  st = OSS() << normalitytest2;
  lwidth = std::max( lwidth, st.size() );
  st = OSS() << normalitytest3;
  lwidth = std::max( lwidth, st.size() );
  st = OSS() << normalitytest4;
  lwidth = std::max( lwidth, st.size() );
  awidth = twidth + 2 * separator.size() + lwidth - 1;
  oss << "\n" << offset << String( awidth, '-' ) << "\n";
  st = "Normality test";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = "p-value";
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n" <<  String( awidth, '-' ) << "\n";
  st = "Anderson-Darling";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = OSS() << normalitytest1;
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n";
  st = "Cramer-Von Mises";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = OSS() << normalitytest4;
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n";
  st = "Chi-Squared";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = OSS() << normalitytest2;
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n";
  st = "Kolmogorov-Smirnov";
  oss << offset << st << String( twidth - st.size(), ' ') << separator;
  st = OSS() << normalitytest3;
  oss << st << String( lwidth - st.size(), ' ') << separatorEndLine;
  oss << "\n" <<  String( awidth, '-' ) << "\n";
  return oss;
}


/* Linear model accessor */
LinearModelResult LinearModelAnalysis::getLinearModelResult() const
{
  return linearModelResult_;
}

Point LinearModelAnalysis::getCoefficientsTScores() const
{
  // The coefficients of linear expansion over their standard error
  const Point estimates(linearModelResult_.getCoefficients());
  const Point standardErrors(linearModelResult_.getCoefficientsStandardErrors());
  Point tScores(estimates.getSize(), 1);
  for (UnsignedInteger i = 0; i < tScores.getSize(); ++i)
  {
    tScores[i] = estimates[i] / standardErrors[i];
  }
  return tScores;
}

Point LinearModelAnalysis::getCoefficientsPValues() const
{
  // Interest is Pr(X > |t|) with t the statistic defined as
  // t: = beta / std_dev(beta)
  // t ~ Student(dof)
  // Pr(X > |t|) = Pr(X > sign(t)*t) + Pr(X < -sign(t)*t)
  //             = 2 * Pr(X > sign(t)*t) as Student distribution is symmetric
  //             = 2 * Pr(X > |t|)
  const Point tScores(getCoefficientsTScores());
  const UnsignedInteger dof = linearModelResult_.getDegreesOfFreedom();
  Point pValues(tScores.getSize());
  for (UnsignedInteger i = 0; i < pValues.getSize(); ++i)
  {
    // true flag define the complementary CDF, ie P(X > t)
    pValues[i] = 2.0 * DistFunc::pStudent(dof, std::abs(tScores[i]), true);
  }
  return pValues;
}

Interval LinearModelAnalysis::getCoefficientsConfidenceInterval(const Scalar level) const
{
  const Point coefficientsErrors(linearModelResult_.getCoefficientsStandardErrors());
  const Point beta(linearModelResult_.getCoefficients());
  const Scalar sigmaConfInt = DistFunc::qStudent(linearModelResult_.getDegreesOfFreedom(), (1.0 - level) * 0.5, true);
  const Interval bounds(beta - coefficientsErrors * sigmaConfInt, beta + coefficientsErrors * sigmaConfInt);
  return bounds;
}


/* Fisher test */
Scalar LinearModelAnalysis::getFisherScore() const
{
  // Get residuals and output samples
  const Sample residuals(linearModelResult_.getSampleResiduals());
  const Sample outputSample(getLinearModelResult().getOutputSample());
  const UnsignedInteger size = residuals.getSize();
  // Get the number of parameter p
  const UnsignedInteger p = linearModelResult_.getCoefficients().getSize();
  // Check if there is an intercept
  const Bool hasIntercept = linearModelResult_.hasIntercept();
  // Degrees of freedom (model)
  UnsignedInteger dofModel = p;

  // Correction of dofModel if intercept
  if ((hasIntercept) && (p == 1))
    throw NotDefinedException(HERE) << "Only intercept in the basis. Fisher Test is not defined is such a case.";
  // Correction of dofModel if intercept
  if (hasIntercept)
    dofModel -= 1;
  // Degrees of freedom (noise)
  const SignedInteger dof = linearModelResult_.getDegreesOfFreedom();
  // Sum of Squared Errors (SSE) or Sum of Squared Residuals (SSR/RSS)
  const Scalar SSR = residuals.computeRawMoment(2)[0] * size;
  // Sum of Squared Total (SST) = n * var(Y) or n * E(Y^2) depending on intercept
  Scalar SST = 1.0;
  if (hasIntercept)
    SST = outputSample.computeCenteredMoment(2)[0] * size;
  else
    SST = outputSample.computeRawMoment(2)[0] * size;
  // Sum of Squared Model (SSM) = SST - SSE
  const Scalar SSM = SST - SSR;
  // Define the statistic
  // numerator = MSM := SSM/DFM
  const Scalar numerator = SSM / dofModel;
  // denominator =  MSE = SSE/DO
  const Scalar denominator = SSR / dof;
  const Scalar fStatistic = numerator / denominator;
  return fStatistic;
}

Scalar LinearModelAnalysis::getFisherPValue() const
{
  // Get the number of parameter p
  const UnsignedInteger p = linearModelResult_.getCoefficients().getSize();
  // Degrees of freedom (model)
  UnsignedInteger dofModel = p;
  const Bool hasIntercept = linearModelResult_.hasIntercept();
  // Correction of dofModel if intercept
  if ((hasIntercept) && (p == 1))
    throw NotDefinedException(HERE) << "Only intercept in the basis. Fisher Test is not defined is such a case.";
  if (hasIntercept)
    dofModel -= 1;
  // Degrees of freedom
  const SignedInteger dof = linearModelResult_.getDegreesOfFreedom();
  const Scalar FStatistic = getFisherScore();
  return FisherSnedecor(dofModel, dof).computeComplementaryCDF(FStatistic);
}

/* Kolmogorov-Smirnov normality test */
TestResult LinearModelAnalysis::getNormalityTestResultKolmogorovSmirnov() const
{
  // We check that residuals are centered with variance = sigma2
  const Sample residuals(linearModelResult_.getSampleResiduals());
  // Compute Sigma2
  const Scalar sigma2(residuals.computeRawMoment(2)[0]);
  const Normal dist(0.0, std::sqrt(sigma2));
  return FittingTest::Kolmogorov(residuals, dist);
}

/* Anderson-Darling normality test */
TestResult LinearModelAnalysis::getNormalityTestResultAndersonDarling() const
{
  return NormalityTest::AndersonDarlingNormal(linearModelResult_.getSampleResiduals());
}

/* Chi-Squared normality test */
TestResult LinearModelAnalysis::getNormalityTestResultChiSquared() const
{
  // Using OT::FittingTest::ChiSquared
  const Sample residuals(linearModelResult_.getSampleResiduals());
  const Normal normalDistribution(residuals.computeMean()[0], residuals.computeStandardDeviation()[0]);
  return FittingTest::ChiSquared(residuals, normalDistribution);
}

/* Cramer Von Mises normality test */
TestResult LinearModelAnalysis::getNormalityTestCramerVonMises() const
{
  return NormalityTest::CramerVonMisesNormal(linearModelResult_.getSampleResiduals());
}

/* [0] Draw model versus fitted values */
Graph LinearModelAnalysis::drawModelVsFitted() const
{
  const Sample inputData(linearModelResult_.getInputSample());
  const Function metamodel(linearModelResult_.getMetaModel());
  const Sample fitted(metamodel(inputData));
  const Sample outputData(linearModelResult_.getOutputSample());
  const Sample residuals(linearModelResult_.getSampleResiduals());

  const UnsignedInteger size(fitted.getSize());

  Sample dataFull(outputData);
  dataFull.stack(fitted);

  // The graph object
  Graph graph("Model vs Fitted", "Model", "Fitted values", true, "topright");

  // Bissectrice graph
  Sample bissectriceCurve(2, 2);
  bissectriceCurve(0, 0) = outputData.getMin()[0];
  bissectriceCurve(0, 1) = outputData.getMin()[0];

  bissectriceCurve(1, 0) = outputData.getMax()[0];
  bissectriceCurve(1, 1) = outputData.getMax()[0];

  Curve curve(bissectriceCurve, "red", "solid");
  graph.add(curve);

  // model vs Fitted values
  Cloud cloud(dataFull, "black", "fcircle");
  graph.add(cloud);
  // Add point identifiers for worst residuals
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  if (identifiers > 0)
  {
    if (identifiers > size)
      identifiers = size;
    Description annotations(size);
    Sample dataWithIndex(size, 2);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex(i, 0) = std::abs(residuals(i, 0));
      dataWithIndex(i, 1) = i;
    }
    const Sample sortedData(dataWithIndex.sortAccordingToAComponent(0));
    Description positions(size, "top");
    for(UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index = sortedData(size - 1 - i, 1);
      annotations[index] = (OSS() << index + 1);
      if (residuals(index, 0) < 0.0)
        positions[index] = "top";
      else
        positions[index] = "bottom";
    }
    Text text(dataFull, annotations, "bottom");
    text.setColor("red");
    text.setTextPositions(positions);
    graph.add(text);
  }
  return graph;
}

/* [1] Draw a plot of residuals versus fitted values */
Graph LinearModelAnalysis::drawResidualsVsFitted() const
{
  const Sample inputData(linearModelResult_.getInputSample());
  const Function metamodel(linearModelResult_.getMetaModel());
  const Sample fitted(metamodel(inputData));
  const Sample residuals(linearModelResult_.getSampleResiduals());
  const UnsignedInteger size(fitted.getSize());
  Sample dataFull(fitted);
  dataFull.stack(residuals);
  Graph graph("Residuals vs Fitted", "Fitted values", "Residuals", true, "topright");
  Cloud cloud(dataFull, "black", "fcircle");
  graph.add(cloud);
  // Add point identifiers for worst residuals
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  if (identifiers > 0)
  {
    if (identifiers > size)
      identifiers = size;
    Description annotations(size);
    Sample dataWithIndex(size, 2);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex(i, 0) = std::abs(residuals(i, 0));
      dataWithIndex(i, 1) = i;
    }
    const Sample sortedData(dataWithIndex.sortAccordingToAComponent(0));
    Description positions(size, "top");
    for(UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index = sortedData(size - 1 - i, 1);
      annotations[index] = (OSS() << index + 1);
      if (residuals(index, 0) < 0.0)
        positions[index] = "top";
      else
        positions[index] = "bottom";
    }
    Text text(dataFull, annotations, "bottom");
    text.setColor("red");
    text.setTextPositions(positions);
    graph.add(text);
  }
  return graph;
}

/* [2] a Scale-Location plot of sqrt(| residuals |) versus fitted values */
Graph LinearModelAnalysis::drawScaleLocation() const
{
  const Sample fitted(linearModelResult_.getFittedSample());
  const Sample stdResiduals(linearModelResult_.getStandardizedResiduals());
  const UnsignedInteger size(fitted.getSize());
  Sample dataFull(fitted);
  Sample sqrtStdResiduals(size, 1);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    sqrtStdResiduals(i, 0) = std::sqrt(std::abs(stdResiduals(i, 0)));
  }
  dataFull.stack(sqrtStdResiduals);
  Graph graph("Scale-Location", "Fitted values", "|Std. residuals|^0.5", true, "topright");
  Cloud cloud(dataFull, "black", "fcircle");
  graph.add(cloud);
  // Add point identifiers for worst standardized residuals
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  if (identifiers > 0)
  {
    if (identifiers > size)
      identifiers = size;
    Description annotations(size);
    Sample dataWithIndex(size, 2);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex(i, 0) = std::abs(stdResiduals(i, 0));
      dataWithIndex(i, 1) = i;
    }
    const Sample sortedData(dataWithIndex.sortAccordingToAComponent(0));
    Description positions(size, "top");
    for(UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index = sortedData(size - 1 - i, 1);
      annotations[index] = (OSS() << index + 1);
      if (stdResiduals(index, 0) < 0.0)
        positions[index] = "top";
      else
        positions[index] = "bottom";
    }
    Text text(dataFull, annotations, "bottom");
    text.setColor("red");
    text.setTextPositions(positions);
    graph.add(text);
  }
  return graph;
}

/* [3] a Normal quantiles-quantiles plot of standardized residuals */
Graph LinearModelAnalysis::drawQQplot() const
{
  const Sample stdResiduals(linearModelResult_.getStandardizedResiduals());
  Graph graph(VisualTest::DrawQQplot(stdResiduals, Normal(0.0, 1.0)));
  graph.setTitle("Normal Q-Q");
  graph.setXTitle("Std. residuals");
  graph.setYTitle("Theoretical Quantiles");

  // Add point identifiers for worst standardized residuals
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  if (identifiers > 0)
  {
    const UnsignedInteger size = stdResiduals.getSize();
    if (identifiers > size)
      identifiers = size;
    const Sample sortedSample(stdResiduals.sort(0));

    const Sample dataFull(graph.getDrawable(1).getData());
    Description annotations(size);
    Sample dataWithIndex1(size, 2);
    Sample dataWithIndex2(size, 2);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex1(i, 0) = std::abs(sortedSample(i, 0));
      dataWithIndex1(i, 1) = i;
      dataWithIndex2(i, 0) = std::abs(stdResiduals(i, 0));
      dataWithIndex2(i, 1) = i;
    }
    const Sample sortedData1(dataWithIndex1.sortAccordingToAComponent(0));
    const Sample sortedData2(dataWithIndex2.sortAccordingToAComponent(0));
    Description positions(size, "top");
    for (UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index1 = sortedData1(size - 1 - i, 1);
      const UnsignedInteger index2 = sortedData2(size - 1 - i, 1);
      annotations[index1] = (OSS() << index2 + 1);
      positions[index1] = dataFull(index1, 0) < 0.0 ? "top" : "bottom";
    }
    Text text(dataFull, annotations, "bottom");
    text.setColor("red");
    text.setTextPositions(positions);
    graph.add(text);
  }
  // Disable legend
  graph.setLegendPosition("");
  return graph;
}

/* [4] a plot of Cook's distances versus row labels */
Graph LinearModelAnalysis::drawCookDistance() const
{
  const Point cookdistances(linearModelResult_.getCookDistances());
  const UnsignedInteger size(cookdistances.getSize());
  // Add point identifiers for worst Cook's distance
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  Description annotations(size);
  if (identifiers > 0)
  {
    if (identifiers > size)
      identifiers = size;
    Sample dataWithIndex(size, 2);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex(i, 0) = cookdistances[i];
      dataWithIndex(i, 1) = i;
    }
    const Sample sortedData(dataWithIndex.sortAccordingToAComponent(0));
    for(UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index = sortedData(size - 1 - i, 1);
      annotations[index] = (OSS() << index + 1);
    }
  }
  Graph graph("Cook's distance", "Obs. number", "Cook's distance", true, "topright");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Sample dataFull(2, 2);
    dataFull(0, 0) = i + 1;
    dataFull(0, 1) = 0.0;
    dataFull(1, 0) = i + 1;
    dataFull(1, 1) = cookdistances[i];
    Curve curve(dataFull, "black", "solid", 2);
    graph.add(curve);
    if (annotations[i] != "")
    {
      Description desc(2);
      desc[1] = annotations[i];
      Text text(dataFull, desc, "top");
      text.setColor("red");
      graph.add(text);
    }
  }
  return graph;
}

/* [5] a plot of residuals versus leverages that adds bands corresponding to Cook's distances of 0.5 and 1. */
Graph LinearModelAnalysis::drawResidualsVsLeverages() const
{
  const Point cookdistances(linearModelResult_.getCookDistances());
  const Point leverages(linearModelResult_.getLeverages());
  const Sample stdResiduals(linearModelResult_.getStandardizedResiduals());
  const UnsignedInteger size(stdResiduals.getSize());
  Sample leveragesS(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    leveragesS(i, 0) = leverages[i];
  }
  Sample dataFull(leveragesS);
  dataFull.stack(stdResiduals);
  Graph graph("Residuals vs Leverage", "Leverage", "Std. residuals", true, "topright");
  Cloud cloud(dataFull, "black", "fcircle");
  graph.add(cloud);
  // Add point identifiers for worst Cook's distance
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  if (identifiers > 0)
  {
    if (identifiers > size)
      identifiers = size;
    Description annotations(size);
    Sample dataWithIndex(size, 2);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex(i, 0) = cookdistances[i];
      dataWithIndex(i, 1) = i;
    }
    const Sample sortedData(dataWithIndex.sortAccordingToAComponent(0));
    Description positions(size, "top");
    for(UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index = sortedData(size - 1 - i, 1);
      annotations[index] = (OSS() << index + 1);
      positions[index] = (cookdistances[index] < 0.0 ? "top" : "bottom");
    }
    Text text(dataFull, annotations, "bottom");
    text.setColor("red");
    text.setTextPositions(positions);
    graph.add(text);
  }
  const Interval boundingBox(graph.getBoundingBox());
  const Point lowerBound(boundingBox.getLowerBound());
  const Point upperBound(boundingBox.getUpperBound());
  const Scalar width = upperBound[0] - lowerBound[0];
  // Add contour plot of Cook's distance
  const UnsignedInteger pPlusOne = linearModelResult_.getCoefficientsNames().getSize();
  const UnsignedInteger step = 100;
  Point isovalues(2);
  isovalues[0] = 0.5;
  isovalues[1] = 1.0;
  Scalar ptx;
  Description annotation(2);
  Sample diagonal1(2, 2);
  Sample diagonal2(2, 2);
  for(UnsignedInteger k = 0; k < isovalues.getSize(); ++k)
  {
    for(UnsignedInteger i = 0; i < step - 1; ++i)
    {
      ptx = lowerBound[0] + i * (width) / step;
      diagonal1(0, 0) = ptx;
      diagonal2(0, 0) = ptx;
      diagonal1(0, 1) =  std::sqrt(std::abs(isovalues[k] * pPlusOne * (1.0 - ptx) / ptx));
      diagonal2(0, 1) = -diagonal1(0, 1);
      ptx = lowerBound[0] + (i + 1) * (width) / step;
      diagonal1(1, 0) = ptx;
      diagonal2(1, 0) = ptx;
      diagonal1(1, 1) =  std::sqrt(std::abs(isovalues[k] * pPlusOne * (1.0 - ptx) / ptx));
      diagonal2(1, 1) = -diagonal1(1, 1);
      Curve curve1(diagonal1, "red", "solid", 1);
      Curve curve2(diagonal2, "red", "solid", 1);
      graph.add(curve1);
      graph.add(curve2);
    }
    annotation[0] = (OSS() << isovalues[k]);
    Cloud cloud1(diagonal1, "red", "dot");
    graph.add(cloud1);
    Text text1(diagonal1, annotation, "top");
    text1.setColor("red");
    graph.add(text1);
    Cloud cloud2(diagonal2, "red", "dot");
    graph.add(cloud2);
    Text text2(diagonal2, annotation, "bottom");
    text2.setColor("red");
    graph.add(text2);
  }
  Curve curve1(diagonal1, "red", "solid", 1);
  curve1.setLegend("Cook's distance");
  graph.add(curve1);
  return graph;
}

/* [6] a plot of Cook's distances versus leverage/(1-leverage) */
Graph LinearModelAnalysis::drawCookVsLeverages() const
{
  const Point leverages(linearModelResult_.getLeverages());
  const Point cookdistances(linearModelResult_.getCookDistances());
  const UnsignedInteger size(cookdistances.getSize());
  Sample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull(i, 0) = leverages[i] / (1.0 - leverages[i]);
    dataFull(i, 1) = cookdistances[i];
  }
  Graph graph("Cook's dist vs Leverage h[ii]/(1-h[ii])", "Leverage h[ii]/(1-h[ii])", "Cook's distance", true, "topright");
  Cloud cloud(dataFull, "black", "fcircle");
  graph.add(cloud);
  // Add point identifiers for worst Cook's distance
  UnsignedInteger identifiers(ResourceMap::GetAsUnsignedInteger("LinearModelAnalysis-Identifiers"));
  if (identifiers > 0)
  {
    if (identifiers > size)
      identifiers = size;
    Description annotations(size);
    Sample dataWithIndex(size, 2);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      dataWithIndex(i, 0) = std::abs(cookdistances[i]);
      dataWithIndex(i, 1) = i;
    }
    const Sample sortedData(dataWithIndex.sortAccordingToAComponent(0));
    Description positions(size, "top");
    for(UnsignedInteger i = 0; i < identifiers; ++i)
    {
      const UnsignedInteger index = sortedData(size - 1 - i, 1);
      annotations[index] = (OSS() << index + 1);
      positions[index] = (cookdistances[index] < 0.0 ? "top" : "bottom");

    }
    Text text(dataFull, annotations, "bottom");
    text.setColor("red");
    text.setTextPositions(positions);
    graph.add(text);
  }
  const Interval boundingBox(graph.getBoundingBox());
  const Point lowerBound(boundingBox.getLowerBound());
  const Point upperBound(boundingBox.getUpperBound());
  // Add contour plot
  Point isovalues(6);
  isovalues[0] = 0.5;
  isovalues[1] = 1.0;
  isovalues[2] = 1.5;
  isovalues[3] = 2.0;
  isovalues[4] = 2.5;
  isovalues[5] = 3.0;
  Point Pt(2);
  Description annotation(2);
  Sample diagonal(2, 2);
  diagonal(0, 0) = 0.0;
  diagonal(0, 1) = 0.0;
  for(UnsignedInteger k = 0; k < isovalues.getSize(); ++k)
  {
    Scalar coeff = isovalues[k] * isovalues[k];
    Pt[0] = upperBound[1] / coeff;
    Pt[1] = upperBound[0] * coeff;
    if (Pt[1] < upperBound[1])
    {
      diagonal(1, 0) = upperBound[0];
      diagonal(1, 1) = Pt[1];
    }
    if (Pt[0] < upperBound[0])
    {
      diagonal(1, 0) = Pt[0];
      diagonal(1, 1) = upperBound[1];
    }
    Curve curve(diagonal, "red", "solid", 1);
    graph.add(curve);
    annotation[1] = (OSS() << isovalues[k]);
    Cloud diagonalCloud(diagonal, "red", "dot");
    graph.add(diagonalCloud);
    Text text(diagonal, annotation, "top");
    text.setColor("red");
    graph.add(text);
  }
  return graph;
}

/* Method save() stores the object through the StorageManager */
void LinearModelAnalysis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "linearModelResult_", linearModelResult_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearModelAnalysis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "linearModelResult_", linearModelResult_ );
}

END_NAMESPACE_OPENTURNS
