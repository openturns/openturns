//                                               -*- C++ -*-
/**
 *  @brief Linear combination of scalar covariance models
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
#include "openturns/LinearCombinationCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/DiracCovarianceModel.hxx"
#include "openturns/OSS.hxx"

#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearCombinationCovarianceModel)

static const Factory<LinearCombinationCovarianceModel> Factory_LinearCombinationCovarianceModel;

/* Default constructor */
LinearCombinationCovarianceModel::LinearCombinationCovarianceModel()
  : CovarianceModelImplementation(1)
  , atoms_(0)
  , coefficients_(0)
{
  AbsoluteExponential atom(1);
  atom.setNuggetFactor(0.0);
  CovarianceModelCollection atoms(1, atom);
  Point coefficients(1, 1.0);
  setAtomsAndCoefficients(atoms, coefficients, 1, true);
}

/* Collection constructor */
LinearCombinationCovarianceModel::LinearCombinationCovarianceModel(
  const CovarianceModelCollection & atoms,
  const Point & coefficients,
  const UnsignedInteger outputDimension)
  : CovarianceModelImplementation()
  , atoms_(0)
  , coefficients_(0)
{
  setAtomsAndCoefficients(atoms, coefficients, outputDimension, true);
}

/* Append a model with a variance weight, flattening nested combinations,
 * normalizing legacy atoms and folding identical atoms */
