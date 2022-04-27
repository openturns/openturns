//                                               -*- C++ -*-
/**
 *  @brief A factory for building PenalizedLeastSquaresAlgorithm objects
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
#ifndef OPENTURNS_PENALIZEDLEASTSQUARESALGORITHMFACTORY_HXX
#define OPENTURNS_PENALIZEDLEASTSQUARESALGORITHMFACTORY_HXX

#include "openturns/Point.hxx"
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"
#include "openturns/ApproximationAlgorithmImplementationFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class PenalizedLeastSquaresAlgorithmFactory
 *
 * A factory for building PenalizedLeastSquaresAlgorithm objects
 */
class OT_API PenalizedLeastSquaresAlgorithmFactory
  : public ApproximationAlgorithmImplementationFactory
{
  CLASSNAME
public:
  typedef ApproximationAlgorithmImplementationFactory::FunctionCollection FunctionCollection;

  /** Default constructor */
  PenalizedLeastSquaresAlgorithmFactory(const Bool useNormal = ResourceMap::GetAsBool("PenalizedLeastSquaresAlgorithm-UseNormal"));

  /** Virtual constructor */
  PenalizedLeastSquaresAlgorithmFactory * clone() const override;

  /** Method to create new ApproximationAlgorithmImplementation objects */
  PenalizedLeastSquaresAlgorithm * build (const Sample & x,
                                          const Sample & y,
                                          const Point & weight,
                                          const FunctionCollection & psi,
                                          const Indices & indices) const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Bool useNormal_;
}; /* class PenalizedLeastSquaresAlgorithmFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDLEASTSQUARESALGORITHMFACTORY_HXX */
