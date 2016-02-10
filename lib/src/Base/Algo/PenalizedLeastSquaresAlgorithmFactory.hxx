//                                               -*- C++ -*-
/**
 *  @brief A factory for building PenalizedLeastSquaresAlgorithm objects
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
#ifndef OPENTURNS_PENALIZEDLEASTSQUARESALGORITHMFACTORY_HXX
#define OPENTURNS_PENALIZEDLEASTSQUARESALGORITHMFACTORY_HXX

#include "NumericalPoint.hxx"
#include "PenalizedLeastSquaresAlgorithm.hxx"
#include "ApproximationAlgorithmImplementationFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class PenalizedLeastSquaresAlgorithmFactory
 *
 * A factory for building PenalizedLeastSquaresAlgorithm objects
 */
class OT_API PenalizedLeastSquaresAlgorithmFactory
  : public ApproximationAlgorithmImplementationFactory
{
  CLASSNAME;
public:

  /** Default constructor */
  PenalizedLeastSquaresAlgorithmFactory(const Bool useNormal = false);

  /** Virtual constructor */
  virtual PenalizedLeastSquaresAlgorithmFactory * clone() const;

  /** Method to create new ApproximationAlgorithmImplementation objects */
  virtual PenalizedLeastSquaresAlgorithm * build (const NumericalSample & x,
      const NumericalSample & y,
      const NumericalPoint & weight,
      const Basis & psi,
      const Indices & indices) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  Bool useNormal_;
}; /* class PenalizedLeastSquaresAlgorithmFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDLEASTSQUARESALGORITHMFACTORY_HXX */
