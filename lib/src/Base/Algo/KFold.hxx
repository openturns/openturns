//                                               -*- C++ -*-
/**
 *  @brief K-Fold cross validation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_KFOLD_HXX
#define OPENTURNS_KFOLD_HXX

#include "ResourceMap.hxx"
#include "FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class KFold
 *
 * Creates all the possible training sets by removing P samples from the complete set
 */
class OT_API KFold
  : public FittingAlgorithmImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  explicit KFold(const UnsignedInteger k = ResourceMap::GetAsUnsignedInteger( "KFold-DefaultK" ),
                 const Bool useNormal = false);

  /** Virtual constructor */
  virtual KFold * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Perform cross-validation */
  virtual NumericalScalar run(const NumericalSample & x,
                              const NumericalSample & y,
                              const NumericalPoint & weight,
                              const Basis & basis,
                              const Indices & indices) const;
#ifndef SWIG
  virtual NumericalScalar run(const NumericalSample & y,
                              const NumericalPoint & weight,
                              const Indices & indices,
                              const DesignProxy & proxy) const;

  virtual NumericalScalar run(LeastSquaresMethod & method) const;
#endif

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** K accessor */
  void setK(const UnsignedInteger p);
  UnsignedInteger getK() const;

protected:

  /** The data is split into k subsamples */
  UnsignedInteger k_;

  /** Use the normal equation for the least square problems */
  Bool useNormal_;
}; /* class KFold */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KFOLD_HXX */
