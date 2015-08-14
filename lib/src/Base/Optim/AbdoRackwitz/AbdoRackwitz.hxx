//                                               -*- C++ -*-
/**
 *  @brief AbdoRackwitz is an actual implementation for
 *         NearestPointAlgorithm using the AbdoRackwitz algorithm.
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_ABDORACKWITZ_HXX
#define OPENTURNS_ABDORACKWITZ_HXX

#include "OTprivate.hxx"
#include "NearestPointAlgorithmImplementation.hxx"
#include "NumericalPoint.hxx"
#include "AbdoRackwitzSpecificParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AbdoRackwitz
 * AbdoRackwitz is an actual implementation for
 * NearestPointAlgorithm
 */
class OT_API AbdoRackwitz :
  public NearestPointAlgorithmImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  AbdoRackwitz();

  /** Constructor with parameters */
  AbdoRackwitz(const AbdoRackwitzSpecificParameters & specificParameters,
               const NumericalMathFunction & levelFunction);


  /** Virtual constructor */
  virtual AbdoRackwitz * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** Specific parameters accessor */
  AbdoRackwitzSpecificParameters getSpecificParameters() const;

  /** Specific parameters accessor */
  void setSpecificParameters(const AbdoRackwitzSpecificParameters & specificParameters);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** Perform a line search in the given direction */
  NumericalScalar computeLineSearch();

  /** Initialize internal state */
  void initialize();

  /** Abdo Rackwitz specific parameters */
  AbdoRackwitzSpecificParameters specificParameters_;

  /** Abdo Rackwitz current penalization factor */
  NumericalScalar currentSigma_;

  /** Abdo Rackwitz current point */
  NumericalPoint currentPoint_;

  /** Abdo Rackwitz current direction */
  NumericalPoint currentDirection_;

  /** Abdo Rackwitz current level value */
  NumericalScalar currentLevelValue_;

  /** Abdo Rackwitz current gradient as a column vector */
  NumericalPoint currentGradient_;

  /** Abdo Rackwitz current Lagrange multiplier */
  NumericalScalar currentLambda_;

}; /* class AbdoRackwitz */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ABDORACKWITZ_HXX */
