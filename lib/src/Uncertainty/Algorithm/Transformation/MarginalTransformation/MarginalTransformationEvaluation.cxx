//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
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
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalTransformationEvaluation)

static const Factory<MarginalTransformationEvaluation> Factory_MarginalTransformationEvaluation;


/* Default constructor */
MarginalTransformationEvaluation::MarginalTransformationEvaluation()
  : EvaluationImplementation()
  , inputDistributionCollection_()
  , outputDistributionCollection_()
  , parameterSide_(NONE)
  , tailThreshold_(ResourceMap::GetAsScalar( "MarginalTransformationEvaluation-DefaultTailThreshold" ))
{
  // Nothing to do
}

/* Parameter constructor */
MarginalTransformationEvaluation::MarginalTransformationEvaluation(const DistributionCollection & inputDistributionCollection,
    const DistributionCollection & outputDistributionCollection,
    const Bool simplify)
  : EvaluationImplementation()
  , inputDistributionCollection_(inputDistributionCollection)
  , outputDistributionCollection_(outputDistributionCollection)
  , parameterSide_(BOTH)
  , tailThreshold_(ResourceMap::GetAsScalar( "MarginalTransformationEvaluation-DefaultTailThreshold" ))
{
  initialize(simplify);
  // Populate the parameter vector and description with the input marginal
  // parameters followed by the output marginal parameters
  Point parameter(0);
  Description parameterDescription(0);
  const UnsignedInteger size = inputDistributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    parameter.add(inputDistributionCollection_[i].getParameter());
    parameterDescription.add(inputDistributionCollection_[i].getParameterDescription());
  }
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    parameter.add(outputDistributionCollection_[i].getParameter());
    parameterDescription.add(outputDistributionCollection_[i].getParameterDescription());
  }
  EvaluationImplementation::setParameter(parameter);
  EvaluationImplementation::setParameterDescription(parameterDescription);
}


