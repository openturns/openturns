//                                               -*- C++ -*-
/**
 *  @brief Dlib functions interface
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DLIBFUNCTIONS_HXX
#define OPENTURNS_DLIBFUNCTIONS_HXX

#include "openturns/OSS.hxx"
#include "openturns/Os.hxx"
#include "openturns/Function.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

#ifdef OPENTURNS_HAVE_DLIB
#include <dlib/optimization.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

/** Define DlibMatrix type for better readability **/
typedef dlib::matrix<double> DlibMatrix;
typedef dlib::matrix<double, 0, 1> DlibVector;

/** Class DlibGradient to ensure interface between OT::Gradient and dlib functions **/
class DlibGradient
  : public Gradient
{
public:
  // Constructor
  explicit DlibGradient(const Gradient & gradient);

  // Operator () for use with DlibFunction
  DlibMatrix operator() (const DlibMatrix & inP) const;

  // Operator () for use with residual DlibFunction
  DlibMatrix operator() (const UnsignedInteger i,
                         const DlibMatrix & inP) const;

} ;

/**  Class DlibHessian to ensure interface between OT::Hessian and dlib functions  **/
class DlibHessian
  : public Hessian
{
public:
  // Constructor
  explicit DlibHessian(const Hessian & hessian);

  // Operator ()
  DlibMatrix operator() (const DlibMatrix & inP) const;
} ;

/** Class DlibFunction to ensure interface between OT::Function and dlib functions **/
class DlibFunction
  : public Function
{
  CLASSNAME

  /** Define types for use with dlib::find_min_trust_region() **/
  typedef DlibVector column_vector;
  typedef DlibMatrix general_matrix;

public:
  // Constructor
  explicit DlibFunction(const Function & function);

  // Operator ()
  double operator() (const DlibMatrix & inP) const;

  // Operator ()
  double operator() ( const UnsignedInteger i,
                      const DlibMatrix & inP) const;

  /* Accessors to gradient */
  // As a DlibGradient
  DlibGradient getGradient() const;

  // Compute at point
  DlibMatrix gradient(const DlibMatrix & inP) const;

  DlibMatrix gradient(const UnsignedInteger i,
                      const DlibMatrix & inP) const;

  /* Accessor to hessian */
  // As a DlibHessian
  DlibHessian getHessian() const;

  // Compute at point
  DlibMatrix hessian(const DlibMatrix & inP) const;

  // Compute gradient and hessian at once
  void get_derivative_and_hessian(const column_vector x,
                                  column_vector & grad,
                                  general_matrix & hess) const;

  /* Accessors to attributes */
  UnsignedInteger getEvaluationNumber() const;
  Sample getInputHistory();
  Sample getOutputHistory();

protected:
  mutable Sample inputHistory_;
  mutable Sample outputHistory_;
} ;

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_DLIBFUNCTIONS_HXX
