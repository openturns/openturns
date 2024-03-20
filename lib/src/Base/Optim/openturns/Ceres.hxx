//                                               -*- C++ -*-
/**
 *  @brief Ceres solver
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
#ifndef OPENTURNS_CERES_HXX
#define OPENTURNS_CERES_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Ceres
 * Ceres solver
 */
class OT_API Ceres :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME
public:
  friend class CostFunctionInterface;
  friend class FirstOrderFunctionInterface;
  friend class IterationCallbackInterface;

  /** Default constructor */
  explicit Ceres(const String & algoName = "LEVENBERG_MARQUARDT");

  /** Constructor with parameters */
  explicit Ceres(const OptimizationProblem & problem,
                 const String & algoName = "LEVENBERG_MARQUARDT");

  /** Virtual constructor */
  Ceres * clone() const override;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run() override;

  /** Algorithm names accessor */
  static Description GetAlgorithmNames();

  void setAlgorithmName(const String algoName);
  String getAlgorithmName() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

#ifndef SWIG
  /** Initialize logging library */
  static void Initialize();
#endif

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const override;

  String algoName_;

private:

  // temporary, used to track input/outputs
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;

};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CERES_HXX */
