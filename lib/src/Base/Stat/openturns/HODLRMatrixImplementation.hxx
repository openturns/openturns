//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrixImplementation.hxx
 *  @brief HODLR compressed matrix implementation
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
 */
#ifndef OPENTURNS_HODLRMATRIXIMPLEMENTATION_HXX
#define OPENTURNS_HODLRMATRIXIMPLEMENTATION_HXX

#include "openturns/OTconfig.hxx"
#include "openturns/OTprivate.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/HODLRMatrixParameters.hxx"
#include "openturns/HODLRCore.hxx"

#include <functional>

BEGIN_NAMESPACE_OPENTURNS

class HODLRMatrixFactory;

/**
 * User-facing assembly callback (same pattern as HMatrixRealAssemblyFunction).
 * Also serves as HODLREntryEvaluator for the tree.
 */
class OT_API HODLRRealAssemblyFunction
  : public HODLREntryEvaluator
{
public:
  virtual ~HODLRRealAssemblyFunction() {}
  HODLRRealAssemblyFunction* clone() const override { return new HODLRRealAssemblyFunction(*this); }
  UnsignedInteger getSize() const override { return 0; }
};

/**
 * Function-based evaluator using std::function.
 * Avoids cross-DLL virtual dispatch issues with Python-defined evaluators.
 */
class OT_API HODLRFunctionEvaluator
  : public HODLRRealAssemblyFunction
{
public:
  typedef std::function<Scalar(UnsignedInteger, UnsignedInteger)> EvalFunction;

  explicit HODLRFunctionEvaluator(EvalFunction f, UnsignedInteger size)
    : f_(std::move(f)), size_(size) {}

  HODLRFunctionEvaluator* clone() const override
  {
    return new HODLRFunctionEvaluator(f_, size_);
  }

  Scalar operator()(UnsignedInteger i, UnsignedInteger j) const override
  {
    return f_(i, j);
  }

  UnsignedInteger getSize() const override { return size_; }

private:
  EvalFunction f_;
  UnsignedInteger size_;
};

/**
 * Covariance model evaluator for HODLR assembly.
 */
class OT_API HODLRCovarianceAssemblyFunction
  : public HODLRRealAssemblyFunction
{
public:
  HODLRCovarianceAssemblyFunction(const CovarianceModel & covarianceModel,
                                  const Sample & vertices);
  HODLRCovarianceAssemblyFunction* clone() const override { return new HODLRCovarianceAssemblyFunction(*this); }
  Scalar operator()(UnsignedInteger i, UnsignedInteger j) const override;
  UnsignedInteger getSize() const override;

private:
  const CovarianceModel covarianceModel_;
  const Pointer<const CovarianceModelImplementation> implementation_;
  const Sample vertices_;
  const Collection<Scalar>::const_iterator verticesBegin_;
  const UnsignedInteger inputDimension_;
  const UnsignedInteger covarianceDimension_;
  const UnsignedInteger size_;
  // Fast Matern kernel evaluation path: when the covariance model is a
  // MaternModel (or an IsotropicCovarianceModel wrapping one) with unit
  // output dimension and half-integer smoothness, the kernel is evaluated
  // directly from the raw coordinates to avoid the virtual dispatch and
  // bounds-checked accesses of the generic computeAsScalar path. These
  // members are set once in the constructor and read-only afterwards.
  Point fastSqrt2nuOverTheta_;
  Scalar fastAmplitudeSquare_;
  Scalar fastNuggetFactor_;
  Scalar fastNu_;
  bool useFastMatern_;
};

/**
 * HODLR compressed matrix implementation.
 *
 * The lifecycle is:
 *   1. Factory::build() creates an empty Implementation (n set, no tree)
 *   2. assemble() builds the HODLR tree using the provided evaluator
 *   3. factorize() calls computeCholesky() on the tree (Cholesky + log_det)
 *   4. solve() / logDeterminant() / gemv() use the factored tree
 */
class OT_API HODLRMatrixImplementation
  : public PersistentObject
{
  friend class HODLRMatrixFactory;

  CLASSNAME

public:
  HODLRMatrixImplementation();
  HODLRMatrixImplementation(const HODLRMatrixImplementation& other);
  HODLRMatrixImplementation * clone() const override;
  HODLRMatrixImplementation& operator=(const HODLRMatrixImplementation& rhs);
  virtual ~HODLRMatrixImplementation();

  UnsignedInteger getNbRows() const;
  UnsignedInteger getNbColumns() const;

  const HODLRMatrixParameters& getParameters() const;

  void assemble(const HODLRRealAssemblyFunction& f, char symmetry);
  void assemble(const HODLRRealAssemblyFunction& f, const HODLRMatrixParameters& parameters, char symmetry);
  void factorize();

  void setPermutation(const Indices& permutation);
  Indices getPermutation() const;

  void scale(Scalar alpha);
  void gemv(char trans, Scalar alpha, const Point& x, Scalar beta, Point& y) const;
  void applyFactor(Point& y, const Point& x) const;

  void addIdentity(Scalar alpha);
  void applyNugget();
  Scalar norm() const;
  Point getDiagonal() const;

  /** Get the regularization shift applied during the last factorization */
  Scalar getRegularizationShift() const;

  Point solve(const Point& b, Bool trans = false) const;
  Matrix solve(const Matrix& m, Bool trans = false) const;
  Point solveLower(const Point& b, Bool trans = false) const;
  Matrix solveLower(const Matrix& m, Bool trans = false) const;

  Scalar logDeterminant() const;
  std::pair<size_t, size_t> compressionRatio() const;
  std::pair<size_t, size_t> fullrkRatio() const;
  void dump(const String & name) const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through an Advocate */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from an Advocate */
  void load(Advocate & adv) override;

private:
  void rebuild();

  UnsignedInteger n_;
  Pointer<HODLRNode> p_node_;
  bool isFactorized_;
  bool symmetric_;
  Scalar logDet_;
  HODLRMatrixParameters parameters_;
  Point diagonal_;
  Scalar shiftAccumulated_;
  Pointer<const HODLREntryEvaluator> p_evaluator_;
  Indices permutation_;
  Indices inversePermutation_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HODLRMATRIXIMPLEMENTATION_HXX */
