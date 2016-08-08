//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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
#include "openturns/NonStationaryCovarianceModelFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonStationaryCovarianceModelFactory);
static const Factory<NonStationaryCovarianceModelFactory> Factory_NonStationaryCovarianceModelFactory;

typedef Collection<CovarianceMatrix>  CovarianceMatrixCollection;

/* Default constructor */
NonStationaryCovarianceModelFactory::NonStationaryCovarianceModelFactory()
  : CovarianceModelFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NonStationaryCovarianceModelFactory * NonStationaryCovarianceModelFactory::clone() const
{
  return new NonStationaryCovarianceModelFactory(*this);
}

/* String converter */
String NonStationaryCovarianceModelFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NonStationaryCovarianceModelFactory::GetClassName();
  return oss;
}

/* String converter */
String NonStationaryCovarianceModelFactory::__str__(const String & offset) const
{
  return __repr__();
}


CovarianceModelImplementation::Implementation NonStationaryCovarianceModelFactory::build(const ProcessSample & sample) const
{
  return buildAsUserDefinedCovarianceModel(sample).clone();
}


struct ComputeCovariancePolicy
{
  const ProcessSample & input_;
  CovarianceMatrixCollection & output_;
  Field mean_;
  UnsignedInteger dimension_;
  UnsignedInteger N_;
  UnsignedInteger size_;
  NumericalScalar alpha_;

  ComputeCovariancePolicy(const ProcessSample & input,
                          CovarianceMatrixCollection & output)
    : input_(input)
    , output_(output)
    , mean_()
    , dimension_(input.getDimension())
    , size_(input.getSize())
    , alpha_(1.0 / (size_ - 1.0))
  {
    mean_ = input.computeMean();
  }

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger i = static_cast< UnsignedInteger >(sqrt(2 * index + 0.25) - 0.5);
      const UnsignedInteger j = index - (i * (i + 1)) / 2;
      CovarianceMatrix & matrix(output_[index]);
      for (UnsignedInteger k = 0; k < dimension_; ++k)
      {
        const NumericalScalar muIK = mean_.getValueAtIndex(i)[k];
        for (UnsignedInteger l = 0; l <= k; ++l)
        {
          const NumericalScalar muJL = mean_.getValueAtIndex(j)[l];
          NumericalScalar coef = 0.0;
          for (UnsignedInteger sampleIndex = 0; sampleIndex < size_; ++sampleIndex)
          {
            coef += (input_[sampleIndex][i][k] - muIK)
                    * (input_[sampleIndex][j][l] - muJL);
          } // sampleIndex
          matrix(k, l) = coef * alpha_;
        } // l
      } // k
    } // index
  }

}; /* end struct ComputeCovariancePolicy */

UserDefinedCovarianceModel NonStationaryCovarianceModelFactory::buildAsUserDefinedCovarianceModel(const ProcessSample & sample) const
{
  const Mesh & mesh(sample.getMesh());
  const UnsignedInteger N = mesh.getVerticesNumber();
  // Create a collection of null CovarianceMatrix
  const UnsignedInteger size = (N * (N + 1)) / 2;
  const UnsignedInteger dimension = sample.getDimension();
  CovarianceMatrixCollection collection(size);
  for (UnsignedInteger i = 0; i < size; ++i) collection[i] = CovarianceMatrix(SquareMatrix(dimension).getImplementation());

  // Special case for a sample of size 1
  if (size == 0) return UserDefinedCovarianceModel(mesh, collection);

  const ComputeCovariancePolicy policy( sample, collection );
  TBB::ParallelFor( 0, size, policy );
  return UserDefinedCovarianceModel(mesh, collection);
}

/* Method save() stores the object through the StorageManager */
void NonStationaryCovarianceModelFactory::save(Advocate & adv) const
{
  CovarianceModelFactoryImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void NonStationaryCovarianceModelFactory::load(Advocate & adv)
{
  CovarianceModelFactoryImplementation::load(adv);
}
END_NAMESPACE_OPENTURNS