void LinearCombinationCovarianceModel::AppendAtom(
  CovarianceModelCollection & atoms,
  Point & coefficients,
  const CovarianceModel & model,
  const Scalar weight,
  const UnsignedInteger inputDimension)
{
  if (!std::isfinite(weight) || (weight < 0.0))
    throw InvalidArgumentException(HERE) << "The variance weight must be finite and nonnegative, got " << weight;
  if (model.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Incompatible input dimension of the atom, got "
                                         << model.getInputDimension() << ", expected " << inputDimension;
  // Flatten a scalar-output combination
  const LinearCombinationCovarianceModel * p_combination =
    dynamic_cast<const LinearCombinationCovarianceModel *>(model.getImplementation().get());
  if (p_combination != nullptr)
  {
    if (p_combination->getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "A multivariate combination cannot be used as an atom, got output dimension "
                                           << p_combination->getOutputDimension();
    const CovarianceModelCollection innerAtoms(p_combination->getAtoms());
    const Point innerCoefficients(p_combination->getCoefficients());
    for (UnsignedInteger k = 0; k < innerAtoms.getSize(); ++k)
      AppendAtom(atoms, coefficients, innerAtoms[k], weight * innerCoefficients[k], inputDimension);
    return;
  }
  if (model.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "An atom must have output dimension 1, got " << model.getOutputDimension();

  // Normalize a legacy atom: move its amplitude and nugget into the
  // coefficient, and add a unit Dirac atom for the nugget.
  CovarianceModel normalized(model.getImplementation()->clone());
  Scalar variance = 1.0;
  Scalar noiseVariance = 0.0;
  Bool normalizedBase = false;
  Bool amplitudeNormalized = false;
  try
  {
    const Point amplitude(model.getAmplitude());
    if (amplitude.getDimension() != 1)
      throw InvalidArgumentException(HERE) << "A scalar atom must have a scalar amplitude, got dimension " << amplitude.getDimension();
    if (!(amplitude[0] > 0.0) || !std::isfinite(amplitude[0]))
      throw InvalidArgumentException(HERE) << "The atom amplitude must be finite and positive, got " << amplitude[0];
    variance = amplitude[0] * amplitude[0];
    if (!std::isfinite(variance))
      throw InvalidArgumentException(HERE) << "The atom variance is not finite, got " << variance;
    normalized.setAmplitude(Point(1, 1.0));
    amplitudeNormalized = true;
    normalizedBase = true;
  }
  catch (const NotDefinedException &) {}
  catch (const NotYetImplementedException &) {}
  if (amplitudeNormalized)
  {
    try
    {
      const Scalar nuggetFactor(model.getNuggetFactor());
      if (!(nuggetFactor >= 0.0) || !std::isfinite(nuggetFactor))
        throw InvalidArgumentException(HERE) << "The atom nugget factor must be finite and nonnegative, got " << nuggetFactor;
      normalized.setNuggetFactor(0.0);
      noiseVariance = variance * nuggetFactor;
      if (!std::isfinite(noiseVariance))
        throw InvalidArgumentException(HERE) << "The atom nugget variance is not finite, got " << noiseVariance;
      normalizedBase = true;
    }
    catch (const NotDefinedException &) {}
    catch (const NotYetImplementedException &) {}
  }
  if (normalizedBase)
  {
    // Remove the generic nugget and amplitude entries from the atom active
    // set: the combination owns the variance from now on.
    const Indices active(normalized.getActiveParameter());
    Indices filtered(0);
    for (UnsignedInteger j = 0; j < active.getSize(); ++j)
      if ((active[j] != inputDimension) && (active[j] != inputDimension + 1))
        filtered.add(active[j]);
    normalized.setActiveParameter(filtered);
  }

  const Scalar smoothWeight = weight * variance;
  if (!std::isfinite(smoothWeight))
    throw InvalidArgumentException(HERE) << "The weighted atom variance is not finite, got " << smoothWeight;
  Bool merged = false;
  for (UnsignedInteger k = 0; k < atoms.getSize(); ++k)
  {
    // Several equals() implementations rely on a dynamic_cast: reject
    // different dynamic types before comparing to avoid a bad_cast.
    if (atoms[k].getImplementation()->getClassName() != normalized.getImplementation()->getClassName())
      continue;
    try
    {
      if (atoms[k] == normalized)
      {
        const Scalar mergedCoefficient = coefficients[k] + smoothWeight;
        if (!std::isfinite(mergedCoefficient))
          throw InvalidArgumentException(HERE) << "The folded atom variance is not finite, got " << mergedCoefficient;
        coefficients[k] = mergedCoefficient;
        merged = true;
        break;
      }
    }
    catch (const NotYetImplementedException &) {}
  }
  if (!merged)
  {
    atoms.add(normalized);
    coefficients.add(smoothWeight);
  }

  if (noiseVariance > 0.0)
  {
    DiracCovarianceModel dirac(inputDimension);
    dirac.setAmplitude(Point(1, 1.0));
    dirac.setNuggetFactor(0.0);
    dirac.setActiveParameter(Indices(0));
    const Scalar noiseWeight = weight * noiseVariance;
    if (!std::isfinite(noiseWeight))
      throw InvalidArgumentException(HERE) << "The weighted atom nugget variance is not finite, got " << noiseWeight;
    Bool noiseMerged = false;
    for (UnsignedInteger k = 0; k < atoms.getSize(); ++k)
    {
      if (atoms[k].getImplementation()->getClassName() != dirac.getClassName())
        continue;
      try
      {
        if (atoms[k] == dirac)
        {
          const Scalar mergedCoefficient = coefficients[k] + noiseWeight;
          if (!std::isfinite(mergedCoefficient))
            throw InvalidArgumentException(HERE) << "The folded atom nugget variance is not finite, got " << mergedCoefficient;
          coefficients[k] = mergedCoefficient;
          noiseMerged = true;
          break;
        }
      }
      catch (const NotYetImplementedException &) {}
    }
    if (!noiseMerged)
    {
      atoms.add(dirac);
      coefficients.add(noiseWeight);
    }
  }
}

/* Initialize the model from atoms and coefficients */
void LinearCombinationCovarianceModel::setAtomsAndCoefficients(
  const CovarianceModelCollection & atoms,
  const Point & coefficients,
  const UnsignedInteger outputDimension,
  const Bool resetActive)
{
  const UnsignedInteger size = atoms.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The atom collection must not be empty";
  // The coefficient argument may alias coefficients_ (e.g. through setAtoms),
  // so keep a copy before clearing the members.
  const Point inputCoefficients(coefficients);
  if (inputCoefficients.getDimension() != size)
    throw InvalidArgumentException(HERE) << "The coefficient vector has dimension " << inputCoefficients.getDimension()
                                         << ", expected " << size;
  if (outputDimension == 0)
    throw InvalidArgumentException(HERE) << "The output dimension must be positive";
  const UnsignedInteger inputDimension = atoms[0].getInputDimension();
  if (inputDimension == 0)
    throw InvalidArgumentException(HERE) << "The input dimension must be positive";

  // Build the normalized and folded atoms in local containers so that a
  // failure leaves the current model unchanged.
  CovarianceModelPersistentCollection normalizedAtoms(0);
  Point normalizedCoefficients(0);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    if (!std::isfinite(inputCoefficients[k]) || (inputCoefficients[k] < 0.0))
      throw InvalidArgumentException(HERE) << "The coefficient #" << k << " must be finite and nonnegative, got " << inputCoefficients[k];
    AppendAtom(normalizedAtoms, normalizedCoefficients, atoms[k], inputCoefficients[k], inputDimension);
  }
  atoms_ = normalizedAtoms;
  coefficients_ = normalizedCoefficients;
  hasNugget_ = false;
  nuggetIndex_ = 0;

  inputDimension_ = inputDimension;
  outputDimension_ = outputDimension;
  scale_ = Point(inputDimension, 1.0);
  nuggetFactor_ = 0.0;
  outputCorrelation_ = CorrelationMatrix();
  isDiagonal_ = true;
  if (resetActive) initializeActiveParameter();
  updateBaseState();
}

/* Atoms accessor */
LinearCombinationCovarianceModel::CovarianceModelCollection
LinearCombinationCovarianceModel::getAtoms() const
{
  return atoms_;
}

void LinearCombinationCovarianceModel::setAtoms(const CovarianceModelCollection & atoms)
{
  const CorrelationMatrix correlation(getOutputCorrelation());
  setAtomsAndCoefficients(atoms, coefficients_, outputDimension_, true);
  // setAtomsAndCoefficients resets the output correlation: restore it.
  if (outputDimension_ > 1)
  {
    setOutputCorrelation(correlation);
    updateBaseState();
  }
}

/* Coefficients accessor */
Point LinearCombinationCovarianceModel::getCoefficients() const
{
  return coefficients_;
}

void LinearCombinationCovarianceModel::setCoefficients(const Point & coefficients)
{
  if (coefficients.getDimension() != atoms_.getSize())
    throw InvalidArgumentException(HERE) << "The coefficient vector has dimension " << coefficients.getDimension()
                                         << ", expected " << atoms_.getSize();
  for (UnsignedInteger k = 0; k < coefficients.getDimension(); ++k)
    if (!std::isfinite(coefficients[k]) || (coefficients[k] < 0.0))
      throw InvalidArgumentException(HERE) << "The coefficient #" << k << " must be finite and nonnegative, got " << coefficients[k];
  coefficients_ = coefficients;
  updateBaseState();
}

Point LinearCombinationCovarianceModel::getAmplitudes() const
{
  Point amplitudes(atoms_.getSize());
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    amplitudes[k] = std::sqrt(std::max(0.0, coefficients_[k]));
  return amplitudes;
}

void LinearCombinationCovarianceModel::setAmplitudes(const Point & amplitudes)
{
  if (amplitudes.getDimension() != atoms_.getSize())
    throw InvalidArgumentException(HERE) << "The amplitude vector has dimension " << amplitudes.getDimension()
                                         << ", expected " << atoms_.getSize();
  Point coefficients(atoms_.getSize());
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
  {
    if (!std::isfinite(amplitudes[k]) || (amplitudes[k] < 0.0))
      throw InvalidArgumentException(HERE) << "The amplitude #" << k << " must be finite and nonnegative, got " << amplitudes[k];
    coefficients[k] = amplitudes[k] * amplitudes[k];
    if (!std::isfinite(coefficients[k]))
      throw InvalidArgumentException(HERE) << "The variance weight #" << k << " is not finite, got " << coefficients[k];
  }
  coefficients_ = coefficients;
  updateBaseState();
}

/* Virtual constructor */
LinearCombinationCovarianceModel * LinearCombinationCovarianceModel::clone() const
{
  return new LinearCombinationCovarianceModel(*this);
}

/* Comparison operators */
Bool LinearCombinationCovarianceModel::operator ==(const LinearCombinationCovarianceModel & other) const
{
  if (this == &other) return true;
  if (!hasEqualBase(other)) return false;
  if (hasNugget_ != other.hasNugget_) return false;
  if (hasNugget_ && (nuggetIndex_ != other.nuggetIndex_)) return false;
  if (atoms_.getSize() != other.atoms_.getSize()) return false;
  if (coefficients_ != other.coefficients_) return false;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
  {
    if (atoms_[k].getImplementation()->getClassName() != other.atoms_[k].getImplementation()->getClassName())
      return false;
    try
    {
      if (!(atoms_[k] == other.atoms_[k])) return false;
    }
    catch (const NotYetImplementedException &)
    {
      return false;
    }
  }
  return true;
}

Bool LinearCombinationCovarianceModel::equals(const CovarianceModelImplementation & other) const
{
  const LinearCombinationCovarianceModel * p_other = dynamic_cast<const LinearCombinationCovarianceModel *>(&other);
  return p_other && (*this == *p_other);
}

/* Evaluation */
SquareMatrix LinearCombinationCovarianceModel::operator()(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  Scalar value = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    value += coefficients_[k] * atoms_[k].computeAsScalar(s, t);
  SquareMatrix result(outputDimension_);
  const CorrelationMatrix correlation(getOutputCorrelation());
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    for (UnsignedInteger i = 0; i < outputDimension_; ++i)
      result(i, j) = value * correlation(i, j);
  return result;
}

SquareMatrix LinearCombinationCovarianceModel::operator()(const Point & tau) const
{
  if (!isStationary_)
    return CovarianceModelImplementation::operator()(tau);
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The lag has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  Scalar value = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    value += coefficients_[k] * atoms_[k].computeAsScalar(tau);
  SquareMatrix result(outputDimension_);
  const CorrelationMatrix correlation(getOutputCorrelation());
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    for (UnsignedInteger i = 0; i < outputDimension_; ++i)
      result(i, j) = value * correlation(i, j);
  return result;
}

Scalar LinearCombinationCovarianceModel::computeAsScalar(const Point & s,
    const Point & t) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result += coefficients_[k] * atoms_[k].computeAsScalar(s, t);
  return result;
}

Scalar LinearCombinationCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result += coefficients_[k] * atoms_[k].computeAsScalar(tau);
  return result;
}

