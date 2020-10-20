//                                               -*- C++ -*-
/**
 *  @brief A factory for building LeastSquaresMetaModelSelection objects
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
#ifndef OPENTURNS_LEASTSQUARESMETAMODELSELECTIONFACTORY_HXX
#define OPENTURNS_LEASTSQUARESMETAMODELSELECTIONFACTORY_HXX

#include "openturns/Sample.hxx"
#include "openturns/ApproximationAlgorithmImplementationFactory.hxx"
#include "openturns/LeastSquaresMetaModelSelection.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LeastSquaresMetaModelSelectionFactory
 *
 * A factory for building LeastSquaresMetaModelSelection objects
 */
class OT_API LeastSquaresMetaModelSelectionFactory
  : public ApproximationAlgorithmImplementationFactory
{
  CLASSNAME
public:
  typedef ApproximationAlgorithmImplementationFactory::FunctionCollection FunctionCollection;

  /** Constructor */
  explicit LeastSquaresMetaModelSelectionFactory(const BasisSequenceFactory & fact = LARS(),
      const FittingAlgorithm & algo = CorrectedLeaveOneOut());

  /** Virtual constructor */
  LeastSquaresMetaModelSelectionFactory * clone() const override;

  /** Accessors */
  BasisSequenceFactory getBasisSequenceFactory() const;
  FittingAlgorithm getFittingAlgorithm() const;

  /** Method to create new LeastSquaresMetaModelSelection objects */
  LeastSquaresMetaModelSelection * build (const Sample & x,
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
  /** The algorithm that generates a family of basis */
  BasisSequenceFactory basisSequenceFactory_;

  /** The cross-validation algorithm */
  FittingAlgorithm fittingAlgorithm_;


}; /* class LeastSquaresMetaModelSelectionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESMETAMODELSELECTIONFACTORY_HXX */
