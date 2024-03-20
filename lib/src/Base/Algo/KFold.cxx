//                                               -*- C++ -*-
/**
 *  @brief K-Fold cross validation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/KFold.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KFold)


static const Factory<KFold> Factory_KFold;


/* Default constructor */
KFold::KFold( const UnsignedInteger k,
              const Bool )
  : FittingAlgorithmImplementation()
{
  setK( k );
}

/* Virtual constructor */
KFold * KFold::clone() const
{
  return new KFold( *this );
}

/* String converter */
String KFold::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Perform cross-validation */
Scalar KFold::run(const Sample & x,
                  const Sample & y,
                  const Point & weight,
                  const FunctionCollection & basis,
                  const Indices & indices) const
{
  return FittingAlgorithmImplementation::run(x, y, weight, basis, indices);
}

Scalar KFold::run(const Sample & y,
                  const Point & weight,
                  const Indices & indices,
                  const DesignProxy & proxy) const
{
  return FittingAlgorithmImplementation::run(y, weight, indices, proxy);
}

Scalar KFold::run(LeastSquaresMethod & method,
                  const Sample & y) const
{
  const Sample x(method.getInputSample());
  const FunctionCollection basis(method.getBasis());

  const UnsignedInteger sampleSize = x.getSize();
  const Scalar variance = y.computeVariance()[0];

  if (y.getDimension() != 1) throw InvalidArgumentException( HERE ) << "Output sample should be unidimensional (dim=" << y.getDimension() << ").";
  if (y.getSize() != sampleSize) throw InvalidArgumentException( HERE ) << "Samples should be equally sized (in=" << sampleSize << " out=" << y.getSize() << ").";
  if (!(k_ < sampleSize)) throw InvalidArgumentException( HERE ) << "K (" << k_ << ") should be < size (" << sampleSize << ").";

  // the size of a subsample
  const UnsignedInteger testSize = sampleSize / k_;
  Scalar quadraticResidual = 0.0;

  // Store the initial row filter if any
  const Indices initialRowFilter(method.getImplementation()->proxy_.getRowFilter());

  // We build the test sample by selecting one over k points of the given samples up to the test size, with a varying initial index
  // i is the initial index
  UnsignedInteger totalTestSize = 0;
  for (UnsignedInteger i = 0; i < k_; ++ i)
  {
    LOGINFO(OSS() << "Sub-sample " << i << " over " << k_ - 1);
    // build training/test samples
    Point yTest(0);
    Point rhs(0);
    Indices addedIndices(0);
    Indices conservedIndices(0);
    Indices removedIndices(0);
    Indices rowFilter(0);
    Indices inverseRowFilter(0);
    for (UnsignedInteger j = 0; j < (k_ * testSize); ++ j)
    {
      const UnsignedInteger jModK = j % k_;
      // If j is in the learning database
      if (jModK != i)
      {
        // If it is the first pass or if j was in the previous test database, it is a new index
        if ((i == 0) || (jModK == (i - 1))) addedIndices.add(j);
        // Else it is a conserved index
        else conservedIndices.add(j);
        rhs.add(y(j, 0));
        rowFilter.add(j);
      }
      else
      {
        // If it is not the first pass, the test points were in the learning database at the previous pass
        if (i > 0) removedIndices.add(j);
        yTest.add(y(j, 0));
        inverseRowFilter.add(j);
      }
    } // Partitioning loop

    // perform LS regression on the current basis
    LOGINFO("Solve current least-squares problem");
    method.getImplementation()->proxy_.setRowFilter(rowFilter);
    method.update(addedIndices, conservedIndices, removedIndices, true);
    const Point coefficients(method.solve(rhs));
    // evaluate on the test sample
    method.getImplementation()->proxy_.setRowFilter(inverseRowFilter);
    const Matrix psiAk(method.computeWeightedDesign());
    const Point yHatTest(psiAk * coefficients);
    LOGINFO("Compute the residual");

    // The empirical error is the normalized L2 error
    totalTestSize += yTest.getSize();
    quadraticResidual += (yTest - yHatTest).normSquare();

    LOGINFO(OSS() << "Cumulated residual=" << quadraticResidual);
  }
  // Restore the row filter
  method.getImplementation()->proxy_.setRowFilter(initialRowFilter);
  const Scalar empiricalError = quadraticResidual / totalTestSize;

  const Scalar relativeError = (!(variance > 0.0) ? 0.0 : empiricalError / variance);
  LOGINFO(OSS() << "Relative error=" << relativeError);
  return relativeError;
}


/* Method save() stores the object through the StorageManager */
void KFold::save(Advocate & adv) const
{
  FittingAlgorithmImplementation::save(adv);
  adv.saveAttribute( "k_", k_ );
}


/* Method load() reloads the object from the StorageManager */
void KFold::load(Advocate & adv)
{
  FittingAlgorithmImplementation::load(adv);
  adv.loadAttribute( "k_", k_ );
}


/* K accessor */
void KFold::setK(const UnsignedInteger k)
{
  if (! (k > 0) )
    throw InvalidArgumentException(HERE) << "KFold k parameter should be > 0, but is " << k;
  k_ = k;
}


UnsignedInteger KFold::getK() const
{
  return k_;
}


END_NAMESPACE_OPENTURNS
