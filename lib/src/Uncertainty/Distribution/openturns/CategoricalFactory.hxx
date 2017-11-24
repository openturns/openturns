//                                               -*- C++ -*-
/**
 *  @brief Factory for Categorical distribution
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
#ifndef OPENTURNS_CATEGORICALFACTORY_HXX
#define OPENTURNS_CATEGORICALFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Categorical.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CategoricalFactory
 */
class OT_API CategoricalFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CategoricalFactory();

  /** Virtual constructor */
  virtual CategoricalFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const Sample & sample) const;
  Implementation build(const Sample & sample,
                       const Scalar epsilon) const;
  Implementation build() const;
  Categorical buildAsCategorical(const Sample & sample,
                                 const Scalar epsilon = 0.0) const;
  Categorical buildAsCategorical() const;

}; /* class CategoricalFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CATEGORICALFACTORY_HXX */