Scalar LinearCombinationCovarianceModel::computeAsScalar(
  const Collection<Scalar>::const_iterator & s_begin,
  const Collection<Scalar>::const_iterator & t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result += coefficients_[k] * atoms_[k].getImplementation()->computeAsScalar(s_begin, t_begin);
  return result;
}

Scalar LinearCombinationCovarianceModel::computeAsScalar(const Scalar * s,
    const Scalar * t) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  if (!isStationary_)
  {
    Point sPoint(inputDimension_);
    Point tPoint(inputDimension_);
    std::copy(s, s + inputDimension_, sPoint.begin());
    std::copy(t, t + inputDimension_, tPoint.begin());
    return computeAsScalar(sPoint, tPoint);
  }
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result += coefficients_[k] * atoms_[k].getImplementation()->computeAsScalar(s, t);
  return result;
}

Scalar LinearCombinationCovarianceModel::computeAsScalar(const Scalar s,
    const Scalar t) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "The covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "The covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result += coefficients_[k] * atoms_[k].computeAsScalar(s, t);
  return result;
}

Scalar LinearCombinationCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "The covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "The covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result += coefficients_[k] * atoms_[k].computeAsScalar(tau);
  return result;
}

/* Gradient with respect to the input */
Matrix LinearCombinationCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  Matrix scalarGradient(inputDimension_, 1);
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    scalarGradient = scalarGradient + coefficients_[k] * atoms_[k].partialGradient(s, t);
  if (outputDimension_ == 1) return scalarGradient;
  Matrix result(inputDimension_, outputDimension_ * outputDimension_);
  const CorrelationMatrix correlation(getOutputCorrelation());
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    for (UnsignedInteger i = 0; i < outputDimension_; ++i)
      for (UnsignedInteger a = 0; a < inputDimension_; ++a)
        result(a, i + outputDimension_ * j) = scalarGradient(a, 0) * correlation(i, j);
  return result;
}

