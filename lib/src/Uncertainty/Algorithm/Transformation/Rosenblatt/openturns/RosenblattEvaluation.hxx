//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformationEvaluation evaluation for elliptical
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ROSENBLATTEVALUATION_HXX
#define OPENTURNS_ROSENBLATTEVALUATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RosenblattEvaluation
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API RosenblattEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  RosenblattEvaluation();

  /** Parameter constructor */
  explicit RosenblattEvaluation(const Distribution & distribution);

  /** Virtual constructor */
  RosenblattEvaluation * clone() const override;

  /** Evaluation */
  Point operator () (const Point & inP) const override;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  Distribution distribution_;

}; /* RosenblattEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ROSENBLATTEVALUATION_HXX */
