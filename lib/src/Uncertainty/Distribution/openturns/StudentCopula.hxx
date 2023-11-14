//                                               -*- C++ -*-
/**
 *  @brief Student or T-Copula
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
#ifndef OPENTURNS_STUDENTCOPULA_HXX
#define OPENTURNS_STUDENTCOPULA_HXX

#include "openturns/SklarCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StudentCopula
 *
 * The StudentCopula distribution.
 */
class OT_API StudentCopula
  : public SklarCopula
{
  CLASSNAME
public:

  /** Default constructor */
  StudentCopula();

  /** Parameters constructor */
  StudentCopula(const Scalar nu,
                const CorrelationMatrix & R);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */
  /** Virtual constructor */
  StudentCopula * clone() const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Nu parameter accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

  /** R parameter accessor */
  void setR(const CorrelationMatrix & R);
  CorrelationMatrix getR() const;

}; /* class StudentCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDENTCOPULA_HXX */