/* Gradient with respect to the parameters */
Matrix LinearCombinationCovarianceModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "The point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const UnsignedInteger atomCount = atoms_.getSize();
  const UnsignedInteger columns = (outputDimension_ == 1) ? 1 : outputDimension_ * outputDimension_;
  Matrix result(activeParameter_.getSize(), columns);
  Indices atomStart(atomCount + 1);
  atomStart[0] = atomCount;
  for (UnsignedInteger k = 0; k < atomCount; ++k)
    atomStart[k + 1] = atomStart[k] + atoms_[k].getFullParameter().getSize();
  const UnsignedInteger correlationStart = atomStart[atomCount];
  const CorrelationMatrix correlation(getOutputCorrelation());
  Point atomValues(atomCount);
  Scalar value = 0.0;
  for (UnsignedInteger k = 0; k < atomCount; ++k)
  {
    atomValues[k] = atoms_[k].computeAsScalar(s, t);
    value += coefficients_[k] * atomValues[k];
  }
  // Each atom gradient is computed once, on the first active row that
  // falls into the atom, then reused for its remaining rows.
  Collection<Matrix> atomGradients(atomCount);
  Collection<Indices> atomActives(atomCount);
  Indices atomGradientComputed(atomCount);
  for (UnsignedInteger r = 0; r < activeParameter_.getSize(); ++r)
  {
    const UnsignedInteger global = activeParameter_[r];
    if (global < atomCount)
    {
      // Derivative with respect to a variance weight
      const Scalar atomValue = atomValues[global];
      if (outputDimension_ == 1)
        result(r, 0) = atomValue;
      else
        for (UnsignedInteger j = 0; j < outputDimension_; ++j)
          for (UnsignedInteger i = 0; i < outputDimension_; ++i)
            result(r, i + outputDimension_ * j) = atomValue * correlation(i, j);
    }
    else if (global < correlationStart)
    {
      // Derivative with respect to an atom parameter
      UnsignedInteger k = 0;
      while ((k + 1 < atomCount) && (global >= atomStart[k + 1])) ++k;
      const UnsignedInteger local = global - atomStart[k];
      if (atomGradientComputed[k] == 0)
      {
        atomGradients[k] = atoms_[k].parameterGradient(s, t);
        atomActives[k] = atoms_[k].getActiveParameter();
        atomGradientComputed[k] = 1;
      }
      const Matrix & atomGradient = atomGradients[k];
      const Indices & atomActive = atomActives[k];
      UnsignedInteger localRow = atomActive.getSize();
      for (UnsignedInteger j = 0; j < atomActive.getSize(); ++j)
        if (atomActive[j] == local)
        {
          localRow = j;
          break;
        }
      if (!(localRow < atomGradient.getNbRows()))
        throw InternalException(HERE) << "In LinearCombinationCovarianceModel::parameterGradient: inconsistent active parameters";
      if (outputDimension_ == 1)
        result(r, 0) = coefficients_[k] * atomGradient(localRow, 0);
      else
        for (UnsignedInteger j = 0; j < outputDimension_; ++j)
          for (UnsignedInteger i = 0; i < outputDimension_; ++i)
            result(r, i + outputDimension_ * j) = coefficients_[k] * atomGradient(localRow, 0) * correlation(i, j);
    }
    else
    {
      // Derivative with respect to an output correlation entry
      UnsignedInteger index = 0;
      UnsignedInteger row = 0;
      UnsignedInteger col = 0;
      Bool found = false;
      for (UnsignedInteger i = 0; i < outputDimension_ && !found; ++i)
        for (UnsignedInteger j = 0; j < i; ++j)
        {
          if (index == global - correlationStart)
          {
            row = i;
            col = j;
            found = true;
            break;
          }
          ++index;
        }
      if (!found)
        throw InternalException(HERE) << "In LinearCombinationCovarianceModel::parameterGradient: inconsistent correlation parameter";
      result(r, row + outputDimension_ * col) = value;
      result(r, col + outputDimension_ * row) = value;
    }
  }
  return result;
}

