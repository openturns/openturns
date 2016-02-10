//                                               -*- C++ -*-
/**
 *  @brief This Penalized Least Squares Algorithm as a functor class
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_PENALIZEDLEASTSQUARESALGORITHM_HXX
#define OPENTURNS_PENALIZEDLEASTSQUARESALGORITHM_HXX

#include "ApproximationAlgorithmImplementation.hxx"
#include "NumericalSample.hxx"
#include "CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PenalizedLeastSquaresAlgorithm
 *
 * This PenalizedLeast Square Algorithm as a functor class
 */

class OT_API PenalizedLeastSquaresAlgorithm
  : public ApproximationAlgorithmImplementation
{
  CLASSNAME;
public:

  typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;

  // friend class Factory<PenalizedLeastSquaresAlgorithm>;

  /** Default constructor */
  PenalizedLeastSquaresAlgorithm(const Bool useNormal = true);

  /** Parameters constructor, simple least squares problem with default parameters, spherically penalized for general parameters */
  PenalizedLeastSquaresAlgorithm(const NumericalSample & x,
                                 const NumericalSample & y,
                                 const Basis & psi,
                                 const Indices & indices,
                                 const NumericalScalar penalizationFactor = 0.0,
                                 const Bool useNormal = false);

  /** Parameters constructor, simple weighted least squares problem with default parameters, spherically penalized for general parameters */
  PenalizedLeastSquaresAlgorithm(const NumericalSample & x,
                                 const NumericalSample & y,
                                 const NumericalPoint & weight,
                                 const Basis & psi,
                                 const Indices & indices,
                                 const NumericalScalar penalizationFactor = 0.0,
                                 const Bool useNormal = false);

  /** Parameters constructor, general penalized weighted least squares problem */
  PenalizedLeastSquaresAlgorithm(const NumericalSample & x,
                                 const NumericalSample & y,
                                 const NumericalPoint & weight,
                                 const Basis & psi,
                                 const Indices & indices,
                                 const NumericalScalar penalizationFactor,
                                 const CovarianceMatrix & penalizationMatrix,
                                 const Bool useNormal = false);

  /** Virtual constructor */
  virtual PenalizedLeastSquaresAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Perform the optimization */
  void run();
#ifndef SWIG
  void run(const DesignProxy & proxy);
#endif

protected:

private:
  NumericalScalar penalizationFactor_;
  CovarianceMatrix penalizationMatrix_;
  Bool useNormal_;

} ; /* class PenalizedLeastSquaresAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDLEASTSQUARESALGORITHM_HXX */
