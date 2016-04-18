//                                               -*- C++ -*-
/**
 *  @brief The test file of class Study for standard methods
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
#include <iostream>
#include <sstream>

#include "openturns/OTconfig.hxx"
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

template<class T>
static inline
void compare(const T & savedInstance, const Study & study, const String & tag = String(""))
{
  OStream fullprint(std::cout);
  T loadedInstance;
  const String className = loadedInstance.getClassName();
  String label = tag;
  if (label.empty())
  {
    label = className;
    label[0] = tolower(label[0]);
  }
  study.fillObject(label, loadedInstance);
  fullprint << "saved  " << className << " = " << savedInstance  << std::endl;
  fullprint << "loaded " << className << " = " << loadedInstance << std::endl;
}



int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  const char fileName[] = "myStudy.xml";

  try
  {

    // Create a Study Object
    Study study;
    study.setStorageManager(XMLStorageManager(fileName));


    // Add a PersistentObject to the Study (here a NumericalPoint)
    NumericalPoint numericalPoint(3, 0.);
    numericalPoint[0] = 10.;
    numericalPoint[1] = 11.;
    numericalPoint[2] = 12.;
    numericalPoint.setName("point");
    study.add("Good", numericalPoint);

    // Add another PersistentObject to the Study (here a NumericalSample)
    NumericalSample numericalSample(3, 2);
    NumericalPoint p2(2, 0.);
    p2[0] = 100.;
    p2[1] = 200.;
    numericalSample[0] = p2;
    p2.setName("One");
    NumericalPoint p3(2, 0.);
    p3[0] = 101.;
    p3[1] = 201.;
    numericalSample[1] = p3;
    p3.setName("Two");
    NumericalPoint p4(2, 0.);
    p4[0] = 102.;
    p4[1] = 202.;
    numericalSample[2] = p4;
    p4.setName("Three");
    study.add("mySample", numericalSample);

    // Create a NumericalPoint that we will try to reinstaciate after reloading
    NumericalPoint namedNumericalPoint(2, 1000.);
    namedNumericalPoint.setName("point");
    study.add("namedNumericalPoint", namedNumericalPoint);

    // Add a point with a description
    NumericalPointWithDescription numericalPointWithDescription(numericalPoint);
    Description desc = numericalPointWithDescription.getDescription();
    desc[0] = "x";
    desc[1] = "y";
    desc[2] = "z";
    numericalPointWithDescription.setDescription(desc);
    study.add("pDesc", numericalPointWithDescription);

    // Add a matrix
    Matrix matrix(2, 3);
    matrix(0, 0) = 0;
    matrix(0, 1) = 1;
    matrix(0, 2) = 2;
    matrix(1, 0) = 3;
    matrix(1, 1) = 4;
    matrix(1, 2) = 5;
    study.add("m", matrix);


    // Add a Staircase
    Staircase staircase;
    {
      // Instanciate one distribution object
      UnsignedInteger dim(1);
      NumericalPoint meanPoint(dim, 1.0);
      meanPoint[0] = 0.5;
      NumericalPoint sigma(dim, 1.0);
      sigma[0] = 2.0;
      CorrelationMatrix R = IdentityMatrix(dim);
      Normal distribution1(meanPoint, sigma, R);

      // Instanciate another distribution object
      meanPoint[0] = -1.5;
      sigma[0] = 4.0;
      Normal distribution2(meanPoint, sigma, R);

      // Test for sampling
      UnsignedInteger size = 2000;
      UnsignedInteger nPoints = 20;
      NumericalSample sample1(distribution1.getSample( size ));
      NumericalSample sample2(distribution2.getSample( size ));

      // Construct empirical CDF for each sample
      NumericalSample data1(nPoints, 2), data2(nPoints, 2);
      NumericalPoint cursor1(2);
      NumericalPoint cursor2(2);
      NumericalScalar count1;
      NumericalScalar count2;

      for(UnsignedInteger i = 0; i < nPoints; i++)
      {
        cursor1[0] = 13.*i / nPoints - 6.5;
        count1 = 0;
        cursor2[0] = 24.*i / nPoints - 13.5;
        count2 = 0;

        for(UnsignedInteger j = 0; j < size; j++)
        {
          if(sample1[j][0] < cursor1[0]) count1++;
          if(sample2[j][0] < cursor2[0]) count2++;
        }

        cursor1[1] = count1 / size;
        cursor2[1] = count2 / size;

        data1[i] = cursor1;
        data2[i] = cursor2;
      }
      staircase = Staircase (data2, "red", "dashed", "S", "eCDF2, pat=S");
    }
    study.add("staircase", staircase);

    // Create a Simulation::Result
    SimulationResult simulationResult(Event(), 0.5, 0.01, 150, 4);
    simulationResult.setName("simulationResult");
    study.add("simulationResult", simulationResult);

    Arcsine arcsine(5.2, 11.6);
    study.add("arcsine", arcsine);

    Beta beta(3.0, 5.0, -1.0, 4.0);
    study.add("beta", beta);

    Chi chi(1.5);
    study.add("chi", chi);

    ChiSquare chiSquare(1.5);
    study.add("chiSquare", chiSquare);

    UnsignedInteger dim = 2;
    NumericalPoint theta(dim + 1);
    for (UnsignedInteger i = 0; i <= dim; i++) theta[i] = 1.0 + (i + 1.0) / 4.0;
    Dirichlet dirichlet(theta);
    study.add("dirichlet", dirichlet);

    Exponential exponential(3.0, -2.0);
    study.add("exponential", exponential);

    FisherSnedecor fisherSnedecor(5.5, 10.5);
    study.add("fisherSnedecor", fisherSnedecor);

    Gamma gamma(1.5, 3.0, -2.0);
    study.add("gamma", gamma);

    GeneralizedPareto generalizedPareto(1.5, -0.2);
    study.add("generalizedPareto", generalizedPareto);

    Geometric geometric(0.15);
    study.add("geometric", geometric);

    Gumbel gumbel(2.0, -0.5);
    study.add("gumbel", gumbel);

    IndependentCopula independentCopula(5);
    study.add("independentCopula", independentCopula);

    InverseNormal inverseNormal(0.5, 2.0);
    study.add("inverseNormal", inverseNormal);

    KernelSmoothing kernelSmoothing;
    kernelSmoothing.build(independentCopula.getSample(20));
    study.add("kernelSmoothing", kernelSmoothing);

    Laplace laplace(1.0 / 1.5, 0.5);
    study.add("laplace", laplace);

    Logistic logistic(0.5, 1.5);
    study.add("logistic", logistic);

    LogNormal logNormal(0.5, 1.5, -1.0);
    study.add("logNormal", logNormal);

    LogUniform logUniform(-0.5, 1.5);
    study.add("logUniform", logUniform);

    MeixnerDistribution meixnerDistribution(1.5, 0.5, 2.5, -0.5);
    study.add("meixnerDistribution", meixnerDistribution);

    Rayleigh rayleigh(2.5, -0.5);
    study.add("rayleigh", rayleigh);

    Rice rice(5.0, 4.0);
    study.add("rice", rice);

    Student student(5.0, -0.5, 2.0);
    study.add("student", student);

    Trapezoidal trapezoidal(1.0, 1.2, 3.0, 14.0);
    study.add("trapezoidal", trapezoidal);

    Triangular triangular(-0.5, 1.5, 2.5);
    study.add("triangular", triangular);

    TruncatedDistribution truncatedDistribution(Normal(2.0, 1.5), 1.0, 4.0);
    study.add("truncatedDistribution", truncatedDistribution);

    TruncatedNormal truncatedNormal(0.5, 3.0, -2.0, 2.0);
    study.add("truncatedNormal", truncatedNormal);

    // Create an UserDefined
    UserDefined userDefined;
    {
      UserDefined::UserDefinedPairCollection collection(3);
      NumericalPoint point(1);
      point[0] = 1.0;
      collection[0] = UserDefinedPair(point, 0.3);
      point[0] = 2.0;
      collection[1] = UserDefinedPair(point, 0.1);
      point[0] = 3.0;
      collection[2] = UserDefinedPair(point, 0.6);
      userDefined = UserDefined(collection);
    }
    study.add("userDefined", userDefined);

    // Create a Weibull distribution
    Weibull weibull(2.0, 1.5, -0.5);
    study.add("weibull", weibull);

    // Create a NormalCopula distribution
    CorrelationMatrix R(3);
    R(0, 1) = 0.5;
    R(1, 2) = 0.5;
    NormalCopula normalCopula(R);
    study.add("normalCopula", normalCopula);

    // Create a Uniform distribution
    Uniform uniform(-1.5, 2.0);
    study.add("uniform", uniform);

    // Create a ComposedDistribution
    ComposedDistribution::DistributionCollection collection;
    collection.add(beta);
    collection.add(gamma);
    collection.add(logistic);
    ComposedDistribution composedDistribution(collection, normalCopula);
    study.add("composedDistribution", composedDistribution);

    // Create an analytical NumericalMathFunction
    Description input(3);
    input[0] = "a";
    input[1] = "b";
    input[2] = "c";
    Description output(3);
    output[0] = "squaresum";
    output[1] = "prod";
    output[2] = "complex";
    Description formulas(output.getSize());
    formulas[0] = "a+b+c";
    formulas[1] = "a-b*c";
    formulas[2] = "(a+2*b^2+3*c^3)/6";
    NumericalMathFunction analytical(input, output, formulas);
    analytical.setName("analytical");
    study.add("analytical", analytical);

    // Create an Event Object
    Event event;
    {
      NumericalPoint point(3);
      point[0] = 101;
      point[1] = 202;
      point[2] = 303;
      RandomVector vect(point);
      RandomVector output(analytical.getMarginal(0), vect);
      event = Event (output, Less(), 50);
    }
    study.add("event", event);

    // Create a StandardEvent Object
    StandardEvent standardEvent(event);
    study.add("standardEvent", standardEvent);

    // Create a TaylorExpansionMoments algorithm
    TaylorExpansionMoments taylorExpansionMoments;
    {
      RandomVector antecedent(IndependentCopula(analytical.getInputDimension()));
      antecedent.setName("antecedent");
      RandomVector composite(analytical, antecedent);
      composite.setName("composite");
      taylorExpansionMoments = TaylorExpansionMoments(composite);
      taylorExpansionMoments.setName("taylorExpansionMoments");
      taylorExpansionMoments.getMeanFirstOrder();
      taylorExpansionMoments.getMeanSecondOrder();
      taylorExpansionMoments.getCovariance();
    }

    study.add("taylorExpansionMoments", taylorExpansionMoments);

    // Create an AbdoRackwitz algorithm
    AbdoRackwitz abdoRackwitz;
    abdoRackwitz.setMaximumIterationNumber(100);
    abdoRackwitz.setMaximumAbsoluteError(1.0e-10);
    abdoRackwitz.setMaximumRelativeError(1.0e-10);
    abdoRackwitz.setMaximumResidualError(1.0e-10);
    abdoRackwitz.setMaximumConstraintError(1.0e-10);
    study.add("abdoRackwitz", abdoRackwitz);

    // Create a SQP algorithm
    SQP sqp;
    sqp.setMaximumIterationNumber(100);
    sqp.setMaximumAbsoluteError(1.0e-10);
    sqp.setMaximumRelativeError(1.0e-10);
    sqp.setMaximumResidualError(1.0e-10);
    sqp.setMaximumConstraintError(1.0e-10);
    study.add("sqp", sqp);

    // Create a Cobyla algorithm
    Cobyla cobyla;
    study.add("cobyla", cobyla);
    cobyla.setMaximumIterationNumber(100);
    cobyla.setMaximumAbsoluteError(1.0e-10);
    cobyla.setMaximumRelativeError(1.0e-10);
    cobyla.setMaximumResidualError(1.0e-10);
    cobyla.setMaximumConstraintError(1.0e-10);

    // Create a TNC algorithm
    OptimizationSolver tnc(new TNC());
    {
      Interval bounds(NumericalPoint(3, -3.0), NumericalPoint(3, 5.0));
      Description input2(3);
      input2[0] = "x";
      input2[1] = "y";
      input2[2] = "z";
      Description output2(1);
      output2[0] = "d";
      Description formula2(output2.getSize());
      formula2[0] = "(x+2*y^2+3*z^3)/6";
      NumericalMathFunction model(input2, output2, formula2);
      model.setName("complex");

      OptimizationProblem problem;
      problem.setBounds(bounds);
      problem.setObjective(model);
      problem.setMinimization(true);

      tnc.setProblem(problem);
      tnc.setStartingPoint(NumericalPoint(3, 1.0));
    }
    study.add("tnc", tnc);

    // Create a SORM algorithm
    SORM sorm(abdoRackwitz, event, NumericalPoint(3, 4.));
    study.add("sorm", sorm);

    // Create a FORMResult
    FORMResult formResult;
    // Create a SORMResult
    SORMResult sormResult;
    {
      Description input2(2);
      input2[0] = "x";
      input2[1] = "y";
      Description output2(1);
      output2[0] = "d";
      Description formula2(1);
      formula2[0] = "y^2-x";
      NumericalMathFunction model(input2, output2, formula2);
      model.setName("sum");
      RandomVector input3(Normal(2));
      input3.setName("input");
      RandomVector output3(model, input3);
      output3.setName("output");
      Event event(output3, Greater(), 1.0);
      event.setName("failureEvent");
      NumericalPoint designPoint(2, 0.0);
      designPoint[0] = 1.0;
      formResult = FORMResult(NumericalPoint(2, 1.0), event, false);
      formResult.setName("formResult");
      formResult.getImportanceFactors();
      formResult.getImportanceFactors(AnalyticalResult::CLASSICAL);
      formResult.getImportanceFactors(AnalyticalResult::PHYSICAL);
      formResult.getEventProbabilitySensitivity();

      sormResult = SORMResult (NumericalPoint(2, 1.0), event, false);
      sormResult.setName("sormResult");
      sormResult.getEventProbabilityBreitung();
      sormResult.getEventProbabilityHohenBichler();
      sormResult.getEventProbabilityTvedt();
      sormResult.getGeneralisedReliabilityIndexBreitung();
      sormResult.getGeneralisedReliabilityIndexHohenBichler();
      sormResult.getGeneralisedReliabilityIndexTvedt();
    }
    study.add("formResult", formResult);
    study.add("sormResult", sormResult);

    // Create an ARMACoefficients
    ARMACoefficients armaCoefficients;
    {
      const UnsignedInteger dim(2);
      SquareMatrix squareMatrix1(dim);
      squareMatrix1(0, 0) = 1.;
      squareMatrix1(1, 0) = 2.;
      squareMatrix1(0, 1) = 3.;
      squareMatrix1(1, 1) = 4.;
      double s = 3.;
      SquareMatrix squareMatrix2 = squareMatrix1.operator * (s) ;
      double t = 1.5;
      SquareMatrix squareMatrix3 = squareMatrix1.operator / (t) ;
      const UnsignedInteger size(3);
      armaCoefficients = ARMACoefficients(size, dim);
      armaCoefficients[0] = squareMatrix1 ;
      armaCoefficients[1] = squareMatrix2 ;
      armaCoefficients[2] = squareMatrix3 ;
    }
    study.add("armaCoefficients", armaCoefficients);

    // Create an ARMAState
    ARMAState armaState;
    {
      const UnsignedInteger dim(1);
      const UnsignedInteger p(6);
      const UnsignedInteger q(4);
      NumericalSample valuesX(p, dim);
      NumericalSample valuesEpsilon(q, dim);
      for(UnsignedInteger j = 0 ; j < dim ; ++j)
      {
        for(UnsignedInteger i = 0 ; i < p ; ++i)
        {
          valuesX[i][j] = 2.0 * i + 3.0 * j + 1.0;
        }
        for(UnsignedInteger i = 0 ; i < q ; ++i)
        {
          valuesEpsilon[i][j] = RandomGenerator::Generate() ;
        }
      }
      armaState = ARMAState(valuesX, valuesEpsilon);
    }
    study.add("armaState", armaState);

    // Create a RegularGrid
    RegularGrid regularGrid;
    {
      const UnsignedInteger points(8);
      const NumericalScalar tMin(0.0);
      const NumericalScalar tStep(1.0 / (points - 1));
      regularGrid = RegularGrid(tMin, tStep, points);
    }
    study.add("regularGrid", regularGrid);

    // Create a WhiteNoise
    WhiteNoise whiteNoise;
    {
      Distribution dist1 = Normal(0.0, 0.01);
      Distribution dist2 = Normal(0.0, 0.02);
      ComposedDistribution::DistributionCollection aCollection;
      aCollection.add( dist1 );
      aCollection.add( dist2 );
      Distribution dist  = ComposedDistribution(aCollection);
      whiteNoise = WhiteNoise(dist);
      whiteNoise.setTimeGrid(regularGrid);
    }
    study.add("whiteNoise", whiteNoise);

    // Create an WhittleFactory
    WhittleFactory whittleFactory;
    {
      const UnsignedInteger p(1);
      const UnsignedInteger q(2);
      whittleFactory = WhittleFactory( p, q );
    }
    study.add("whittleFactory", whittleFactory);

    // Create an ARMA
    ARMA arma;
    {
      const UnsignedInteger dim(2);
      SquareMatrix squareMatrix1(dim);
      squareMatrix1(0, 0) = 0.2 ;
      squareMatrix1(1, 0) = 0.3 ;
      squareMatrix1(0, 1) = 0.7 ;
      squareMatrix1(1, 1) = 0.4 ;
      SquareMatrix squareMatrix2(dim) ;
      squareMatrix2(0, 0) = 0.1;
      squareMatrix2(1, 0) = 0.0;
      squareMatrix2(0, 1) = 0.0;
      squareMatrix2(1, 1) = 0.5;
      const UnsignedInteger p(1);
      const UnsignedInteger q(1);
      ARMACoefficients coefficientsP(p, dim);
      coefficientsP[0] = squareMatrix1 ;
      ARMACoefficients coefficientsQ(q, dim);
      coefficientsQ[0] = squareMatrix2 ;
      arma = ARMA(coefficientsP, coefficientsQ, whiteNoise);
    }
    study.add("arma", arma);

    // Create a Basis, BasisSequence
    Basis basis;
    BasisSequence basisSequence;
    {
      const UnsignedInteger dimension = 3;
      Description input;
      Description output[dimension];
      for ( UnsignedInteger j = 0; j < dimension; ++ j )
      {
        OSS oss;
        oss << "x" << j;
        String in(oss);
        input.add(in);
        output[j].add(in);
      }
      Collection<NumericalMathFunction> coll;
      Indices cumulBasis;
      Indices partialBasis[dimension];
      for ( UnsignedInteger j = 0; j < dimension; ++ j )
      {
        NumericalMathFunction ei(input, output[j], output[j]);
        coll.add(ei);
        cumulBasis.add(j);
        partialBasis[j] = cumulBasis;
      }

      basis = Basis(coll);
      basisSequence = BasisSequence(basis);
      for ( UnsignedInteger j = 0; j < dimension; ++ j )
      {
        basisSequence.add(partialBasis[j]);
      }
    }
    study.add("basis", basis);
    study.add("basisSequence", basisSequence);

    // Create an UniVariatePolynomial
    UniVariatePolynomial uniVariatePolynomial;
    {
      UniVariatePolynomial::Coefficients coefficients(3);
      coefficients[0] = -2.5;
      coefficients[1] = 3.5;
      coefficients[2] = -1.2;
      uniVariatePolynomial = UniVariatePolynomial(coefficients);
    }
    study.add("uniVariatePolynomial", uniVariatePolynomial);

    // Create a Burr
    Burr burr(1.5, 4.5);
    study.add("burr", burr);



    // Create a CauchyModel
    CauchyModel cauchyModel;
    // Create an ExponentialCauchy
    ExponentialCauchy exponentialCauchy;
    // Create an AbsoluteExponential
    AbsoluteExponential absoluteExponential;
    {
      const UnsignedInteger defaultDimension(1);
      const UnsignedInteger spatialDimension(1);
      NumericalPoint amplitude(defaultDimension, 1.0);
      NumericalPoint scale(spatialDimension, 1.0);
      cauchyModel = CauchyModel (amplitude, scale);
      exponentialCauchy = ExponentialCauchy (amplitude, scale);
      absoluteExponential = AbsoluteExponential(scale, amplitude);
    }
    study.add("cauchyModel", cauchyModel);
    study.add("exponentialCauchy", exponentialCauchy);
    study.add("absoluteExponential", absoluteExponential);

    // Create an ExponentialModel
    SecondOrderModel secondOrderModel(absoluteExponential, cauchyModel);
    study.add("secondOrderModel", secondOrderModel);

    // Create a SpectralNormalProcess
    SpectralNormalProcess spectralNormalProcess(exponentialCauchy, regularGrid);
    study.add("spectralNormalProcess", spectralNormalProcess);

    // Create an  UserDefinedSpectralModel
    UserDefinedSpectralModel userDefinedSpectralModel;
    {
      UnsignedInteger size(5);
      UserDefinedSpectralModel::HermitianMatrixCollection dspCollection(size);
      RegularGrid frequencyGrid(0.0, 2.0 / size, size);
      for (UnsignedInteger i = 0; i < size; ++i)
        dspCollection[i] = cauchyModel(frequencyGrid.getValue(i));
      userDefinedSpectralModel = UserDefinedSpectralModel(frequencyGrid, dspCollection);
    }
    study.add("userDefinedSpectralModel", userDefinedSpectralModel);

    // Create a TemporalNormalProcess
    TemporalNormalProcess temporalNormalProcess(exponentialCauchy, regularGrid);
    study.add("temporalNormalProcess", temporalNormalProcess);

    // Create a CompositeProcess Object
    CompositeProcess compositeProcess;
    {
      NumericalScalar Tmin(0.0);
      NumericalScalar deltaT(0.1);
      UnsignedInteger steps(11);
      RegularGrid timeGrid(Tmin, deltaT, steps);
      ARMA myARMAProcess;
      myARMAProcess.setTimeGrid(timeGrid);
      Description inputDescription(1, "x");
      Description formula(1, "2 * x + 5");
      NumericalMathFunction myOneDimensionalFunction(inputDescription, formula);
      SpatialFunction myFunction(myOneDimensionalFunction);
      compositeProcess = CompositeProcess (myFunction, myARMAProcess);
    }
    study.add("compositeProcess", compositeProcess);

    // Create a Domain Object
    Domain domain;
    {
      UnsignedInteger dim(2);
      NumericalPoint a(dim, -1.0);
      NumericalPoint b(dim, 2.0);
      domain  = Interval(a, b);
    }
    study.add("domain", domain);

    // Create an EventDomainImplementation
    EventDomainImplementation eventDomainImplementation;
    {
      UnsignedInteger dim(2);
      Normal distribution(dim);
      RandomVector X(distribution);
      Description inVars(dim);
      for (UnsignedInteger i = 0; i < dim; ++i) inVars[i] = OSS() << "x" << i;
      NumericalMathFunction model(inVars, inVars);
      CompositeRandomVector Y(model, X);
      Interval domain(dim);
      eventDomainImplementation = EventDomainImplementation(Y, domain);
    }
    study.add("eventDomainImplementation", eventDomainImplementation);

    // Create an EventProcess
    EventProcess eventProcess;
    {
      UnsignedInteger dim(2);
      Normal distribution(dim);

      WhiteNoise X(distribution);
      Interval domain(dim);
      eventProcess = EventProcess(X, domain);
    }
    study.add("eventProcess", eventProcess);

    // Create a ConstantStep
    ConstantStep constantStep;
    // Create a BlendedStep
    BlendedStep blendedStep;
    {
      UnsignedInteger dimension = 2;
      NumericalPoint epsilon( dimension, 1e-5 );
      constantStep = ConstantStep(epsilon);
      NumericalPoint eta( dimension, 1.0 );
      blendedStep = BlendedStep(epsilon, eta);
    }
    study.add("constantStep", constantStep);
    study.add("blendedStep", blendedStep);

    // Create a FunctionalBasisProcess
    FunctionalBasisProcess functionalBasisProcess;
    {
      UnsignedInteger basisDimension(10);
      Basis basis(basisDimension);
      Collection<Distribution> coefficients(basisDimension);
      for (UnsignedInteger i = 0; i < basisDimension; ++i)
      {
        basis[i] = NumericalMathFunction("x", String(OSS() << "sin(" << i << "*x)"));
        coefficients[i] = Normal(0.0, (1.0 + i));
      }
      functionalBasisProcess = FunctionalBasisProcess(ComposedDistribution(coefficients), basis);
    }
    study.add("functionalBasisProcess", functionalBasisProcess);

    // Create a SquareMatrix
    SquareMatrix squareMatrix(2);
    squareMatrix(0, 0) = 1. ;
    squareMatrix(1, 0) = 2. ;
    squareMatrix(0, 1) = 3. ;
    squareMatrix(1, 1) = 4. ;
    study.add("squareMatrix", squareMatrix);

    // Create a SymmetricMatrix
    SymmetricMatrix symmetricMatrix(2);
    symmetricMatrix(0, 0) = 1. ;
    symmetricMatrix(1, 0) = 2. ;
    symmetricMatrix(0, 1) = 3. ;
    symmetricMatrix(1, 1) = 4. ;
    study.add("symmetricMatrix", symmetricMatrix);

    // Create a Tensor
    Tensor tensor(2, 2, 3);
    tensor.setName("tensor1");
    tensor(0, 0, 0) = 1. ;
    tensor(1, 0, 0) = 2. ;
    tensor(0, 1, 0) = 3. ;
    tensor(1, 1, 0) = 4. ;
    tensor(0, 0, 1) = 5. ;
    tensor(1, 0, 1) = 6. ;
    tensor(0, 1, 1) = 7. ;
    tensor(1, 1, 1) = 8. ;
    tensor(0, 0, 2) = 9. ;
    tensor(1, 0, 2) = 10. ;
    tensor(0, 1, 2) = 11. ;
    tensor(1, 1, 2) = 12. ;
    study.add("tensor", tensor);

    // Create a SymmetricTensor
    SymmetricTensor symmetricTensor(2, 3);
    symmetricTensor.setName("symtensor1");
    symmetricTensor(0, 0, 0) = 1. ;
    symmetricTensor(1, 0, 0) = 2. ;
    symmetricTensor(1, 1, 0) = 3. ;
    symmetricTensor(0, 0, 1) = 4. ;
    symmetricTensor(1, 0, 1) = 5. ;
    symmetricTensor(1, 1, 1) = 6. ;
    symmetricTensor(0, 0, 2) = 7. ;
    symmetricTensor(1, 0, 2) = 8. ;
    symmetricTensor(1, 1, 2) = 9. ;
    study.add("symmetricTensor", symmetricTensor);

    // Create a SquareComplexMatrix
    SquareComplexMatrix squareComplexMatrix(2);
    squareComplexMatrix(0, 0) = NumericalComplex(1.0, 1.0) ;
    squareComplexMatrix(0, 1) = NumericalComplex(3.0, 1.0) ;
    squareComplexMatrix(1, 0) = NumericalComplex(0.0, 1.0) ;
    squareComplexMatrix(1, 1) = NumericalComplex(5.0, 1.0) ;
    study.add("squareComplexMatrix", squareComplexMatrix);

    // Create a TriangularComplexMatrix
    TriangularComplexMatrix triangularComplexMatrix(2);
    triangularComplexMatrix(0, 0) = 1. ;
    triangularComplexMatrix(1, 0) = NumericalComplex(3., 1.0) ;
    triangularComplexMatrix(1, 1) = 4. ;
    study.add("triangularComplexMatrix", triangularComplexMatrix);

    // Create an HermitianMatrix
    HermitianMatrix hermitianMatrix(2);
    study.add("hermitianMatrix", hermitianMatrix);

    // Create a LHS
    LHS lhs(event);
    lhs.setMaximumOuterSampling(250);
    lhs.setBlockSize(4);
    lhs.setMaximumCoefficientOfVariation(0.1);
    study.add("lhs", lhs);

    // Create a Mixture
    Mixture mixture;
    {
      UnsignedInteger dimension(3);
      NumericalPoint meanPoint(dimension, 1.0);
      meanPoint[0] = 0.5;
      meanPoint[1] = -0.5;
      NumericalPoint sigma(dimension, 1.0);
      sigma[0] = 2.0;
      sigma[1] = 3.0;
      CorrelationMatrix R(dimension);
      for (UnsignedInteger i = 1; i < dimension; i++)
      {
        R(i, i - 1) = 0.5;
      }
      // Create a collection of distribution
      Mixture::DistributionCollection aCollection;

      aCollection.add( Normal(meanPoint, sigma, R) );
      meanPoint += NumericalPoint(dimension, 1.0);
      aCollection.add( Normal(meanPoint, sigma, R) );
      meanPoint += NumericalPoint(dimension, 1.0);
      aCollection.add( Normal(meanPoint, sigma, R) );

      mixture = Mixture(aCollection, NumericalPoint(aCollection.getSize(), 2.0));
    }
    study.add("mixture", mixture);

    // Create a MixtureClassifier
    MixtureClassifier mixtureClassifier(mixture);
    study.add("mixtureClassifier", mixtureClassifier);

    // Create a MonteCarlo
    MonteCarlo monteCarlo(event);
    monteCarlo.setMaximumOuterSampling(250);
    monteCarlo.setBlockSize(4);
    monteCarlo.setMaximumCoefficientOfVariation(0.1);
    study.add("monteCarlo", monteCarlo);

    // Create a QuasiMonteCarlo
    QuasiMonteCarlo quasiMonteCarlo(event);
    study.add("quasiMonteCarlo", quasiMonteCarlo);

    // Create a RandomizedLHS
    RandomizedLHS randomizedLHS(event);
    study.add("randomizedLHS", randomizedLHS);

    // Create a RandomizedLHS
    RandomizedQuasiMonteCarlo randomizedQuasiMonteCarlo(event);
    study.add("randomizedQuasiMonteCarlo", randomizedQuasiMonteCarlo);

    // Create a RandomWalk
    RandomWalk randomWalk;
    {
      Distribution dist = Uniform();
      NumericalPoint origin(dist.getDimension());
      randomWalk = RandomWalk(origin, dist);
    }
    study.add("randomWalk", randomWalk);

    // Create a TestResult
    TestResult testResult;
    {
      String type = "testResult";
      Bool testBool = 1;
      NumericalScalar testVal = 0.372;
      NumericalScalar testThres = 0.42;
      testResult = TestResult(type, testBool, testVal, testThres);
    }
    study.add("testResult", testResult);

    // Create a StrongMaximumTest
    StrongMaximumTest strongMaximumTest;
    {
      UnsignedInteger dim(analytical.getInputDimension());
      double seuil(10);
      NumericalPoint designPoint(dim, 0.0);
      double C(0.3);
      designPoint[0] = - sqrt(seuil) + C;
      NumericalPoint pseudoDesignPoint(dim, 0.0);
      pseudoDesignPoint[0] = sqrt(seuil) + C;
      NumericalScalar importanceLevel(0.01);
      NumericalScalar accuracyLevel(2);
      NumericalScalar confidenceLevel(0.999999);
      strongMaximumTest = StrongMaximumTest(standardEvent, designPoint, importanceLevel, accuracyLevel, confidenceLevel);
    }
    study.add("strongMaximumTest", strongMaximumTest);

    // Create an ExpertMixture
    ExpertMixture expertMixture;
    {
      Mixture::DistributionCollection aCollection;
      CorrelationMatrix R(2);
      R(0, 1) = -0.99;
      NumericalPoint mean(2);
      mean[0] = -1.0;
      mean[1] = 1.0;
      aCollection.add( Normal(mean, NumericalPoint(2, 1.0), R) );
      R(0, 1) = 0.99;
      mean[0] = 1.0;
      aCollection.add( Normal(mean, NumericalPoint(2, 1.0), R) );
      Mixture distribution(aCollection, NumericalPoint(aCollection.getSize(), 1.0));
      MixtureClassifier classifier(distribution);
      Basis experts(0);
      experts.add(NumericalMathFunction("x", "-x"));
      experts.add(NumericalMathFunction("x", "x"));
      expertMixture = ExpertMixture(experts, classifier);
    }
    study.add("expertMixture", expertMixture);

    // Create a CovarianceModel
    UserDefinedCovarianceModel userDefinedCovarianceModel;
    UserDefinedStationaryCovarianceModel userDefinedStationaryCovarianceModel;
    {
      const UnsignedInteger dimension(1);
      const UnsignedInteger spatialDimension(1);
      NumericalPoint amplitude(dimension);
      NumericalPoint scale(spatialDimension);
      CorrelationMatrix spatialCorrelation(dimension);
      for (UnsignedInteger index = 0 ; index < dimension; ++index)
      {
        amplitude[index] = 1.0 ;
        if (index > 0) spatialCorrelation(index, index - 1) = 1.0 / index;
      }
      for (UnsignedInteger index = 0 ; index < spatialDimension; ++index)
        scale[index] = (index + 1.0) / dimension ;
      ExponentialModel referenceModel(spatialDimension, amplitude, scale, spatialCorrelation);
      UnsignedInteger size(20);
      RegularGrid timeGrid(0.0, 0.1, size);
      {
        const UnsignedInteger collectionSize(size * (size + 1) / 2);
        UserDefinedCovarianceModel::CovarianceMatrixCollection covarianceCollection(collectionSize);
        UnsignedInteger k(0);
        for (UnsignedInteger i = 0; i < timeGrid.getN(); ++i)
        {
          const NumericalScalar t(timeGrid.getValue(i));
          for (UnsignedInteger j = i; j < timeGrid.getN(); ++j)
          {
            const NumericalScalar s(timeGrid.getValue(j));
            covarianceCollection[k] = referenceModel(t, s);
            k++;
          }
        }
        userDefinedCovarianceModel = UserDefinedCovarianceModel(timeGrid, covarianceCollection);
      }
      {
        UserDefinedStationaryCovarianceModel::CovarianceMatrixCollection covarianceCollection(size);
        for (UnsignedInteger i = 0; i < size; ++i)
        {
          const NumericalScalar t(timeGrid.getValue(i));
          covarianceCollection[i] = referenceModel(0, t);
        }
        userDefinedStationaryCovarianceModel = UserDefinedStationaryCovarianceModel(timeGrid, covarianceCollection);
      }
    }
    study.add("userDefinedCovarianceModel", userDefinedCovarianceModel);
    study.add("userDefinedStationaryCovarianceModel", userDefinedStationaryCovarianceModel);

    // Create a RandomGeneratorState
    RandomGenerator::SetSeed(0);
    RandomGeneratorState randomGeneratorState(RandomGenerator::GetState());
    study.add("randomGeneratorState", randomGeneratorState);

    fullprint << "Save the study..." << std::endl;
    study.save();


    // Create a Study Object
    Study study2;
    study2.setStorageManager(XMLStorageManager(fileName));
    study2.load();
    //     fullprint << "saved  Study = " << study << std::endl;
    //     fullprint << "loaded Study = " << study2    << std::endl;

    NumericalPoint namedNumericalPoint2;
    study2.fillObjectByName( namedNumericalPoint2, "point" );
    fullprint << "saved  NumericalPoint = " << namedNumericalPoint  << std::endl;
    fullprint << "loaded NumericalPoint = " << namedNumericalPoint2 << std::endl;


    // Type
    compare<NumericalPoint >( numericalPoint, study2, "Good");
    compare<NumericalPointWithDescription >( numericalPointWithDescription, study2, "pDesc");
    compare<Matrix >( matrix, study2, "m");
    compare<SquareMatrix>(squareMatrix, study2 );
    compare<SymmetricMatrix>(symmetricMatrix, study2 );
    compare<Tensor>(tensor, study2 );
    compare<SymmetricTensor>(symmetricTensor, study2 );
    compare<SquareComplexMatrix>(squareComplexMatrix, study2 );
    compare<TriangularComplexMatrix>(triangularComplexMatrix, study2 );
    compare<HermitianMatrix>(hermitianMatrix, study2 );

    // Graph
    compare<Staircase >( staircase, study2 );

    // Stat
    compare<NumericalSample >( numericalSample, study2, "mySample");
    compare<RandomGeneratorState>( randomGeneratorState, study2 );

    // Func
    compare<NumericalMathFunction >( analytical, study2 , "analytical");

    // Optim
    compare<AbdoRackwitz >( abdoRackwitz, study2 );
    compare<SQP >( sqp, study2, "sqp" );
    compare<Cobyla >( cobyla, study2, "cobyla" );
    compare<OptimizationSolver >( tnc, study2, "tnc" );

    // Model
    compare<Event >( event, study2 );
    compare<StandardEvent >( standardEvent, study2 );

    // Distribution
    compare<Arcsine >( arcsine, study2 );
    compare<Beta >( beta, study2 );
    compare<Burr >( burr, study2 );
    compare<Chi >( chi, study2 );
    compare<ChiSquare >( chiSquare, study2 );
    compare<Dirichlet >( dirichlet, study2 );
    compare<ComposedDistribution >( composedDistribution, study2 );
    compare<Exponential >( exponential, study2 );
    compare<FisherSnedecor >( fisherSnedecor, study2 );
    compare<Gamma >( gamma, study2 );
    compare<GeneralizedPareto >( generalizedPareto, study2 );
    compare<Geometric >( geometric, study2 );
    compare<Gumbel >( gumbel, study2 );
    compare<IndependentCopula >( independentCopula, study2 );
    compare<InverseNormal>(inverseNormal, study2 );
    compare<KernelSmoothing >( kernelSmoothing, study2 );
    compare<Laplace >( laplace, study2 );
    compare<Logistic >( logistic, study2 );
    compare<LogNormal >( logNormal, study2 );
    compare<LogUniform>(logUniform, study2 );
    compare<MeixnerDistribution>(meixnerDistribution, study2 );
    compare<Mixture>(mixture, study2 );
    compare<NormalCopula >( normalCopula, study2 );
    compare<Rayleigh>(rayleigh, study2 );
    compare<Rice>(rice, study2 );
    compare<Student>(student, study2 );
    compare<Trapezoidal>(trapezoidal, study2 );
    compare<Triangular>(triangular, study2 );
    compare<TruncatedDistribution>(truncatedDistribution, study2 );
    compare<TruncatedNormal>(truncatedNormal, study2 );
    compare<Uniform >( uniform, study2 );
    compare<UserDefined>(userDefined, study2 );
    compare<Weibull>(weibull, study2 );

    // Simulation
    compare<MonteCarlo>(monteCarlo, study2 );
    compare<LHS>(lhs, study2, "lhs");
    compare<QuasiMonteCarlo>(quasiMonteCarlo, study2 );
    compare<RandomizedLHS>(randomizedLHS, study2 );
    compare<RandomizedQuasiMonteCarlo>(randomizedQuasiMonteCarlo, study2 );
    compare<SimulationResult >( simulationResult, study2 );

    // Analytical
    compare<SORM >( sorm, study2, "sorm");
    compare<FORMResult >( formResult, study2 , "formResult");
    compare<SORMResult >( sormResult, study2,  "sormResult");

    compare<TaylorExpansionMoments >( taylorExpansionMoments, study2 );

    // Process
    compare<ARMACoefficients >( armaCoefficients, study2 , "armaCoefficients");
    compare<ARMAState >( armaState, study2, "armaState");
    compare<WhiteNoise >( whiteNoise, study2 );
    compare<WhittleFactory >( whittleFactory, study2 );
    compare<ARMA >( arma, study2, "arma");
    compare<Basis >( basis, study2 );
    compare<BasisSequence >( basisSequence, study2 );
    compare<UniVariatePolynomial >( uniVariatePolynomial, study2 );
    compare<CauchyModel >( cauchyModel, study2 );
    compare<ExponentialCauchy >( exponentialCauchy, study2 );
    compare<AbsoluteExponential >( absoluteExponential, study2 );
    compare<SecondOrderModel >( secondOrderModel, study2 );
    compare<CompositeProcess >( compositeProcess, study2 );
    compare<SpectralNormalProcess >( spectralNormalProcess, study2 );
    compare<TemporalNormalProcess >( temporalNormalProcess, study2 );
    compare<Domain >( domain, study2 );
    compare<EventDomainImplementation >( eventDomainImplementation, study2 );
    compare<EventProcess >( eventProcess, study2 );
    compare<ConstantStep >( constantStep, study2 );
    compare<BlendedStep >( blendedStep, study2 );
    compare<FunctionalBasisProcess >( functionalBasisProcess, study2 );
    compare<RandomWalk>(randomWalk, study2 );
    compare<RegularGrid>(regularGrid, study2 );
    compare<MixtureClassifier>(mixtureClassifier, study2 );

    // StatTest
    compare<TestResult>( testResult, study2 );
    compare<StrongMaximumTest>( strongMaximumTest, study2 );

    // ExpertMixture
    compare<ExpertMixture>( expertMixture, study2 );

    // CovarianceModel
    compare<UserDefinedCovarianceModel>( userDefinedCovarianceModel, study2 );
    compare<UserDefinedStationaryCovarianceModel>( userDefinedStationaryCovarianceModel, study2 );

    // cleanup
    Os::Remove(fileName);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }




  return ExitCode::Success;
}