/* Generic scale accessor: no unique value at the combination level */
Point LinearCombinationCovarianceModel::getScale() const
{
  throw NotDefinedException(HERE) << "In LinearCombinationCovarianceModel::getScale: no unique scale at the combination level, use getAtoms() or getFullParameter() instead.";
}

void LinearCombinationCovarianceModel::setScale(const Point &)
{
  throw NotDefinedException(HERE) << "In LinearCombinationCovarianceModel::setScale: no unique scale at the combination level, use getAtoms() or setFullParameter() instead.";
}

/* Amplitude accessor: total smooth standard deviation */
Point LinearCombinationCovarianceModel::getAmplitude() const
{
  const Scalar variance = smoothVariance();
  return Point(outputDimension_, std::sqrt(std::max(0.0, variance)));
}

void LinearCombinationCovarianceModel::setAmplitude(const Point & amplitude)
{
  if ((amplitude.getDimension() != 1) && (amplitude.getDimension() != outputDimension_))
    throw InvalidArgumentException(HERE) << "The amplitude vector has dimension " << amplitude.getDimension()
                                         << ", expected 1 or " << outputDimension_;
  for (UnsignedInteger i = 0; i < amplitude.getDimension(); ++i)
    if (!std::isfinite(amplitude[i]) || (amplitude[i] < 0.0))
      throw InvalidArgumentException(HERE) << "The amplitude must be finite and nonnegative, got " << amplitude[i];
  const Scalar target = amplitude[0] * amplitude[0];
  if (!std::isfinite(target))
    throw InvalidArgumentException(HERE) << "The amplitude variance is not finite, got " << target;
  for (UnsignedInteger i = 1; i < amplitude.getDimension(); ++i)
    if (amplitude[i] != amplitude[0])
      throw InvalidArgumentException(HERE) << "The combination has equal output variances: all amplitude components must be equal";
  const Scalar current = smoothVariance();
  if (!(current > 0.0))
    throw InvalidArgumentException(HERE) << "Cannot set the amplitude of a combination with no smooth atom";
  const Scalar ratio = target / current;
  if (!std::isfinite(ratio))
    throw InvalidArgumentException(HERE) << "The amplitude ratio is not finite, got " << ratio;
  const Scalar factor = getNuggetFactor();
  Point coefficients(coefficients_);
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    if (!(hasNugget_ && (k == nuggetIndex_)))
    {
      coefficients[k] *= ratio;
      if (!std::isfinite(coefficients[k]))
        throw InvalidArgumentException(HERE) << "The scaled variance weight #" << k << " is not finite, got " << coefficients[k];
    }
  if (hasNugget_)
  {
    coefficients[nuggetIndex_] = factor * target;
    if (!std::isfinite(coefficients[nuggetIndex_]))
      throw InvalidArgumentException(HERE) << "The managed nugget variance is not finite, got " << coefficients[nuggetIndex_];
  }
  coefficients_ = coefficients;
  updateBaseState();
}

/* Managed nugget accessor */
Scalar LinearCombinationCovarianceModel::getNuggetFactor() const
{
  if (!hasNugget_) return 0.0;
  const Scalar variance = smoothVariance();
  if (!(variance > 0.0)) return 0.0;
  return coefficients_[nuggetIndex_] / variance;
}

void LinearCombinationCovarianceModel::setNuggetFactor(const Scalar nuggetFactor)
{
  if (!std::isfinite(nuggetFactor) || (nuggetFactor < 0.0))
    throw InvalidArgumentException(HERE) << "The nugget factor must be finite and nonnegative, got " << nuggetFactor;
  const Scalar variance = smoothVariance();
  const Scalar target = nuggetFactor * variance;
  if (!std::isfinite(target))
    throw InvalidArgumentException(HERE) << "The nugget variance is not finite, got " << target;
  if (!hasNugget_)
  {
    const UnsignedInteger oldAtomCount = atoms_.getSize();
    UnsignedInteger correlationStart = oldAtomCount;
    for (UnsignedInteger k = 0; k < oldAtomCount; ++k)
      correlationStart += atoms_[k].getFullParameter().getSize();
    DiracCovarianceModel dirac(inputDimension_);
    dirac.setAmplitude(Point(1, 1.0));
    dirac.setNuggetFactor(0.0);
    dirac.setActiveParameter(Indices(0));
    const UnsignedInteger insertedSize = CovarianceModel(dirac).getFullParameter().getSize();
    atoms_.add(dirac);
    coefficients_.add(target);
    nuggetIndex_ = atoms_.getSize() - 1;
    hasNugget_ = true;
    // Appending an atom adds one coefficient and one atom block before the
    // output correlation block: shift the existing active indices.
    Indices active(activeParameter_);
    for (UnsignedInteger j = 0; j < active.getSize(); ++j)
    {
      if (active[j] >= correlationStart) active[j] += 1 + insertedSize;
      else if (active[j] >= oldAtomCount) active[j] += 1;
    }
    activeParameter_ = active;
  }
  else
    coefficients_[nuggetIndex_] = target;
  updateBaseState();
}

