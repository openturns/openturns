//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm interface
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
#ifndef OPENTURNS_LEASTSQUARESMETHOD_HXX
#define OPENTURNS_LEASTSQUARESMETHOD_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Sample.hxx"
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
  CLASSNAME

public:

  typedef TypedInterfaceObject<LeastSquaresMethodImplementation>::Implementation  Implementation;
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  LeastSquaresMethod();

  /** Parameters constructor */
  LeastSquaresMethod (const DesignProxy & proxy,
                      const Point & weight,
                      const Indices & indices);

  /** Parameters constructor */
  LeastSquaresMethod (const DesignProxy & proxy,
                      const Indices & indices);

  /** Parameters constructor */
  explicit LeastSquaresMethod (const Matrix & matrix);

  /** Constructor from implementation */
  LeastSquaresMethod(const Implementation & p_implementation);

  /** Constructor from implementation */
  LeastSquaresMethod(const LeastSquaresMethodImplementation & implementation);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Input sample accessor */
  Sample getInputSample() const;

  /** Weight accessor */
  Point getWeight() const;

  /** Basis accessor */
  FunctionCollection getBasis() const;

  /** Current indices accessor */
  Indices getCurrentIndices() const;

  /** Initial indices accessor */
  Indices getInitialIndices() const;

  /** Solve least-squares problem, ie x=\argmin |Mx-b|^2 */
  Point solve(const Point & rhs);

  /** Solve normal equation M^T*M*x=M^T*b */
  Point solveNormal(const Point & rhs);

  /** Get the diagonal of H */
  Point getHDiag() const;
  SymmetricMatrix getH() const;

  /** Get the inverse of the Gram matrix */
  CovarianceMatrix getGramInverse() const;

  /** Get the diagonal of the Gram inverse */
  Point getGramInverseDiag() const;

  /** Get the trace of the Gram inverse */
  Scalar getGramInverseTrace() const;

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
                                  const Point & weight,
                                  const Indices & indices);

  static LeastSquaresMethod Build(const String name,
                                  const DesignProxy & proxy,
                                  const Indices & indices);

  static LeastSquaresMethod Build(const String name,
                                  const Matrix & matrix);
protected:

}; /* class LeastSquaresMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESMETHOD_HXX */
