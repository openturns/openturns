//                                               -*- C++ -*-
/**
 *  @brief Least Angle Regression Algorithm
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
#ifndef OPENTURNS_LARS_HXX
#define OPENTURNS_LARS_HXX

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
  LARS * clone() const override;

  /** Method to create new BasisSequence objects */
  BasisSequence build(const Sample & x,
                      const Sample & y,
                      const FunctionCollection & psi,
                      const Indices & indices) override;
#ifndef SWIG
//   virtual BasisSequence build(const Sample & y,
//                               const Indices & indices,
//                               const DesignProxy & proxy);

  void initialize() override;
  void updateBasis(LeastSquaresMethod & method, const Sample & y) override;
#endif
  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Selection history accessor */
  Collection<Indices> getSelectionHistory(Collection<Point> & coefficientsHistory) const override;

private:
  Scalar relativeConvergence_;

  Point coefficients_;
  Point mu_;
  Scalar oldCoefficientsL1Norm_;
  Scalar coefficientsL1Norm_;

  Indices predictors_;
  Indices inPredictors_;

  mutable Matrix mPsiX_;

  /** Selection history */
  Collection<Indices> indicesHistory_;
  PersistentCollection<Point> coefficientsHistory_;

}; /* class LARS */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LARS_HXX */
