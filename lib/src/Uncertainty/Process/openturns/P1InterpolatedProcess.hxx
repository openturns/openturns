//                                               -*- C++ -*-
/**
 *  @brief P1InterpolatedProcess implements a process whose realizations are obtained by P1 Lagrange interpolation of another process on a finer mesh.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_P1INTERPOLATEDPROCESS_HXX
#define OPENTURNS_P1INTERPOLATEDPROCESS_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/Process.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/FieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class P1InterpolatedProcess
 *
 * The implementation of the P1InterpolatedProcess class
 */
class OT_API P1InterpolatedProcess
  : public ProcessImplementation
{
  CLASSNAME

public:


  /** Default constructor */
  P1InterpolatedProcess();

  /** Standard constructor */
  P1InterpolatedProcess(const Process & compoundProcess,
                        const Mesh & mesh);

  /** Virtual constructor */
  P1InterpolatedProcess * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Realization accessor */
  Field getRealization() const override;

  /** Compound process accessor */
  Process getCompoundProcess() const;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The compound process (source of the realizations) */
  Process compoundProcess_;

  /** The P1 Lagrange interpolation function */
  FieldFunction p1Interpolation_;

}; /* class P1InterpolatedProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_P1INTERPOLATEDPROCESS_HXX */
