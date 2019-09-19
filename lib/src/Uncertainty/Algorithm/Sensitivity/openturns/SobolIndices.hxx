//                                               -*- C++ -*-
/**
 *  @brief SobolIndices provides data structure for Sobol indices
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SOBOLINDICES_HXX
#define OPENTURNS_SOBOLINDICES_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SobolIndicesImplementation.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SobolIndices
 * This class provides data structure for Sobol indices
 */

class OT_API SobolIndices
  : public TypedInterfaceObject<SobolIndicesImplementation>
{

  CLASSNAME
public:

  typedef Pointer<SobolIndicesImplementation> Implementation;

  /* Default constructor */
  SobolIndices();

  /* Constructor from an implementation */
  SobolIndices(const SobolIndicesImplementation & implementation);

  /* Constructor from a Pointer to an implementation */
  SobolIndices(const Implementation & p_implementation);

  /* Standard constructor */
  SobolIndices(const UnsignedInteger modelInputDimension,
               const UnsignedInteger modelOutputDimension);

  /** Set the input and output model dimensions */
  void SetModelDimensions(const UnsignedInteger modelInputDimension,
                          const UnsignedInteger modelOutputDimension);

  /** Get the model input dimension */
  UnsignedInteger GetModelInputDimensions();

  /** Get the model output dimension */
  UnsignedInteger GetModelOutputDimensions();

  void computeIndices(const Sample inputSample);

  void incrementIndices(const Sample inputSample);

  Point getMean() const;

  Point getVariance() const;

  Point getFirstOrderIndices(const UnsignedInteger marginalIndex) const;

  Point getTotalOrderIndices(const UnsignedInteger marginalIndex) const;

  Interval getFirstOrderIndicesInterval() const;

  Interval getTotalOrderIndicesInterval() const;

  Point getAggregatedFirstOrderIndices() const;

  Point getAggregatedTotalOrderIndices() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class SobolIndices */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLINDICES_HXX */
