//                                               -*- C++ -*-
/**
 *  @brief Validation of GEV inference
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GeneralizedExtremeValueValidation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/VisualTest.hxx"
#include "openturns/GeneralizedExtremeValue.hxx"
#include "openturns/GeneralizedExtremeValueFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExtremeValueValidation)

static const Factory<GeneralizedExtremeValueValidation> Factory_GeneralizedExtremeValueValidation;


GeneralizedExtremeValueValidation::GeneralizedExtremeValueValidation()
  : PersistentObject()
{}

GeneralizedExtremeValueValidation::GeneralizedExtremeValueValidation(const DistributionFactoryResult & result,
    const Sample & sample)
  : PersistentObject()
  , sample_(sample)
  , result_(result)
{
  const String distName(result.getDistribution().getImplementation()->getClassName());
  if (distName != "GeneralizedExtremeValue")
    throw InvalidArgumentException(HERE) << "Expected a GEV distribution, got " << distName;
}

GeneralizedExtremeValueValidation * GeneralizedExtremeValueValidation::clone() const
{
  return new GeneralizedExtremeValueValidation(*this);
}

Graph GeneralizedExtremeValueValidation::drawReturnLevel() const
{
  Graph rlPlot(dynamic_cast<GeneralizedExtremeValue*>(result_.getDistribution().getImplementation().get())->drawReturnLevel());
  const Sample rlData(rlPlot.getDrawable(0).getData().getMarginal(0));
  const UnsignedInteger size = rlData.getSize();
  Sample rlCILO(size, 1);
  Sample rlCIUP(size, 1);
  Sample rlEmp(size, 1);
  const GeneralizedExtremeValueFactory factory;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Scalar mi = rlData(i, 0);
    const Distribution zDistribution(factory.buildReturnLevelEstimator(result_, mi));
    const Interval zCI(zDistribution.computeBilateralConfidenceInterval(confidenceLevel_));
    rlCILO(i, 0) = zCI.getLowerBound()[0];
    rlCIUP(i, 0) = zCI.getUpperBound()[0];
    rlEmp(i, 0) = sample_.computeQuantile(1.0 - 1.0 / mi)[0];
  }
  Curve curveLO(rlData, rlCILO);
  Curve curveUP(rlData, rlCIUP);
  curveLO.setLineStyle("dashed");
  curveUP.setLineStyle("dashed");
  Cloud cloudRl(rlData, rlEmp);
  cloudRl.setColor("green");
  rlPlot.add(curveLO);
  rlPlot.add(curveUP);
  rlPlot.add(cloudRl);
  rlPlot.setLegends({"model", "CI low", "CI up", "data"});
  rlPlot.setTitle("Return level plot");
  return rlPlot;
}

Graph GeneralizedExtremeValueValidation::drawPDF() const
{
  Graph graph(result_.getDistribution().drawPDF());
  graph.setLegends({"estimated PDF"});
  Graph histogram(HistogramFactory().build(sample_).drawPDF());
  histogram.setLegends({"data"});
  histogram.setColors({"blue"});
  graph.add(histogram);
  graph.setXTitle(sample_.getDescription()[0]);
  graph.setTitle("Density");
  return graph;
}

GridLayout GeneralizedExtremeValueValidation::drawDiagnosticPlot() const
{
  GridLayout grid(2, 2);

  // pp/qq plots
  Graph ppPlot(VisualTest::DrawPPplot(sample_, result_.getDistribution()));
  ppPlot.setYTitle("model probability");
  ppPlot.setXTitle("sample probability");
  Graph qqPlot(VisualTest::DrawQQplot(sample_, result_.getDistribution()));
  qqPlot.setYTitle("model quantile");
  qqPlot.setXTitle("sample quantile");
  grid.setGraph(0, 0, ppPlot);
  grid.setGraph(0, 1, qqPlot);

  // return level / density
  grid.setGraph(1, 0, drawReturnLevel());
  grid.setGraph(1, 1, drawPDF());
  return grid;
}

void GeneralizedExtremeValueValidation::setConfidenceLevel(const Scalar confidenceLevel)
{
  confidenceLevel_ = confidenceLevel;
}

Scalar GeneralizedExtremeValueValidation::getConfidenceLevel() const
{
  return confidenceLevel_;
}

String GeneralizedExtremeValueValidation::__repr__() const
{
  return OSS() << PersistentObject::__repr__();
}

/* Method save() stores the object through the StorageManager */
void GeneralizedExtremeValueValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("sample_", sample_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void GeneralizedExtremeValueValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("sample_", sample_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
