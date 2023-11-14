//                                               -*- C++ -*-
/**
 *  @brief This class allows one to compute integrals of a function over a mesh.
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
#ifndef OPENTURNS_SIMPLICIALCUBATURE_HXX
#define OPENTURNS_SIMPLICIALCUBATURE_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimplicialCubature
 */

class OT_API SimplicialCubature
  : public PersistentObject
{

  CLASSNAME

public:

  /** Default constructor */
  SimplicialCubature();

  /** Virtual copy constructor */
  SimplicialCubature * clone() const override;

  /** Compute an approximation of \int_D f(x_1,\dots,x_n)dx_1\dotsdx_n, where D is a mesh.
   */
  Point integrate(const Function & function, const Mesh & mesh) const;

  // Integration rule accessor
  void setRule(const UnsignedInteger rule);
  UnsignedInteger getRule() const;


  /** Maximum absolute error accessor */
  void setMaximumAbsoluteError(const Scalar maximumAbsoluteError);
  Scalar getMaximumAbsoluteError() const;

  /** Maximum relative error accessor */
  void setMaximumRelativeError(const Scalar maximumRelativeError);
  Scalar getMaximumRelativeError() const;

  /** Maximum evaluations number accessor */
  void setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber);
  UnsignedInteger getMaximumEvaluationNumber() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
private:
  UnsignedInteger getNodeNumber(const UnsignedInteger dimension) const;

  Point computePermutationSums(const UnsignedInteger dimension, const Sample & simplexVertices,
                               const UnsignedInteger outputDimension, const Function & function,
                               const Point & G) const;

  Indices initializeBasicRule(const UnsignedInteger dimension, Matrix & W, Matrix & G) const;

  void computeRuleValueAndError(const UnsignedInteger dimension, const Sample & simplexVertices, const Scalar volume,
                                const UnsignedInteger outputDimension, const Function & function, const Matrix & G,
                                const Matrix & W, const Indices & evalBudget, Point & value, Point & error) const;

  UnsignedInteger computeNewSubregions(const UnsignedInteger dimension, const UnsignedInteger outputDimension,
                                       const Function & function, const UnsignedInteger bestSimplex,
                                       const UnsignedInteger flatSize, Collection<Sample> & flatVertices) const;

  // integration rule
  UnsignedInteger rule_ = 0;

  // stop criterion
  Scalar maximumAbsoluteError_ = 0.0;
  Scalar maximumRelativeError_ = 0.0;
  UnsignedInteger maximumEvaluationNumber_ = 0;

} ; /* class SimplicialCubature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMPLICIALCUBATURE_HXX */
