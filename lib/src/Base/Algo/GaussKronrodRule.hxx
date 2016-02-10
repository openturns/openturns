//                                               -*- C++ -*-
/**
 *  @brief This class defines embredded Gauss/Kronrod integration rules
 *         used by the GaussKronrod integration method for 1D scalar function
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
#ifndef OPENTURNS_GAUSSKRONRODRULE_HXX
#define OPENTURNS_GAUSSKRONRODRULE_HXX

#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussKronrodRule
 */

class OT_API GaussKronrodRule
  : public PersistentObject
{

  CLASSNAME;

  /** Default constructor without parameters */
  GaussKronrodRule();

public:

  typedef enum {G1K3 = 0, G3K7 = 1, G7K15 = 2, G11K23 = 3, G15K31 = 4, G25K51 = 5} GaussKronrodPair;

  /** Parameter constructor */
  GaussKronrodRule(const GaussKronrodPair pair);

  /** Virtual copy constructor */
  virtual GaussKronrodRule * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Pair accessor */
  GaussKronrodPair getPair() const;

  /** Order accessor */
  UnsignedInteger getOrder() const;

  /** Zero Gauss weight accessor */
  NumericalScalar getZeroGaussWeight() const;

  /** Other Gauss weight accessor */
  NumericalPoint getOtherGaussWeights() const;

  /** Other Kronrod nodes accessor */
  NumericalPoint getOtherKronrodNodes() const;

  /** Zero Kronrod weight accessor */
  NumericalScalar getZeroKronrodWeight() const;

  /** Other Kronrod weight accessor */
  NumericalPoint getOtherKronrodWeights() const;

private:

  friend class GaussKronrod;

  /** Pair of rules */
  GaussKronrodPair pair_;

  /** Order of the rule */
  UnsignedInteger order_;

  /** Gauss weight of node 0.0 */
  NumericalScalar zeroGaussWeight_;

  /** Gauss weights of the nonzero nodes */
  NumericalPoint otherGaussWeights_;

  /** Kronrod nodes */
  NumericalPoint otherKronrodNodes_;

  /** Kronrod weight of node 0.0 */
  NumericalScalar zeroKronrodWeight_;

  /** Kronrod weights of the nonzero nodes */
  NumericalPoint otherKronrodWeights_;

} ; /* class GaussKronrodRule */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSKRONRODRULE_HXX */
