//                                               -*- C++ -*-
/**
 *  @brief This Penalized Least Squares Algorithm as a functor class
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
#ifndef OPENTURNS_PENALIZEDLEASTSQUARESALGORITHM_HXX
#define OPENTURNS_PENALIZEDLEASTSQUARESALGORITHM_HXX

#include "openturns/ApproximationAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PenalizedLeastSquaresAlgorithm
 *
 * This PenalizedLeast Square Algorithm as a functor class
 */

class OT_API PenalizedLeastSquaresAlgorithm
  : public ApproximationAlgorithmImplementation
{
  CLASSNAME
public:

  typedef ApproximationAlgorithmImplementation::FunctionCollection FunctionCollection;

  /** Default constructor */
  explicit PenalizedLeastSquaresAlgorithm(const Bool useNormal = ResourceMap::GetAsBool("PenalizedLeastSquaresAlgorithm-UseNormal"));

  /** Parameters constructor, simple least squares problem with default parameters, spherically penalized for general parameters */
  PenalizedLeastSquaresAlgorithm(const Sample & x,
                                 const Sample & y,
                                 const FunctionCollection & psi,
                                 const Indices & indices,
                                 const Scalar penalizationFactor = 0.0,
                                 const Bool useNormal = false);

  /** Parameters constructor, simple weighted least squares problem with default parameters, spherically penalized for general parameters */
  PenalizedLeastSquaresAlgorithm(const Sample & x,
                                 const Sample & y,
                                 const Point & weight,
                                 const FunctionCollection & psi,
                                 const Indices & indices,
                                 const Scalar penalizationFactor = 0.0,
                                 const Bool useNormal = false);

  /** Parameters constructor, general penalized weighted least squares problem */
  PenalizedLeastSquaresAlgorithm(const Sample & x,
                                 const Sample & y,
                                 const Point & weight,
                                 const FunctionCollection & psi,
                                 const Indices & indices,
                                 const Scalar penalizationFactor,
                                 const CovarianceMatrix & penalizationMatrix,
                                 const Bool useNormal = false);

  /** Virtual constructor */
  PenalizedLeastSquaresAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Perform the optimization */
  void run() override;
#ifndef SWIG
  void run(const DesignProxy & proxy) override;
#endif

  /** Selection history accessor */
  Collection<Indices> getSelectionHistory(Collection<Point> & coefficientsHistory) const override;
  Point getErrorHistory() const override;

protected:

private:
  Scalar penalizationFactor_;
  CovarianceMatrix penalizationMatrix_;
  Bool useNormal_;

} ; /* class PenalizedLeastSquaresAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDLEASTSQUARESALGORITHM_HXX */
