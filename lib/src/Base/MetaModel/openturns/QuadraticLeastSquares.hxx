//                                               -*- C++ -*-
/**
 *  @brief Second order polynomial response surface by least square
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_QUADRATICLEASTSQUARES_HXX
#define OPENTURNS_QUADRATICLEASTSQUARES_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/MetaModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class QuadraticLeastSquares
 *
 * The class describes the concept of quadratic response surface
 */
class OT_API QuadraticLeastSquares
  : public PersistentObject
{
  CLASSNAME
public:
  /** Default constructor */
  QuadraticLeastSquares();

  /** Constructor with parameters */
  QuadraticLeastSquares(const Sample & dataIn,
                        const Sample & dataOut);


  /** Virtual constructor */
  QuadraticLeastSquares * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Response surface computation */
  void run();

  /** Result accessor */
  MetaModelResult getResult() const;
  
  /** Constant accessor */
  Point getConstant() const;

  /** Linear accessor */
  Matrix getLinear() const;

  /** Quadratic accessor */
  SymmetricTensor getQuadratic() const;

  /** @deprecated Metamodel accessor */
  Function getMetaModel() const;

  /** DataIn accessor */
  Sample getInputSample() const;
  Sample getDataIn() const;

  /** DataOut accessor */
  Sample getOutputSample() const;
  void setDataOut(const Sample & dataOut);
  Sample getDataOut();

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
protected:

private:
  Sample dataIn_;
  Sample dataOut_;
  MetaModelResult result_;
  Point constant_;
  Matrix linear_;
  SymmetricTensor quadratic_;

}; /* class QuadraticLeastSquares */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICLEASTSQUARES_HXX */
