//                                               -*- C++ -*-
/**
 *  @brief A factory for building BasisSequence objects
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_BASISSEQUENCEFACTORY_HXX
#define OPENTURNS_BASISSEQUENCEFACTORY_HXX

#include <iostream>
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/BasisSequence.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"
#include "openturns/LeastSquaresMethod.hxx"


BEGIN_NAMESPACE_OPENTURNS



/**
 * @class BasisSequenceFactory
 *
 * A factory for building BasisSequence objects
 */
class OT_API BasisSequenceFactory
  : public TypedInterfaceObject<BasisSequenceFactoryImplementation>
{
  CLASSNAME;

public:


  /** Default constructor */
  BasisSequenceFactory();

  /** Constructor from implementation */
  BasisSequenceFactory(const BasisSequenceFactoryImplementation & implementation);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Method to create new BasisSequence objects */
  BasisSequence build(const NumericalSample & x,
                      const NumericalSample & y,
                      const Basis & psi,
                      const Indices & indices);
#ifndef SWIG
  BasisSequence build(const NumericalSample & y,
                      const Indices & indices,
                      const DesignProxy & proxy);

  BasisSequence build(LeastSquaresMethod & method,
                      const NumericalSample & y);

  void initialize();
  void updateBasis(LeastSquaresMethod & method,
                   const NumericalSample & y);

#endif

  /** Stopping criterion on the L1-norm of the coefficients accessor */
  void setMaximumRelativeConvergence(const NumericalScalar coefficientsPaths);
  NumericalScalar getMaximumRelativeConvergence() const;

}; /* class BasisSequenceFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BASISSEQUENCEFACTORY_HXX */
