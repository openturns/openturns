//                                               -*- C++ -*-
/**
 *  @brief The interface class that implements all process
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
#ifndef OPENTURNS_PROCESS_HXX
#define OPENTURNS_PROCESS_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/ProcessImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Process
 *
 * The class that implements all processs
 */
class OT_API Process
  : public TypedInterfaceObject<ProcessImplementation>
{
  CLASSNAME

public:

  /* Some typedefs for easy reading */
  typedef Pointer<ProcessImplementation> Implementation;

  /** Default constructor */
  Process();

  /** Copy constructors */
  Process(const ProcessImplementation & implementation);


  /** Constructor from implementation */
  Process(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  Process(ProcessImplementation * p_implementation);
#endif

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Is the underlying a Gaussian process ? */
  Bool isNormal() const;

  /** Is the underlying a stationary process ? */
  Bool isStationary() const;

  /** Is the underlying a composite process ? */
  Bool isComposite() const;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Trend accessor */
  TrendTransform getTrend() const;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** Discrete realization accessor */
  Field getRealization() const;

  /** Continuous realization accessor */
  Function getContinuousRealization() const;

  /** TimeGrid accessor */
  RegularGrid getTimeGrid() const;
  void setTimeGrid (const RegularGrid & timeGrid);

  /** Mesh accessor */
  Mesh getMesh() const;
  void setMesh (const Mesh & mesh);

  /** Process sample accessors */
  ProcessSample getSample(const UnsignedInteger size) const;

  /** Continuation of the last realization on a given number of steps */
  TimeSeries getFuture(const UnsignedInteger stepNumber) const;
  ProcessSample getFuture(const UnsignedInteger stepNumber,
                          const UnsignedInteger size) const;

  /** Get the process corresponding to the i-th marginal component */
  Process getMarginal(const UnsignedInteger i) const;

  /** Get the marginal process corresponding to indices components */
  Process getMarginal(const Indices & indices) const;


}; /* class Process */
END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMVECTOR_HXX */