Scalar LinearCombinationCovarianceModel::getNuggetVariance() const
{
  return hasNugget_ ? coefficients_[nuggetIndex_] : 0.0;
}

void LinearCombinationCovarianceModel::setNuggetVariance(const Scalar nuggetVariance)
{
  if (!std::isfinite(nuggetVariance) || (nuggetVariance < 0.0))
    throw InvalidArgumentException(HERE) << "The nugget variance must be finite and nonnegative, got " << nuggetVariance;
  if (!hasNugget_)
  {
    const UnsignedInteger oldAtomCount = atoms_.getSize();
    UnsignedInteger correlationStart = oldAtomCount;
    for (UnsignedInteger k = 0; k < oldAtomCount; ++k)
      correlationStart += atoms_[k].getFullParameter().getSize();
    DiracCovarianceModel dirac(inputDimension_);
    dirac.setAmplitude(Point(1, 1.0));
    dirac.setNuggetFactor(0.0);
    dirac.setActiveParameter(Indices(0));
    const UnsignedInteger insertedSize = CovarianceModel(dirac).getFullParameter().getSize();
    atoms_.add(dirac);
    coefficients_.add(nuggetVariance);
    nuggetIndex_ = atoms_.getSize() - 1;
    hasNugget_ = true;
    Indices active(activeParameter_);
    for (UnsignedInteger j = 0; j < active.getSize(); ++j)
    {
      if (active[j] >= correlationStart) active[j] += 1 + insertedSize;
      else if (active[j] >= oldAtomCount) active[j] += 1;
    }
    activeParameter_ = active;
  }
  else
    coefficients_[nuggetIndex_] = nuggetVariance;
  updateBaseState();
}

void LinearCombinationCovarianceModel::activateNuggetFactor(const Bool isNuggetFactorActive)
{
  if (!hasNugget_)
  {
    if (!isNuggetFactorActive) return;
    setNuggetFactor(0.0);
  }
  Indices active(activeParameter_);
  Indices::iterator position = std::find(active.begin(), active.end(), nuggetIndex_);
  if (isNuggetFactorActive)
  {
    if (position == active.end()) active.add(nuggetIndex_);
  }
  else if (position != active.end())
    active.erase(position);
  std::sort(active.begin(), active.end());
  setActiveParameter(active);
}

/* Marginal accessors */
CovarianceModel LinearCombinationCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (!(index < outputDimension_))
    throw InvalidArgumentException(HERE) << "The index=" << index << " must be less than the output dimension=" << outputDimension_;
  LinearCombinationCovarianceModel result(*this);
  result.outputDimension_ = 1;
  result.isDiagonal_ = true;
  result.outputCorrelation_ = CorrelationMatrix();
  result.initializeActiveParameter();
  result.updateBaseState();
  return result;
}

CovarianceModel LinearCombinationCovarianceModel::getMarginal(const Indices & indices) const
{
  if (indices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "The marginal indices must not be empty";
  if (!indices.check(outputDimension_))
    throw InvalidArgumentException(HERE) << "The indices=" << indices << " must be less than the output dimension=" << outputDimension_;
  LinearCombinationCovarianceModel result(*this);
  const UnsignedInteger size = indices.getSize();
  result.outputDimension_ = size;
  CorrelationMatrix correlation(size);
  const CorrelationMatrix full(getOutputCorrelation());
  for (UnsignedInteger j = 0; j < size; ++j)
    for (UnsignedInteger i = 0; i < size; ++i)
      correlation(i, j) = full(indices[i], indices[j]);
  result.isDiagonal_ = correlation.isDiagonal();
  result.outputCorrelation_ = result.isDiagonal_ ? CorrelationMatrix() : correlation;
  result.initializeActiveParameter();
  result.updateBaseState();
  return result;
}

/* Flags */
Bool LinearCombinationCovarianceModel::isStationary() const
{
  return isStationary_;
}

Bool LinearCombinationCovarianceModel::isDiagonal() const
{
  return isDiagonal_;
}

Bool LinearCombinationCovarianceModel::isParallel() const
{
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    if (!atoms_[k].getImplementation()->isParallel()) return false;
  return true;
}

/* String converters */
String LinearCombinationCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << LinearCombinationCovarianceModel::GetClassName()
      << " input dimension=" << inputDimension_
      << " output dimension=" << outputDimension_
      << " atoms=" << atoms_
      << " coefficients=" << coefficients_;
  if (hasNugget_) oss << " nugget index=" << nuggetIndex_;
  return oss;
}

