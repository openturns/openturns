//                                               -*- C++ -*-
/**
 *  @brief Regression from a data sample upon a particular basis
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_APPROXIMATIONALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_APPROXIMATIONALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ApproximationAlgorithmImplementation
 *
 * Regression from a data sample upon a particular basis
 */
class OT_API ApproximationAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;


  /** Default constructor */
  ApproximationAlgorithmImplementation();

  /** Default constructor */
  ApproximationAlgorithmImplementation(const Sample & x,
                                       const Sample & y,
                                       const FunctionCollection & psi,
                                       const Indices & indices);

  /** Default constructor */
  ApproximationAlgorithmImplementation(const Sample & x,
                                       const Sample & y,
                                       const Point & weight,
                                       const FunctionCollection & psi,
                                       const Indices & indices);

  /** Virtual constructor */
  virtual ApproximationAlgorithmImplementation * clone() const;

  /** Sample accessors */
  Sample getX() const;
  Sample getY() const;

  /** Weigths accessor */
protected:
  void setWeight(const Point & weight);
public:
  Point getWeight() const;

  /** Basis accessor */
  FunctionCollection getPsi() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Perform the selection */
  virtual void run();
#ifndef SWIG
  virtual void run(const DesignProxy & proxy);
#endif

  /** Accessor to the coefficients of the selected metamodel on the provided basis */
  Point getCoefficients();

  /** Accessor to the residual of the selected metamodel on the provided basis */
  Scalar getResidual();

  Scalar getRelativeError();

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  void setCoefficients(const Point & coefficients);

  void setResidual(const Scalar residual);

  void setRelativeError(const Scalar relativeError);

  /** Input sample */
  Sample x_;

  /** Output sample */
  Sample y_;

  /** Weights */
  Point weight_;

  /** Flag to tell if the weights are uniform */
  Bool hasUniformWeight_;

  /** Basis */
  FunctionPersistentCollection psi_;
  Indices currentIndices_;

  mutable Bool isAlreadyComputedCoefficients_;

private:
  /** Regression coefficients */
  Point coefficients_;

  /** Residual */
  Scalar residual_;

  /** Relative error */
  Scalar relativeError_;

  /** Verbosity flag */
  Bool verbose_;


}; /* class ApproximationAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_APPROXIMATIONALGORITHMIMPLEMENTATION_HXX */
