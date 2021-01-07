//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' Distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
  PythonDistribution * clone() const override;

  /** Copy assignment operator */
  PythonDistribution & operator=(const PythonDistribution & rhs);

  /** Destructor */
  virtual ~PythonDistribution();

  /** Comparison operator */
  Bool operator ==(const PythonDistribution & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Here is the interface that all derived class must implement */

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get a numerical sample whose elements follow the distributionImplementation */
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the DDF of the distribution */
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  Scalar computePDF(const Point & point) const override;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  Scalar computeCDF(const Point & point) const override;

  /** Get the complementary CDF of the distribution */
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Get the quantile of the distributionImplementation */
  virtual Point computeQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const override;

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  Scalar getRoughness() const override;

  /** Get the mean of the distribution */
  Point getMean() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const override;

  /** Get the raw moments of the distribution */
  Point getMoment(const UnsignedInteger n) const override;

  /** Get the centered moments of the distribution */
  Point getCenteredMoment(const UnsignedInteger n) const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is a copula */
  Bool isCopula() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is integral */
  Bool isIntegral() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method save() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;


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