void MarginalTransformationEvaluation::initialize(const Bool simplify)
{
  const UnsignedInteger size = inputDistributionCollection_.getSize();

  simplifications_ = Indices(size, 0);
  expressions_.resize(size);

  // Check that the collections of input and output distributions have the same size
  if (outputDistributionCollection_.getSize() != size) throw InvalidArgumentException(HERE) << "Error: a MarginalTransformationEvaluation cannot be built using collections of input and output distributions of different size";
  // First, check that the distributions are all 1D
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (inputDistributionCollection_[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: a MarginalTransformationEvaluation cannot be built using distributions with dimension > 1.";
    if (outputDistributionCollection_[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: a MarginalTransformationEvaluation cannot be built using distributions with dimension > 1.";
  }

  // Second, build the description of the transformation
  Description description;
  for (UnsignedInteger i = 0; i < size; ++i)
    description.add(inputDistributionCollection_[i].getDescription()[0]);
  for (UnsignedInteger i = 0; i < size; ++i)
    description.add(outputDistributionCollection_[i].getDescription()[0]);
  const auto iter = std::unique(description.begin(), description.end());
  if (iter < description.end())
  {
    description = Description::BuildDefault(size, "x");
    description.add(Description::BuildDefault(size, "y"));
  }
  setDescription(description);

  if (simplify)
  {
    const Description xNames(Description::BuildDefault(size, "x"));
    // Third, look for possible simplifications
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Distribution inputDistribution(inputDistributionCollection_[i]);
      const Distribution outputDistribution(outputDistributionCollection_[i]);
      const String xName(xNames[i]);
      const String inputClass(inputDistribution.getImplementation()->getClassName());
      const String outputClass(outputDistribution.getImplementation()->getClassName());
      const Point inputParameters(inputDistribution.getParametersCollection()[0]);
      const Point outputParameters(outputDistribution.getParametersCollection()[0]);
      // The first obvious simplification: the distributions share the same standard representative.
      if (inputClass == outputClass)
      {
        const Distribution inputStandardDistribution(inputDistribution.getStandardRepresentative());
        const Point inputStandardRepresentativeParameters(inputStandardDistribution.getParametersCollection()[0]);
        const Distribution outputStandardDistribution(outputDistribution.getStandardRepresentative());
        const Point outputStandardRepresentativeParameters(outputStandardDistribution.getParametersCollection()[0]);
        const Scalar difference = (inputStandardRepresentativeParameters - outputStandardRepresentativeParameters).norm();
        const Bool sameParameters = difference < ResourceMap::GetAsScalar("MarginalTransformationEvaluation-ParametersEpsilon");
        if (sameParameters)
        {
          // The transformation is the composition of two affine transformations: (input distribution->standard representative of input distribution) and (standard representative of output distribution->output distribution)
          // The two affine transformations are obtained using quantiles in order to deal with distributions with no moments
          const Scalar q25Input = inputDistribution.computeScalarQuantile(0.25);
          const Scalar q75Input = inputDistribution.computeScalarQuantile(0.75);
          const Scalar q25Output = outputDistribution.computeScalarQuantile(0.25);
          const Scalar q75Output = outputDistribution.computeScalarQuantile(0.75);
          const Scalar a = 0.5 * (q75Output + q25Output);
          // Here, b > 0 by construction
          const Scalar b = (q75Input == q25Input ? 1.0 : (q75Output - q25Output) / (q75Input - q25Input));
          const Scalar c = -0.5 * (q75Input + q25Input);
          OSS oss;
          oss.setPrecision(20);
          // First case, b = 1: we don't need to center the input variable
          if (b == 1.0)
          {
            const Scalar alpha = a + c;
            if (alpha != 0.0) oss << alpha << "+";
            oss << xName;
          } // b == 1
          else
          {
            if (a != 0.0) oss << a << "+";
            oss << b << "*";
            if (c != 0.0)
            {
              oss << "(" << xName;
              if (c > 0.0) oss << "+";
              if (c < 0.0) oss << "-";
              oss << std::abs(c) << ")";
            } // |c| != 0
            else oss << xName;
          } // b != 1
          const String formula(oss);
          expressions_[i] = SymbolicFunction(xName, formula);
          simplifications_[i] = 1;
        } // sameParameters
      } // inputClass == outputClass
      if (((inputClass == "ChiSquare") || (inputClass == "Exponential") || (inputClass == "Gamma")) &&
          ((outputClass == "ChiSquare") || (outputClass == "Exponential") || (outputClass == "Gamma")))
      {
        Scalar k1 = -1.0;
        Scalar lambda1 = -1.0;
        Scalar gamma1 = -1.0;
        if (inputClass == "ChiSquare")
        {
          k1 = 0.5 * inputParameters[0];
          lambda1 = 0.5 * inputParameters[0];
          gamma1 = 0.0;
        }
        else if (inputClass == "Exponential")
        {
          k1 = 1.0;
          lambda1 = inputParameters[0];
          gamma1 = inputParameters[1];
        }
        else
        {
          k1 = inputParameters[0];
          lambda1 = inputParameters[1];
          gamma1 = inputParameters[2];
        }
        Scalar k2 = -1.0;
        Scalar lambda2 = -1.0;
        Scalar gamma2 = -1.0;
        if (outputClass == "ChiSquare")
        {
          k2 = 0.5 * outputParameters[0];
          lambda2 = 0.5 * outputParameters[0];
          gamma2 = 0.0;
        }
        else if (outputClass == "Exponential")
        {
          k2 = 1.0;
          lambda2 = outputParameters[0];
          gamma2 = outputParameters[1];
        }
        else
        {
          k2 = outputParameters[0];
          lambda2 = outputParameters[1];
          gamma2 = outputParameters[2];
        }
        // There is a simplification only if k1 == k2
        if (k1 == k2)
        {
          OSS oss;
          oss.setPrecision(20);
          // T(x) = gamma2+lambda1/lambda2*(x-gamma1)
          oss << "max(0.0, ";
          if (gamma2 != 0.0)
            oss << gamma2 << " + ";
          if (lambda1 != lambda2)
            oss << lambda1 / lambda2 << " * ";
          if (gamma1 == 0.0) oss << xName << ")";
          else
          {
            oss << "(" << xName;
            if (gamma1 > 0.0) oss << " - " << gamma1 << "))";
            else oss << " + " << -gamma1 << "))";
          }
          const String formula(oss);
          expressions_[i] = SymbolicFunction(xName, formula);
          simplifications_[i] = 1;
        } // k1 == k2
      } // ChiSquare || Exponential || Gamma vs ChiSquare || Exponential || Gamma
      // Normal -> LogNormal simplification
      if ((inputClass == "Normal") && (outputClass == "LogNormal"))
      {
        const Scalar mu1 = inputParameters[0];
        const Scalar sigma1 = inputParameters[1];
        const Scalar muLog2 = outputParameters[0];
        const Scalar sigmaLog2 = outputParameters[1];
        const Scalar gamma2 = outputParameters[2];
        OSS oss;
        oss.setPrecision(20);
        if (gamma2 != 0.0) oss << gamma2 << " + ";
        oss << "exp(";
        if (muLog2 != 0.0) oss << muLog2 << " + ";
        if (sigmaLog2 != 1.0) oss << sigmaLog2 << " * ";
        if (mu1 != 0.0) oss << "(" << xName << " - " << mu1 << ")";
        else oss << xName;
        if (sigma1 != 1.0) oss << " / " << sigma1;
        oss << ")";
        const String formula(oss);
        expressions_[i] = SymbolicFunction(xName, formula);
        simplifications_[i] = 1;
      } // Normal -> LogNormal simplification
      // LogNormal -> Normal simplification
      if ((inputClass == "LogNormal") && (outputClass == "Normal"))
      {
        const Scalar muLog1 = inputParameters[0];
        const Scalar sigmaLog1 = inputParameters[1];
        const Scalar gamma1 = inputParameters[2];
        const Scalar mu2 = outputParameters[0];
        const Scalar sigma2 = outputParameters[1];
        OSS oss;
        oss.setPrecision(20);
        if (mu2 != 0.0) oss << mu2 << " + ";
        if (sigma2 != 1.0) oss << sigma2 << " * ";
        if (muLog1 != 0.0) oss << "(";
        oss << "log(max(" << SpecFunc::MinScalar << ", " << xName;
        if (gamma1 != 0.0) oss << " - " << gamma1;
        oss << "))";
        if (muLog1 != 0.0) oss << " - " << muLog1 << ")";
        if (sigmaLog1 != 1.0) oss << " / " << sigmaLog1;
        const String formula(oss);
        expressions_[i] = SymbolicFunction(xName, formula);
        simplifications_[i] = 1;
      } // LogNormal -> Normal simplification
    } // Loop over the components
  } // Simplifications
}

/* Parameter constructor */
MarginalTransformationEvaluation::MarginalTransformationEvaluation(const DistributionCollection & distributionCollection,
    const UnsignedInteger direction,
    const Distribution & standardMarginal)
  : EvaluationImplementation()
  , parameterSide_(NONE)
{
  if (standardMarginal.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: the standard marginal should be 1-d";
  const DistributionCollection standardCollection(distributionCollection.getSize(), standardMarginal);
  switch (direction)
  {
    case FROM:
      // From the given marginals to the standard ones
      *this = MarginalTransformationEvaluation(distributionCollection, standardCollection);
      parameterSide_ = LEFT;
      break;
    case TO:
      // From standard marginals to the given ones
      *this = MarginalTransformationEvaluation(standardCollection, distributionCollection);
      parameterSide_ = RIGHT;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: wrong value given for direction";
  }
  // Get all the parameters
  // The notion of parameters is used only for transformation from or to a standard space, so we have to extract the parameters of either the input distributions or the output distribution depending on the direction
  const UnsignedInteger size = distributionCollection.getSize();
  Point parameter(0);
  Description parameterDescription(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    parameter.add(distributionCollection[i].getParameter());
    parameterDescription.add(distributionCollection[i].getParameterDescription());
  } // get all the parameters
  setParameter(parameter);
  setParameterDescription(parameterDescription);
}

/* Virtual constructor */
MarginalTransformationEvaluation * MarginalTransformationEvaluation::clone() const
{
  return new MarginalTransformationEvaluation(*this);
}
/* Comparison operator */
Bool MarginalTransformationEvaluation::operator ==(const MarginalTransformationEvaluation & other) const
{
  if (this == &other) return true;
  return (inputDistributionCollection_ == other.inputDistributionCollection_) && (outputDistributionCollection_ == other.outputDistributionCollection_);
}
Bool MarginalTransformationEvaluation::equals(const EvaluationImplementation & other) const
{
  const MarginalTransformationEvaluation * p_other = dynamic_cast<const MarginalTransformationEvaluation *>(&other);
  return p_other && (*this == *p_other);
}

/* Evaluation */
Point MarginalTransformationEvaluation::operator () (const Point & inP) const
{
  const UnsignedInteger dimension = getOutputDimension();
  Point result(dimension);
  // The marginal transformation apply G^{-1} o F to each component of the input, where F is the ith input CDF and G the ith output CDf
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (simplifications_[i]) result[i] = expressions_[i](Point(1, inP[i]))[0];
    else
    {
      Scalar inputCDF = inputDistributionCollection_[i].computeCDF(inP[i]);
      // For accuracy reason, check if we are in the upper tail of the distribution
      const Bool upperTail = (inputCDF > tailThreshold_);
      if (upperTail) inputCDF = inputDistributionCollection_[i].computeComplementaryCDF(inP[i]);
      // The upper tail CDF is defined by CDF(x, upper) = P(X>x)
      // The upper tail quantile is defined by Quantile(CDF(x, upper), upper) = x
      // Avoid to create useless Point
      const Scalar q = outputDistributionCollection_[i].getImplementation()->computeScalarQuantile(inputCDF, upperTail);
      result[i] = q;
    }
  }
  callsNumber_.increment();
  return result;
}

/* Gradient according to the marginal parameters.
 *
 * F is the CDF of the ith marginal input distribution
 * Q is the quantile function of the ith output distribution
 *
 * F : RxRs -> [0,1]
 *    (x, pf) -> F(x, pf)
 *
 * Q : [0,1]xRt -> R
 *    (y, pq) -> G(y, pq)
 *
 * Let p = [pf, pq] be the parameter vector of H, with:
 *
 * H : RmxRt+s -> Rn
 *     (x, p)  -> Q(F(x, pf), pq)
 *
 * We have:
 *
 * (dH/dp)(x, p) = [(dQ/dy)(F(x, pf), pq) . (dF/dpf)(x, pf), 0] + [0, (dQ/dpq)(F(x, pf), pq)]
 *
 * The computation of (dQ/dy) leads to:
 *
 * (dQ/dy)(y, pq) = 1 / (pdf(Q(y, pq)))
 *
 * where pdf is the PDF of the distribution associated with the quantile function Q.
 *
 * The computation of (dQ/dpq) leads to:
 *
 * (dQ/dpq)(y, pq) = -(dcdf/dpq)(Q(y, pq), pq) / pdf(Q(y, pq), pq)
 *
 * where dcdf/dpq is the gradient according to its parameters of the CDF of the distribution
 * associated with the quantile function Q.
 *
 * the needed gradient is [(dH/dp)(x,p)]^t
 *
 */
Matrix MarginalTransformationEvaluation::parameterGradient(const Point & inP) const
{
  const Point parameters(getParameter());
  const UnsignedInteger parametersDimension = parameters.getDimension();
  const UnsignedInteger inputDimension = getInputDimension();
  Matrix result(parametersDimension, inputDimension);
  if (parametersDimension > 0)
  {
    UnsignedInteger rowIndex = 0;
    switch (parameterSide_)
    {
      case LEFT:
        /*
        * Here, we suppose that pq is empty, so dQ/dpq = 0
        * For each row, store (dF/dpf)(x, pf) / pdf(Q(F(x, pf)))
        */
        for (UnsignedInteger j = 0; j < inputDimension; ++j)
        {
          const Point x(1, inP[j]);
          const Distribution inputMarginal(inputDistributionCollection_[j]);
          const Distribution outputMarginal(outputDistributionCollection_[j]);
          // Here it is useless to call computeScalarQuantile() as the result will
          // be promoted into a Point in computePDF()
          const Scalar denominator = outputMarginal.computePDF(outputMarginal.computeQuantile(inputMarginal.computeCDF(x)));
          if (denominator > 0.0)
          {
            const Point normalizedCDFGradient(inputMarginal.computeCDFGradient(x) * (1.0 / denominator));
            const UnsignedInteger marginalParametersDimension = normalizedCDFGradient.getDimension();
            for (UnsignedInteger i = 0; i < marginalParametersDimension; ++i)
            {
              result(rowIndex, j) = normalizedCDFGradient[i];
              ++ rowIndex;
            }
          }
        } // FROM
        break;
      case RIGHT:
        /*
        * Here, we suppose that pf is empty, so dF/dpf = 0
        * For each row, store -(dcdf/dpq)(Q(F(x), pq), pq) / pdf(Q(F(x), pq), pq)
        */
        for (UnsignedInteger j = 0; j < inputDimension; ++j)
        {
          const Point x(1, inP[j]);
          const Distribution inputMarginal(inputDistributionCollection_[j]);
          const Distribution outputMarginal(outputDistributionCollection_[j]);
          // Here it is useless to call computeScalarQuantile() as the result will
          // be promoted into a Point in computePDF()
          const Point q(outputMarginal.computeQuantile(inputMarginal.computeCDF(x)));
          const Scalar denominator = outputMarginal.computePDF(q);
          if (denominator > 0.0)
          {
            const Point normalizedCDFGradient(outputMarginal.computeCDFGradient(q) * (-1.0 / denominator));
            const UnsignedInteger marginalParametersDimension = normalizedCDFGradient.getDimension();
            for (UnsignedInteger i = 0; i < marginalParametersDimension; ++i)
            {
              result(rowIndex, j) = normalizedCDFGradient[i];
              ++ rowIndex;
            }
          }
        } // TO
        break;
      case BOTH:
        /*
        * Combine the LEFT and RIGHT formulas: the rows associated with the
        * input marginal parameters store (dF/dpf)(x, pf) / pdf(Q(F(x, pf))),
        * the rows associated with the output marginal parameters store
        * -(dcdf/dpq)(Q(F(x, pf), pq), pq) / pdf(Q(F(x, pf), pq))
        */
        {
          UnsignedInteger inputParametersDimension = 0;
          for (UnsignedInteger j = 0; j < inputDimension; ++j)
            inputParametersDimension += inputDistributionCollection_[j].getParameter().getDimension();
          UnsignedInteger rowInput = 0;
          UnsignedInteger rowOutput = inputParametersDimension;
          for (UnsignedInteger j = 0; j < inputDimension; ++j)
          {
            const Point x(1, inP[j]);
            const Distribution inputMarginal(inputDistributionCollection_[j]);
            const Distribution outputMarginal(outputDistributionCollection_[j]);
            const Point q(outputMarginal.computeQuantile(inputMarginal.computeCDF(x)));
            const Scalar denominator = outputMarginal.computePDF(q);
            const UnsignedInteger inputMarginalParametersDimension = inputMarginal.getParameter().getDimension();
            const UnsignedInteger outputMarginalParametersDimension = outputMarginal.getParameter().getDimension();
            if (denominator > 0.0)
            {
              const Point leftGradient(inputMarginal.computeCDFGradient(x) * (1.0 / denominator));
              for (UnsignedInteger i = 0; i < leftGradient.getDimension(); ++i)
              {
                result(rowInput, j) = leftGradient[i];
                ++rowInput;
              }
              const Point rightGradient(outputMarginal.computeCDFGradient(q) * (-1.0 / denominator));
              for (UnsignedInteger i = 0; i < rightGradient.getDimension(); ++i)
              {
                result(rowOutput, j) = rightGradient[i];
                ++rowOutput;
              }
            }
            else
            {
              rowInput += inputMarginalParametersDimension;
              rowOutput += outputMarginalParametersDimension;
            }
          } // BOTH
        }
        break;
      default:
        // Should never go there
        throw NotYetImplementedException(HERE) << "In MarginalTransformationEvaluation::parameterGradient(const Point & inP)";
    }
  }
  return result;
}


Point MarginalTransformationEvaluation::getParameter() const
{
  Point parameter(0);
  if ((parameterSide_ == LEFT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger size = inputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      parameter.add(inputDistributionCollection_[i].getParameter());
  }
  if ((parameterSide_ == RIGHT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger size = outputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      parameter.add(outputDistributionCollection_[i].getParameter());
  }
  return parameter;
}


void MarginalTransformationEvaluation::setParameter(const Point & parameter)
{
  UnsignedInteger inputParametersDimension = 0;
  if ((parameterSide_ == LEFT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger size = inputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      inputParametersDimension += inputDistributionCollection_[i].getParameter().getDimension();
  }
  UnsignedInteger outputParametersDimension = 0;
  if ((parameterSide_ == RIGHT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger size = outputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      outputParametersDimension += outputDistributionCollection_[i].getParameter().getDimension();
  }
  if (parameter.getDimension() != inputParametersDimension + outputParametersDimension) throw InvalidArgumentException(HERE) << "Required parameter of dimension " << inputParametersDimension + outputParametersDimension << ", provided " << parameter.getDimension();
  // Keep track of the active simplifications in order to rebuild consistent expressions
  Bool simplify = false;
  const UnsignedInteger size = inputDistributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (simplifications_[i])
    {
      simplify = true;
      break;
    }
  UnsignedInteger index = 0;
  if ((parameterSide_ == LEFT) || (parameterSide_ == BOTH))
  {
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger marginalParametersDimension = inputDistributionCollection_[i].getParameter().getDimension();
      Point marginalParameter(marginalParametersDimension);
      std::copy(parameter.begin() + index, parameter.begin() + index + marginalParametersDimension, marginalParameter.begin());
      inputDistributionCollection_[i].setParameter(marginalParameter);
      index += marginalParametersDimension;
    }
  }
  if ((parameterSide_ == RIGHT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger outputSize = outputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < outputSize; ++i)
    {
      const UnsignedInteger marginalParametersDimension = outputDistributionCollection_[i].getParameter().getDimension();
      Point marginalParameter(marginalParametersDimension);
      std::copy(parameter.begin() + index, parameter.begin() + index + marginalParametersDimension, marginalParameter.begin());
      outputDistributionCollection_[i].setParameter(marginalParameter);
      index += marginalParametersDimension;
    }
  }
  EvaluationImplementation::setParameter(parameter);
  initialize(simplify);
}


/* Parameters description accessor */
Description MarginalTransformationEvaluation::getParameterDescription() const
{
  // Use the externally set descriptions if any, e.g. the prefixed names set
  // through JointDistribution, else expose the marginal descriptions
  if (parameterDescription_.getSize() > 0) return parameterDescription_;
  Description description(0);
  if ((parameterSide_ == LEFT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger size = inputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      description.add(inputDistributionCollection_[i].getParameterDescription());
  }
  if ((parameterSide_ == RIGHT) || (parameterSide_ == BOTH))
  {
    const UnsignedInteger size = outputDistributionCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      description.add(outputDistributionCollection_[i].getParameterDescription());
  }
  return description;
}


void MarginalTransformationEvaluation::setParameterDescription(const Description & description)
{
  EvaluationImplementation::setParameterDescription(description);
}


/* Accessor for input point dimension */
UnsignedInteger MarginalTransformationEvaluation::getInputDimension() const
{
  return inputDistributionCollection_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalTransformationEvaluation::getOutputDimension() const
{
  return inputDistributionCollection_.getSize();
}

/* Is it safe to call in parallel? */
Bool MarginalTransformationEvaluation::isParallel() const
{
  for (UnsignedInteger i = 0; i < inputDistributionCollection_.getSize(); ++ i)
  {
    if (simplifications_[i])
    {
      if (!expressions_[i].getImplementation()->isParallel())
        return false;
    }
    else
    {
      if (!inputDistributionCollection_[i].getImplementation()->isParallel())
        return false;
      if (!outputDistributionCollection_[i].getImplementation()->isParallel())
        return false;
    }
  }
  return true;
}

/* Input distribution collection accessor */
void MarginalTransformationEvaluation::setInputDistributionCollection(const DistributionCollection & inputDistributionCollection)
{
  inputDistributionCollection_ = inputDistributionCollection;
}

MarginalTransformationEvaluation::DistributionCollection MarginalTransformationEvaluation::getInputDistributionCollection() const
{
  return inputDistributionCollection_;
}

/* Output distribution collection accessor */
void MarginalTransformationEvaluation::setOutputDistributionCollection(const DistributionCollection & outputDistributionCollection)
{
  outputDistributionCollection_ = outputDistributionCollection;
}

MarginalTransformationEvaluation::DistributionCollection MarginalTransformationEvaluation::getOutputDistributionCollection() const
{
  return outputDistributionCollection_;
}

/* Simplifications accessor */
Collection<UnsignedInteger> MarginalTransformationEvaluation::getSimplifications() const
{
  return simplifications_;
}

/* expressions accessor */
Collection<Function> MarginalTransformationEvaluation::getExpressions() const
{
  return expressions_;
}

/* String converter */
String MarginalTransformationEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MarginalTransformationEvaluation::GetClassName()
      << " description=" << getDescription()
      << " input marginals=" << inputDistributionCollection_
      << " output marginals=" << outputDistributionCollection_
      << " simplifications=" << simplifications_
      << " expressions=" << expressions_.__repr__();
  return oss;
}

String MarginalTransformationEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  const String name(getName());
  if (hasVisibleName()) oss << "Marginal transformation " << getName() << " :" << "\n" << offset;
  const Description outputDescription(getOutputDescription());
  UnsignedInteger length = 0;
  for (UnsignedInteger i = 0; i < inputDistributionCollection_.getSize(); ++i)
  {
    const UnsignedInteger l = outputDescription[i].length();
    if (l > length) length = l;
  }
  for (UnsignedInteger i = 0; i < inputDistributionCollection_.getSize(); ++i)
  {
    if (inputDistributionCollection_.getSize() > 1) oss << "| " << std::setw(length) << outputDescription[i] << " = ";
    if (simplifications_[i]) oss << expressions_[i].getEvaluation().__str__() << "\n" << offset;
    else oss << inputDistributionCollection_[i] << " -> " << outputDescription[i] << " : " << outputDistributionCollection_[i] << "\n" << offset;
  }
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MarginalTransformationEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "inputDistributionCollection_", inputDistributionCollection_ );
  adv.saveAttribute( "outputDistributionCollection_", outputDistributionCollection_ );
  adv.saveAttribute( "parameterSide_", parameterSide_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalTransformationEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "inputDistributionCollection_", inputDistributionCollection_ );
  adv.loadAttribute( "outputDistributionCollection_", outputDistributionCollection_ );
  adv.loadAttribute( "parameterSide_", parameterSide_ );
  const Bool simplify = ResourceMap::GetAsBool("MarginalTransformationEvaluation-Simplify");
  initialize(simplify);
}

END_NAMESPACE_OPENTURNS
