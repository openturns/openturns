//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm interface
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
#ifndef OPENTURNS_LEASTSQUARESMETHOD_HXX
#define OPENTURNS_LEASTSQUARESMETHOD_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Basis.hxx"
#include "openturns/LeastSquaresMethodImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class LeastSquaresMethod
 *
 * LeastSquares solving algorithm interface
 */
class OT_API LeastSquaresMethod
  : public TypedInterfaceObject<LeastSquaresMethodImplementation>
{
  CLASSNAME;

public:

  typedef TypedInterfaceObject<LeastSquaresMethodImplementation>::Implementation  Implementation;

  /** Default constructor */
  LeastSquaresMethod();

  /** Parameters constructor */
  LeastSquaresMethod (const DesignProxy & proxy,
                      const NumericalPoint & weight,
                      const Indices & indices);

  /** Parameters constructor */
  LeastSquaresMethod (const DesignProxy & proxy,
                      const Indices & indices);

  /** Constructor from implementation */
  LeastSquaresMethod(const Implementation & p_implementation);

  /** Constructor from implementation */
  LeastSquaresMethod(const LeastSquaresMethodImplementation & implementation);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Input sample accessor */
  NumericalSample getInputSample() const;

  /** Weight accessor */
  NumericalPoint getWeight() const;

  /** Basis accessor */
  Basis getBasis() const;

  /** Current indices accessor */
  Indices getCurrentIndices() const;

  /** Initial indices accessor */
  Indices getInitialIndices() const;

  /** Build the current functions */
  Basis buildCurrentBasis() const;

  /** Solve least-squares problem, ie x=\argmin |Mx-b|^2 */
  NumericalPoint solve(const NumericalPoint & rhs);

  /** Solve normal equation M^T*M*x=M^T*b */
  NumericalPoint solveNormal(const NumericalPoint & rhs);

  /** Get the diagonal of H */
  NumericalPoint getHDiag() const;

  /** Get the diagonal of the Gram inverse */
  NumericalPoint getGramInverseDiag() const;

  /** Get the trace of the Gram inverse */
  NumericalScalar getGramInverseTrace() const;

  /** Update basis */
  void update(const Indices & addedIndices,
              const Indices & conservedIndices,
              const Indices & removedIndices,
              const Bool row = false);

  /** Build the Gram matrix */
  Matrix computeWeightedDesign(bool whole = false) const;

  /** Build a method instance from its name */
  static LeastSquaresMethod Build(const String name,
                                  const DesignProxy & proxy,
                                  const NumericalPoint & weight,
                                  const Indices & indices);

  static LeastSquaresMethod Build(const String name,
                                  const DesignProxy & proxy,
                                  const Indices & indices);
protected:

}; /* class LeastSquaresMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESMETHOD_HXX */