String LinearCombinationCovarianceModel::__str__(const String &) const
{
  return __repr__();
}

/* Full parameter: coefficients, atom blocks and output correlation */
Point LinearCombinationCovarianceModel::getFullParameter() const
{
  Point result(coefficients_);
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    result.add(atoms_[k].getFullParameter());
  const CorrelationMatrix correlation(getOutputCorrelation());
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      result.add(correlation(i, j));
  return result;
}

void LinearCombinationCovarianceModel::setFullParameter(const Point & parameter)
{
  const UnsignedInteger atomCount = atoms_.getSize();
  UnsignedInteger expectedSize = atomCount;
  for (UnsignedInteger k = 0; k < atomCount; ++k)
    expectedSize += atoms_[k].getFullParameter().getSize();
  expectedSize += outputDimension_ * (outputDimension_ - 1) / 2;
  if (parameter.getDimension() != expectedSize)
    throw InvalidArgumentException(HERE) << "The parameter has dimension " << parameter.getDimension()
                                         << ", expected " << expectedSize;

  Point coefficients(atomCount);
  for (UnsignedInteger k = 0; k < atomCount; ++k)
  {
    if (!std::isfinite(parameter[k]) || (parameter[k] < 0.0))
      throw InvalidArgumentException(HERE) << "The coefficient #" << k << " must be finite and nonnegative, got " << parameter[k];
    coefficients[k] = parameter[k];
  }
  coefficients_ = coefficients;

  UnsignedInteger offset = atomCount;
  for (UnsignedInteger k = 0; k < atomCount; ++k)
  {
    const UnsignedInteger localSize = atoms_[k].getFullParameter().getSize();
    Point localParameter(localSize);
    std::copy(parameter.begin() + offset, parameter.begin() + offset + localSize, localParameter.begin());
    atoms_[k].setFullParameter(localParameter);
    offset += localSize;
  }

  CorrelationMatrix correlation(outputDimension_);
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      correlation(i, j) = parameter[offset++];
  setOutputCorrelation(correlation);
  updateBaseState();
}

Description LinearCombinationCovarianceModel::getFullParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    description.add(OSS() << "coefficient_" << k);
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
  {
    const Description localDescription(atoms_[k].getFullParameterDescription());
    for (UnsignedInteger j = 0; j < localDescription.getSize(); ++j)
      description.add(OSS() << "atom_" << k << "_" << localDescription[j]);
  }
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      description.add(OSS() << "R_" << i << "_" << j);
  return description;
}

void LinearCombinationCovarianceModel::setActiveParameter(const Indices & active)
{
  const UnsignedInteger atomCount = atoms_.getSize();
  UnsignedInteger totalSize = atomCount;
  for (UnsignedInteger k = 0; k < atomCount; ++k)
    totalSize += atoms_[k].getFullParameter().getSize();
  totalSize += outputDimension_ * (outputDimension_ - 1) / 2;
  Indices sorted(active);
  std::sort(sorted.begin(), sorted.end());
  const Indices::const_iterator last = std::unique(sorted.begin(), sorted.end());
  if (last != sorted.end())
    throw InvalidArgumentException(HERE) << "Active parameter indices must be unique";
  for (UnsignedInteger j = 0; j < active.getSize(); ++j)
    if (!(active[j] < totalSize))
      throw InvalidArgumentException(HERE) << "Active parameter index " << active[j] << " is out of range [0, " << totalSize << ")";
  activeParameter_ = active;

  UnsignedInteger offset = atomCount;
  for (UnsignedInteger k = 0; k < atomCount; ++k)
  {
    const UnsignedInteger localSize = atoms_[k].getFullParameter().getSize();
    Indices localActive(0);
    for (UnsignedInteger j = 0; j < active.getSize(); ++j)
      if ((active[j] >= offset) && (active[j] < offset + localSize))
        localActive.add(active[j] - offset);
    atoms_[k].setActiveParameter(localActive);
    offset += localSize;
  }
}

/* Persistence */
void LinearCombinationCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("atoms_", atoms_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("nuggetIndex_", nuggetIndex_);
  adv.saveAttribute("hasNugget_", hasNugget_);
}

void LinearCombinationCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("atoms_", atoms_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("nuggetIndex_", nuggetIndex_);
  adv.loadAttribute("hasNugget_", hasNugget_);
  if ((atoms_.getSize() == 0) || (coefficients_.getDimension() != atoms_.getSize()))
    throw InvalidArgumentException(HERE) << "Inconsistent linear combination persistence";
  for (UnsignedInteger k = 0; k < coefficients_.getDimension(); ++k)
    if (!std::isfinite(coefficients_[k]) || (coefficients_[k] < 0.0))
      throw InvalidArgumentException(HERE) << "Inconsistent variance weight #" << k << " in linear combination persistence";
  if (hasNugget_ && !(nuggetIndex_ < atoms_.getSize()))
    throw InvalidArgumentException(HERE) << "Inconsistent managed nugget index";
  updateBaseState();
}

