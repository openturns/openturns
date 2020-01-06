//                                               -*- C++ -*-
/**
 *  @brief Least Angle Regression Algorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LARS_HXX
#define OPENTURNS_LARS_HXX

#include <limits>
#include "openturns/Matrix.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LARS
 *
 * A sequence of numerical math function collection.
 */
class OT_API LARS
  : public BasisSequenceFactoryImplementation
{
  CLASSNAME
public:
  typedef BasisSequenceFactoryImplementation::FunctionCollection FunctionCollection;


  /** Default constructor */
  explicit LARS(const Bool verbose = false);

  /** Virtual constructor */
  virtual LARS * clone() const;

  /** Method to create new BasisSequence objects */
  virtual BasisSequence build(const Sample & x,
                              const Sample & y,
                              const FunctionCollection & psi,
                              const Indices & indices);
#ifndef SWIG
//   virtual BasisSequence build(const Sample & y,
//                               const Indices & indices,
//                               const DesignProxy & proxy);

  virtual void initialize();
  virtual void updateBasis(LeastSquaresMethod & method,
                           const Sample & y);
#endif
  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  Scalar relativeConvergence_;

  Point coefficients_;
  Point mu_;
  Scalar oldCoefficientsL1Norm_;
  Scalar coefficientsL1Norm_;

  Indices predictors_;
  Indices inPredictors_;

  mutable Matrix mPsiX_;
}; /* class LARS */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LARS_HXX */
