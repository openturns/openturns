//                                               -*- C++ -*-
/**
 *  @brief An interface for all implementation class of process
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ARMAFACTORYIMPLEMENTATION_HXX
#define OPENTURNS_ARMAFACTORYIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/ARMA.hxx"
#include "openturns/TimeSeries.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ARMAFactoryImplementation
 *
 * An interface for all implementation class of process
 */
class OT_API ARMAFactoryImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Some typedefs to ease reading */
  typedef Pointer<ARMAFactoryImplementation>     Antecedent;
  typedef Antecedent                         Implementation;

  /** Default constructor */
  ARMAFactoryImplementation();

  /** Parameter constructor */
  ARMAFactoryImplementation(const UnsignedInteger p,
                            const UnsignedInteger q,
                            const Bool invertible = true);

  /** Parameter constructor */
  ARMAFactoryImplementation(const Indices & p,
                            const Indices & q,
                            const Bool invertible = true);

  /** Virtual constructor */
  ARMAFactoryImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** P accessor */
  Indices getP() const;
  UnsignedInteger getCurrentP() const;

  /** Q accessor */
  Indices getQ() const;
  UnsignedInteger getCurrentQ() const;

  /** Invertibility accessor */
  Bool getInvertible() const;
  void setInvertible(const Bool invertible);

  /** Build method */
  virtual ARMA build(const TimeSeries & timeSeries) const;

  /** Build method */
  virtual ARMA build(const ProcessSample & sample) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** P accessor */
  void setP(const Indices & ARIndices);
  void setCurrentP(const UnsignedInteger p) const;

  /** Q accessor */
  void setQ(const Indices & MAIndices);
  void setCurrentQ(const UnsignedInteger q) const;

  /** AR size */
  Indices p_;
  mutable UnsignedInteger currentP_;

  /** MA size */
  Indices q_;
  mutable UnsignedInteger currentQ_;

  /** Invertibility constraint */
  Bool invertible_;

}; /* class ARMAFactoryImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ARMAFACTORYIMPLEMENTATION_HXX */
