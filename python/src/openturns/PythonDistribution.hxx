//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' Distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_PYTHONDISTRIBUTION_HXX
#define OPENTURNS_PYTHONDISTRIBUTION_HXX

#include <Python.h>
#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class PythonDistribution
 *
 * This class binds a Python object to an OpenTURNS' RandomVector
 */
class PythonDistribution
  : public DistributionImplementation
{
  CLASSNAME;
public:


  /** Constructor from Python object*/
  PythonDistribution(PyObject * pyObject);

  /** Copy constructor */
  PythonDistribution(const PythonDistribution & other);

  /** Virtual constructor */
  virtual PythonDistribution * clone() const;

  /** Destructor */
  virtual ~PythonDistribution();

  /** Comparison operator */
  Bool operator ==(const PythonDistribution & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /* Here is the interface that all derived class must implement */

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get a numerical sample whose elements follow the distributionImplementation */
  Sample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the distribution */
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the distribution */
  NumericalScalar computePDF(const NumericalPoint & point) const;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the complementary CDF of the distribution */
  NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;

  /** Get the quantile of the distributionImplementation */
  virtual NumericalPoint computeQuantile(const NumericalScalar prob,
                                         const Bool tail = false) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;

  /** Get the PDFGradient of the distribution */
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDFGradient of the distribution */
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the quantile of the distribution */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  NumericalScalar getRoughness() const;

  /** Get the mean of the distribution */
  NumericalPoint getMean() const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the raw moments of the distribution */
  NumericalPoint getMoment(const UnsignedInteger n) const;

  /** Get the centered moments of the distribution */
  NumericalPoint getCenteredMoment(const UnsignedInteger n) const;

  /** Parameters value and description accessor */
  //   NumericalPointWithDescriptionCollection getParametersCollection() const;
  //   void setParametersCollection(const NumericalPointCollection & parametersCollection);
  //

  /** Check if the distribution is a copula */
  Bool isCopula() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is integral */
  Bool isIntegral() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  virtual Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method save() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Compute the numerical range of the distribution given the parameters values */
  virtual void computeRange();


protected:


private:

  friend class Factory<PythonDistribution>;

  /** Default constructor */
  PythonDistribution();

  /** The underlying Python object */
  PyObject * pyObj_;

}; /* class PythonDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONDISTRIBUTION_HXX */