/* Build the sum of two scalar models */
CovarianceModel LinearCombinationCovarianceModel::Add(const CovarianceModel & left,
    const CovarianceModel & right)
{
  if ((left.getOutputDimension() != 1) || (right.getOutputDimension() != 1))
    throw InvalidArgumentException(HERE) << "The covariance algebra is defined for scalar-output models, got output dimensions "
                                         << left.getOutputDimension() << " and " << right.getOutputDimension();
  if (left.getInputDimension() != right.getInputDimension())
    throw InvalidArgumentException(HERE) << "The input dimensions are incompatible: "
                                         << left.getInputDimension() << " and " << right.getInputDimension();
  CovarianceModelCollection atoms(0);
  Point coefficients(0);
  AppendAtom(atoms, coefficients, left, 1.0, left.getInputDimension());
  AppendAtom(atoms, coefficients, right, 1.0, right.getInputDimension());
  return LinearCombinationCovarianceModel(atoms, coefficients, 1);
}

/* Multiply a scalar model or a combination by a positive factor */
CovarianceModel LinearCombinationCovarianceModel::Scale(const CovarianceModel & model,
    const Scalar factor)
{
  if (!std::isfinite(factor) || !(factor > 0.0))
    throw InvalidArgumentException(HERE) << "The scale factor must be finite and positive, got " << factor;
  if (factor == 1.0) return model;
  const LinearCombinationCovarianceModel * p_combination =
    dynamic_cast<const LinearCombinationCovarianceModel *>(model.getImplementation().get());
  if (p_combination != nullptr)
  {
    LinearCombinationCovarianceModel result(*p_combination);
    Point coefficients(result.getCoefficients());
    for (UnsignedInteger k = 0; k < coefficients.getDimension(); ++k)
    {
      coefficients[k] *= factor;
      if (!std::isfinite(coefficients[k]))
        throw InvalidArgumentException(HERE) << "The scaled variance weight #" << k << " is not finite, got " << coefficients[k];
    }
    result.setCoefficients(coefficients);
    return result;
  }
  if (model.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "The covariance algebra is defined for scalar-output models, got output dimension "
                                         << model.getOutputDimension();
  CovarianceModelCollection atoms(1, model);
  Point coefficients(1, factor);
  return LinearCombinationCovarianceModel(atoms, coefficients, 1);
}

/* Smooth variance: sum of the non-nugget coefficients at zero lag */
Scalar LinearCombinationCovarianceModel::smoothVariance() const
{
  Scalar result = 0.0;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    if (!(hasNugget_ && (k == nuggetIndex_)))
      result += coefficients_[k] * zeroLagVariance(atoms_[k]);
  return result;
}

Scalar LinearCombinationCovarianceModel::zeroLagVariance(const CovarianceModel & atom) const
{
  if (atom.isStationary())
    return atom.computeAsScalar(Point(inputDimension_));
  return atom.computeAsScalar(Point(inputDimension_), Point(inputDimension_));
}

void LinearCombinationCovarianceModel::updateBaseState()
{
  Scalar totalVariance = 0.0;
  Bool stationary = true;
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
  {
    const Scalar atomVariance = zeroLagVariance(atoms_[k]);
    if (!std::isfinite(atomVariance) || (atomVariance < 0.0))
      throw InvalidArgumentException(HERE) << "The atom zero-lag variance #" << k << " must be finite and nonnegative, got " << atomVariance;
    totalVariance += coefficients_[k] * atomVariance;
    if (!std::isfinite(totalVariance))
      throw InvalidArgumentException(HERE) << "The total zero-lag variance is not finite";
    if (!atoms_[k].isStationary()) stationary = false;
  }
  totalVariance = std::max(0.0, totalVariance);
  amplitude_ = Point(outputDimension_, std::sqrt(totalVariance));
  nuggetFactor_ = 0.0;
  isStationary_ = stationary;
  // Preserve the stored correlation when it is not diagonal.
  const CorrelationMatrix correlation(isDiagonal_ ? CorrelationMatrix(outputDimension_) : outputCorrelation_);
  isDiagonal_ = correlation.isDiagonal();
  updateOutputCovariance();
}

void LinearCombinationCovarianceModel::initializeActiveParameter()
{
  activeParameter_ = Indices(0);
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
    if (!(hasNugget_ && (k == nuggetIndex_)))
      activeParameter_.add(k);
  UnsignedInteger offset = atoms_.getSize();
  for (UnsignedInteger k = 0; k < atoms_.getSize(); ++k)
  {
    const Indices localActive(atoms_[k].getActiveParameter());
    for (UnsignedInteger j = 0; j < localActive.getSize(); ++j)
      activeParameter_.add(offset + localActive[j]);
    offset += atoms_[k].getFullParameter().getSize();
  }
  for (UnsignedInteger j = 0; j < outputDimension_ * (outputDimension_ - 1) / 2; ++j)
    activeParameter_.add(offset + j);
}

END_NAMESPACE_OPENTURNS
