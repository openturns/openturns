//                                               -*- C++ -*-
/**
 *  @brief Classification algorithm base type
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
#ifndef OPENTURNS_CLASSIFIERIMPLEMENTATION_HXX
#define OPENTURNS_CLASSIFIERIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ClassifierImplementation
 *
 * Classification algorithm base type
 */
class OT_API ClassifierImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  ClassifierImplementation();

  /** Default constructor */
  ClassifierImplementation(const Sample & inputSample,
                           const Indices & outClasses);

  /** Virtual constructor */
  virtual ClassifierImplementation * clone() const;

  virtual UnsignedInteger getNumberOfClasses() const;

  /** Associate a point with a class */
  virtual UnsignedInteger classify(const Point & inP) const;
  virtual Indices classify(const Sample & inS) const;

protected:
  virtual Indices classifyParallel(const Sample & inS) const;
  virtual Indices classifySequential(const Sample & inS) const;
public:

  /** Grade a point as if it were associated to a class */
  virtual Scalar grade(const Point & inP,
                       const UnsignedInteger outC) const;
  virtual Point grade(const Sample & inS,
                      const Indices & outC) const;

protected:
  virtual Point gradeParallel(const Sample & inS,
                              const Indices & outC) const;
  virtual Point gradeSequential(const Sample & inS,
                                const Indices & outC) const;
public:

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Input space dimension */
  virtual UnsignedInteger getDimension() const;

  /** Parallelization flag accessor */
  void setParallel(const Bool flag);
  Bool isParallel() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /** Input sample */
  Sample inputSample_;

  /** Output sample */
  Indices classes_;

private:
  /** Verbosity flag */
  Bool verbose_;

  /** Flag to tell if parallelization must be used */
  Bool isParallel_;

}; /* class ClassifierImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CLASSIFIERIMPLEMENTATION_HXX */

