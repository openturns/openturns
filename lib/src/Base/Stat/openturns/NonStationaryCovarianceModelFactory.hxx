//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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
#ifndef OPENTURNS_NONSTATIONARYCOVARIANCEMODELFACTORY_HXX
#define OPENTURNS_NONSTATIONARYCOVARIANCEMODELFACTORY_HXX

#include "openturns/CovarianceModelFactoryImplementation.hxx"
#include "openturns/UserDefinedCovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NonStationaryCovarianceModelFactory
 */
class OT_API NonStationaryCovarianceModelFactory
  : public CovarianceModelFactoryImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  NonStationaryCovarianceModelFactory();

  /** Virtual constructor */
  virtual NonStationaryCovarianceModelFactory * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Build a a spectral model based on a sample */
  using CovarianceModelFactoryImplementation::build;
  CovarianceModelImplementation::Implementation build(const ProcessSample & sample) const;
  CovarianceModelImplementation::Implementation build(const ProcessSample & sample,
                                                      const Bool isCentered) const;

  UserDefinedCovarianceModel buildAsUserDefinedCovarianceModel(const ProcessSample & sample,
                                                      const Bool isCentered = false) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


}; /* class NonStationaryCovarianceModelFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NONSTATIONARYCOVARIANCEMODELFACTORY_HXX */
