//                                               -*- C++ -*-
/**
 *  @brief Factory for AliMikhailHaqCopula distribution
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
#ifndef OPENTURNS_ALIMIKHAILHAQCOPULAFACTORY_HXX
#define OPENTURNS_ALIMIKHAILHAQCOPULAFACTORY_HXX

#include "OTprivate.hxx"
#include "DistributionImplementationFactory.hxx"
#include "AliMikhailHaqCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class AliMikhailHaqCopulaFactory
 */
class OT_API AliMikhailHaqCopulaFactory
  : public DistributionImplementationFactory
{
  CLASSNAME;
public:

  /** Default constructor */
  AliMikhailHaqCopulaFactory();

  /** Virtual constructor */
  virtual AliMikhailHaqCopulaFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionImplementationFactory::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  AliMikhailHaqCopula buildAsAliMikhailHaqCopula(const NumericalSample & sample) const;
  AliMikhailHaqCopula buildAsAliMikhailHaqCopula(const NumericalPoint & parameters) const;
  AliMikhailHaqCopula buildAsAliMikhailHaqCopula() const;

private:
  // Compute Kendall's tau from AliMikhailHaq copula's parameter
  NumericalScalar KendallTauFromParameter(const NumericalScalar theta) const;

}; /* class AliMikhailHaqCopulaFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ALIMIKHAILHAQCOPULAFACTORY_HXX */
