//                                               -*- C++ -*-
/**
 *  @brief PythonActiveLearningReliabilityFunctionImplementation binds a Python object to an OpenTURNS' ActiveLearningReliabilityFunctionImplementation
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

#ifndef OPENTURNS_PYTHONACTIVELEARNINGRELIABILITYFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_PYTHONACTIVELEARNINGRELIABILITYFUNCTIONIMPLEMENTATION_HXX

#include <Python.h>
#include "openturns/ActiveLearningReliabilityFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PythonActiveLearningReliabilityFunctionImplementation
 *
 * This class binds a Python object to an OpenTURNS' ActiveLearningReliabilityFunctionImplementation
 */
class PythonActiveLearningReliabilityFunctionImplementation
  : public ActiveLearningReliabilityFunctionImplementation
{
  CLASSNAME
public:

  /** Constructor from Python object */
  explicit PythonActiveLearningReliabilityFunctionImplementation(PyObject * pyObject);

  /** Copy constructor */
  PythonActiveLearningReliabilityFunctionImplementation(const PythonActiveLearningReliabilityFunctionImplementation & other);

  /** Virtual constructor */
  PythonActiveLearningReliabilityFunctionImplementation * clone() const override;

  /** Copy assignment operator */
  PythonActiveLearningReliabilityFunctionImplementation & operator=(const PythonActiveLearningReliabilityFunctionImplementation & rhs);

  /** Destructor */
  ~PythonActiveLearningReliabilityFunctionImplementation() override;

  /** String converter */
  String __repr__() const override;

  /** Is it safe to call in parallel? No: Python calls hold the GIL. */
  Bool isParallel() const override;

  /** Score the candidate pool against the current design */
  Sample operator()(const Sample & inputSample, const Sample & inputDoE) const override;

  /** Select the infill sample to evaluate on the true function */
  Sample getInfillSample(const Sample & inputSample,
                         const Sample & criterionValues) const override;

  /** Test convergence of active learning */
  Bool checkConvergenceLearning(const Sample & criterionValues) const override;

  /** Selection direction */
  Bool isMaximization() const override;

  /** Gaussian process surrogate accessor, keeps the Python side in sync */
  void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult) override;

  /** Threshold accessors, keep the Python side in sync */
  void setReliabilityThreshold(const Scalar reliabilityThreshold) override;
  void setLearningThreshold(const Scalar learningThreshold) override;
  Scalar getReliabilityThreshold() const override;
  Scalar getLearningThreshold() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Pointwise score, forwards to Python computeAsScalar */
  Scalar computeAsScalar(const Point & x) const override;

private:

  friend class Factory<PythonActiveLearningReliabilityFunctionImplementation>;

  /** Default constructor */
  PythonActiveLearningReliabilityFunctionImplementation();

  /** Push the current GPR result to the Python object (best effort, no throw on missing hook) */
  void syncGPRToPython() const;

  /** Push the current thresholds to the Python object (best effort) */
  void syncThresholdsToPython() const;

  /** Pull the thresholds from the Python object (best effort) */
  void syncThresholdsFromPython() const;

  /** The underlying Python object */
  PyObject * pyObj_ = nullptr;

}; /* class PythonActiveLearningReliabilityFunctionImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONACTIVELEARNINGRELIABILITYFUNCTIONIMPLEMENTATION_HXX */
