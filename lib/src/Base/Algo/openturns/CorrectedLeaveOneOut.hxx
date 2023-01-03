//                                               -*- C++ -*-
/**
 *  @brief Corrected implicit leave-one-out cross validation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CORRECTEDLEAVEONEOUT_HXX
#define OPENTURNS_CORRECTEDLEAVEONEOUT_HXX

#include "openturns/FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class CorrectedLeaveOneOut
 *
 * Corrected implicit leave-one-out cross validation
 */
class OT_API CorrectedLeaveOneOut
  : public FittingAlgorithmImplementation
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  CorrectedLeaveOneOut();

  /** Virtual constructor */
  CorrectedLeaveOneOut * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform cross-validation */
  Scalar run(const Sample & x,
             const Sample & y,
             const Point & weight,
             const FunctionCollection & psi,
             const Indices & indices) const override;

#ifndef SWIG
  Scalar run(const Sample & y,
             const Point & weight,
             const Indices & indices,
             const DesignProxy & proxy) const override;

  Scalar run(LeastSquaresMethod & method,
             const Sample & y) const override;
#endif

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

}; /* class CorrectedLeaveOneOut */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CORRECTEDLEAVEONEOUT_HXX */
