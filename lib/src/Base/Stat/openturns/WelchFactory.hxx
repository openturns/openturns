//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_WELCHFACTORY_HXX
#define OPENTURNS_WELCHFACTORY_HXX

#include "openturns/SpectralModelFactoryImplementation.hxx"
#include "openturns/UserDefinedSpectralModel.hxx"
#include "openturns/FilteringWindows.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WelchFactory
 */
class OT_API WelchFactory
  : public SpectralModelFactoryImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  WelchFactory();

  /** Standard constructor using a filtering windows and a frequency limit*/
  WelchFactory(const FilteringWindows & window,
               const UnsignedInteger blockNumber,
               const Scalar overlap = 0.5);

  /** Virtual constructor */
  virtual WelchFactory * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** FilteringWindows accessor */
  FilteringWindows getFilteringWindows() const;
  void setFilteringWindows(const FilteringWindows & window);

  /** Number of block accessor */
  UnsignedInteger getBlockNumber() const;
  void setBlockNumber(const UnsignedInteger blockNumber);

  /** Overlap accessor */
  Scalar getOverlap() const;
  void setOverlap(const Scalar overlap);

  /** Build a spectral model based on a sample */
  Implementation build(const FieldSample & sample) const;

  /** Build a spectral model based on a Field */
  Implementation build(const Field & timeSerie) const;

  /** Build a spectral model based on a sample */
  UserDefinedSpectralModel buildAsUserDefinedSpectralModel(const FieldSample & sample) const;

  /** Build a spectral model based on a sample */
  UserDefinedSpectralModel buildAsUserDefinedSpectralModel(const Field & timeSeries) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** FilteringWindows */
  FilteringWindows window_;

  /** Size of a block */
  UnsignedInteger blockNumber_;

  /** Overlap percentage for Welch method */
  Scalar overlap_;

}; /* class WelchFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WELCHFACTORY_HXX */
