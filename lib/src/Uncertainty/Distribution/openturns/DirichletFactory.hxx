//                                               -*- C++ -*-
/**
 *  @brief Factory for Dirichlet distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DIRICHLETFACTORY_HXX
#define OPENTURNS_DIRICHLETFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Dirichlet.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DirichletFactory
 */
class OT_API DirichletFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  DirichletFactory();

  /** Virtual constructor */
  DirichletFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  Dirichlet buildAsDirichlet(const Sample & sample) const;
  Dirichlet buildAsDirichlet(const Point & parameters) const;
  Dirichlet buildAsDirichlet() const;

}; /* class DirichletFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DIRICHLETFACTORY_HXX */
