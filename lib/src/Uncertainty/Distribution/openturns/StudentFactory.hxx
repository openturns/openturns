//                                               -*- C++ -*-
/**
 *  @brief Factory for Student distribution
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
#ifndef OPENTURNS_STUDENTFACTORY_HXX
#define OPENTURNS_STUDENTFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Student.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StudentFactory
 */
class OT_API StudentFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  StudentFactory();

  /** Virtual constructor */
  virtual StudentFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const;
  Distribution build(const Point & parameter) const;
  Distribution build() const;

  using DistributionFactoryImplementation::buildEstimator;
  DistributionFactoryResult buildEstimator(const Sample & sample) const;

  Student buildAsStudent(const Sample & sample) const;
  Student buildAsStudent(const Point & parameter) const;
  Student buildAsStudent() const;

}; /* class StudentFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDENTFACTORY_HXX */
