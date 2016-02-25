//                                               -*- C++ -*-
/**
 *  @brief Least Angle Regression Algorithm
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LAR_HXX
#define OPENTURNS_LAR_HXX

#include <limits>
#include "openturns/Matrix.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LAR
 *
 * A sequence of numerical math function collection.
 */
class OT_API LAR
  : public BasisSequenceFactoryImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  explicit LAR(const Bool verbose = false);

  /** Virtual constructor */
  virtual LAR * clone() const;

  /** Method to create new BasisSequence objects */
  virtual BasisSequence build(const NumericalSample & x,
                              const NumericalSample & y,
                              const Basis & psi,
                              const Indices & indices);
#ifndef SWIG
//   virtual BasisSequence build(const NumericalSample & y,
//                               const Indices & indices,
//                               const DesignProxy & proxy);

  virtual void initialize();
  virtual void updateBasis(LeastSquaresMethod & method);
#endif
  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  NumericalScalar relativeConvergence_;

  NumericalPoint coefficients_;
  NumericalPoint mu_;
  NumericalScalar oldCoefficientsL1Norm_;
  NumericalScalar coefficientsL1Norm_;

  Indices predictors_;
  Indices inPredictors_;

  mutable Matrix mPsiX_;
}; /* class LAR */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LAR_HXX */
