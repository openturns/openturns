//                                               -*- C++ -*-
/**
 *  @brief Factory for MatrixFisher distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MATRIXFISHERFACTORY_HXX
#define OPENTURNS_MATRIXFISHERFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/MatrixFisher.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API MatrixFisherFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  MatrixFisherFactory();

  MatrixFisherFactory * clone() const override;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;

  MatrixFisher buildAsMatrixFisher(const Sample & sample) const;
  MatrixFisher buildAsMatrixFisher(const Point & parameters) const;
  MatrixFisher buildAsMatrixFisher() const;

}; /* class MatrixFisherFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIXFISHERFACTORY_HXX */