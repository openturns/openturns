//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' Distribution
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

#ifndef OPENTURNS_PYTHONDISTRIBUTION_HXX
#define OPENTURNS_PYTHONDISTRIBUTION_HXX

#include <Python.h>
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class PythonDistribution
 *
 * This class binds a Python object to an OpenTURNS' RandomVector
 */
class PythonDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:


  /** Constructor from Python object*/
  PythonDistribution(PyObject * pyObject);

  /** Copy constructor */
  PythonDistribution(const PythonDistribution & other);

  /** Virtual constructor */
  virtual PythonDistribution * clone() const;

  /** Copy assignment operator */
  PythonDistribution & operator=(const PythonDistribution & rhs);

  /** Destructor */
  virtual ~PythonDistribution();

  /** Comparison operator */
  Bool operator ==(const PythonDistribution & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;


  /* Here is the interface that all derived class must implement */

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get a numerical sample whose elements follow the distributionImplementation */
  Sample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the distribution */
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  Scalar computePDF(const Point & point) const;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the distribution */
  Scalar computeCDF(const Point & point) const;

  /** Get the complementary CDF of the distribution */
  Scalar computeComplementaryCDF(const Point & point) const;

  /** Get the quantile of the distributionImplementation */
  virtual Point computeQuantile(const Scalar prob,
                                const Bool tail = false) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  Scalar getRoughness() const;

  /** Get the mean of the distribution */
  Point getMean() const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const;

  /** Get the raw moments of the distribution */
  Point getMoment(const UnsignedInteger n) const;

  /** Get the centered moments of the distribution */
  Point getCenteredMoment(const UnsignedInteger n) const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

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
  virtual Distribution getMarginal(const Indices & indices) const;
  virtual Distribution getMarginal(const UnsignedInteger i) const;

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
