//                                               -*- C++ -*-
/**
 *  @brief PythonEfficientGlobalOptimizationCriterion binds a Python object to
 *  an OpenTURNS' EfficientGlobalOptimizationCriterionImplementation
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
#ifndef OPENTURNS_PYTHONEFFICIENTGLOBALOPTIMIZATIONCRITERION_HXX
#define OPENTURNS_PYTHONEFFICIENTGLOBALOPTIMIZATIONCRITERION_HXX

#include <Python.h>
#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonEfficientGlobalOptimizationCriterion
 *
 * This class binds a Python object to an OpenTURNS'
 * EfficientGlobalOptimizationCriterionImplementation
 */
class PythonEfficientGlobalOptimizationCriterion
  : public EfficientGlobalOptimizationCriterionImplementation
{
  CLASSNAME
public:

  /** Constructor from Python object */
  explicit PythonEfficientGlobalOptimizationCriterion(PyObject * pyObject);

  /** Copy constructor */
  PythonEfficientGlobalOptimizationCriterion(const PythonEfficientGlobalOptimizationCriterion & other);

  /** Virtual constructor */
  PythonEfficientGlobalOptimizationCriterion * clone() const override;

  /** Copy assignment operator */
  PythonEfficientGlobalOptimizationCriterion & operator=(const PythonEfficientGlobalOptimizationCriterion & rhs);

  /** Destructor */
  ~PythonEfficientGlobalOptimizationCriterion() override;

  /** String converter */
  String __repr__() const override;

  /** The objective to maximize, built by the Python criterion */
  Function buildObjective() const override;

  /** Convergence test on the criterion value, forwarded to Python */
  Bool checkConvergence(const Scalar criterionValue) const override;

  /** State accessors, keeping the Python side in sync */
  void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult) override;
  void setInputSample(const Sample & inputSample) override;
  void setOptimalValue(const Scalar optimalValue) override;
  void setMinimization(const Bool minimization) override;
  void setNoiseFunction(const Function & noiseFunction) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  friend class Factory<PythonEfficientGlobalOptimizationCriterion>;

  /** Default constructor */
  PythonEfficientGlobalOptimizationCriterion();

  /** Push the whole current state to the Python object */
  void syncStateToPython() const;

  /** Forward one state value to the Python object, using its setter or attribute */
  void forwardToPython(const char * setterName, const char * attributeName, PyObject * value) const;

  /** The underlying Python object */
  PyObject * pyObj_ = nullptr;

}; /* class PythonEfficientGlobalOptimizationCriterion */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONEFFICIENTGLOBALOPTIMIZATIONCRITERION_HXX */
