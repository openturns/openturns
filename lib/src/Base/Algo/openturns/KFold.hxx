//                                               -*- C++ -*-
/**
 *  @brief K-Fold cross validation
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
#ifndef OPENTURNS_KFOLD_HXX
#define OPENTURNS_KFOLD_HXX

#include "openturns/ResourceMap.hxx"
#include "openturns/FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class KFold
 *
 * Creates all the possible training sets by removing P samples from the complete set
 */
class OT_API KFold
  : public FittingAlgorithmImplementation
{
  CLASSNAME
public:
  typedef FittingAlgorithmImplementation::FunctionCollection FunctionCollection;


  /** Default constructor */
  explicit KFold(const UnsignedInteger k = ResourceMap::GetAsUnsignedInteger( "KFold-DefaultK" ),
                 const Bool useNormal = false);

  /** Virtual constructor */
  KFold * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Perform cross-validation */
  Scalar run(const Sample & x,
             const Sample & y,
             const Point & weight,
             const FunctionCollection & basis,
             const Indices & indices) const override;
#ifndef SWIG
  Scalar run(const Sample & y,
             const Point & weight,
             const Indices & indices,
             const DesignProxy & proxy) const override;

  Scalar run(LeastSquaresMethod & method,
             const Sample & y) const override;
#endif

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

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
