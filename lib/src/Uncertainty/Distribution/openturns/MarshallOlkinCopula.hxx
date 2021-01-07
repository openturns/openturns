//                                               -*- C++ -*-
/**
 *  @brief The MarshallOlkinCopula distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MARSHALLOLKINCOPULA_HXX
#define OPENTURNS_MARSHALLOLKINCOPULA_HXX

#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MarshallOlkinCopula
 *
 * The MarshallOlkinCopula distribution.
 */
class OT_API MarshallOlkinCopula
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MarshallOlkinCopula();

  /** Parameters constructor */
  MarshallOlkinCopula(const Scalar alpha,
                      const Scalar beta);

  /** Comparison operator */
  Bool operator ==(const MarshallOlkinCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */
  /** Virtual constructor */
  MarshallOlkinCopula * clone() const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /* Interface specific to MarshallOlkinCopula */

  /** Alpha accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Beta accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /** Get the Spearman correlation of the distribution */
  CorrelationMatrix getSpearmanCorrelation() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** The first parameter of the MarshallOlkinCopula distribution */
  Scalar alpha_;

  /** The second parameter of the MarshallOlkinCopula distribution */
  Scalar beta_;

}; /* class MarshallOlkinCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARSHALLOLKINCOPULA_HXX */
