//                                               -*- C++ -*-
/**
 *  @brief Implementation for conjugate gradient search strategy
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DLIBCONJUGATEGRADIENTSEARCHSTRATEGY_HXX
#define OPENTURNS_DLIBCONJUGATEGRADIENTSEARCHSTRATEGY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DlibSearchStrategyImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObject.hxx"

#ifdef OPENTURNS_HAVE_DLIB
#include <dlib/optimization.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DlibConjugateGradientSearchStrategy
 * Conjugate gradient search strategy for use in dlib solvers
 */

/** Define DlibMatrix type for better readability **/
typedef dlib::matrix<double> DlibMatrix;

class DlibConjugateGradientSearchStrategy
  : public DlibSearchStrategyImplementation
{
  CLASSNAME

public:
  // Constructor with parameters
  DlibConjugateGradientSearchStrategy(const Scalar wolfeRho,
                                      const Scalar wolfeSigma,
                                      const UnsignedInteger maxLineSearchIterations);

  // Virtual constructor
  virtual DlibConjugateGradientSearchStrategy * clone() const;

  /** ACCESSORS **/
  String getAlgorithmName() const;

  /** FUNCTION USED IN ACTUAL SEARCH **/
  const DlibMatrix get_next_direction ( const DlibMatrix& x,
                                        const double f_value,
                                        const DlibMatrix& funct_derivative);

private:


} ; /* class DlibConjugateGradientSearchStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DLIBCONJUGATEGRADIENTSEARCHSTRATEGY_HXX */
