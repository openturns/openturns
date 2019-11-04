//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm implementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FITTINGALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_FITTINGALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/Collection.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/DesignProxy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FittingAlgorithmImplementation
 *
 * Cross-validation algorithm implementation
 */
class OT_API FittingAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  FittingAlgorithmImplementation();

  /** Virtual constructor */
  virtual FittingAlgorithmImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Perform cross-validation */
  virtual Scalar run(const Sample & x,
                     const Sample & y,
                     const Point & weight,
                     const FunctionCollection & psi,
                     const Indices & indices) const;

  virtual Scalar run(const Sample & x,
                     const Sample & y,
                     const FunctionCollection & psi,
                     const Indices & indices) const;

#ifndef SWIG
  virtual Scalar run(const Sample & y,
                     const Point & weight,
                     const Indices & indices,
                     const DesignProxy & proxy) const;

  virtual Scalar run(const Sample & y,
                     const Indices & indices,
                     const DesignProxy & proxy) const;

  virtual Scalar run(LeastSquaresMethod & method,
                     const Sample & y) const;

#endif

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

}; /* class FittingAlgorithmImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FITTINGALGORITHMIMPLEMENTATION_HXX */
