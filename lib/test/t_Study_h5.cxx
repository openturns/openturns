//                                               -*- C++ -*-
/**
 *  @brief The test file of class Study along XMLH5StorageManager
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



int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  const char fileName[] = "myStudyH5.xml";
  const char h5fileName[] = "myStudyH5.h5";

  try
  {

    // Using wrong type of StorageManager when loading study
    Study study(fileName);
    study.setStorageManager(XMLH5StorageManager(fileName));
    Point point(2, 1.0);
    study.add("point", point);
    study.save();
    Study study2(fileName);
    study2.setStorageManager(XMLStorageManager(fileName));
    study2.load();

    // cleanup
    Os::Remove(fileName);
    Os::Remove(h5fileName);
  }
  catch (std::exception& exc)
  {
    std::cout << "Expected: " << exc.what() << std::endl;
  }

  try
  {

    // Create a Study Object by giving its name
    {
      Study study(fileName);
      study.setStorageManager(XMLH5StorageManager(fileName));
      Point point(2, 1.0);
      study.add("point", point);
      study.save();
      Study study2(fileName);
      study2.setStorageManager(XMLH5StorageManager(fileName));
      study2.load();
      Point point2;
      study2.fillObject("point", point2);

      // cleanup
      Os::Remove(fileName);
      Os::Remove(h5fileName);
    }

    // Create a Study Object by giving its storage manager with a nonzero compression level
    {
      Study study;
      study.setStorageManager(XMLH5StorageManager(fileName, 5));
      //study.setStorageManager(XMLStorageManager(fileName, 5));
      Point point(2, 1.0);
      study.add("point", point);
      study.save();
      Study study2(fileName);
      study2.setStorageManager(XMLH5StorageManager(fileName, 5));
      study2.load();
      Point point2;
      study2.fillObject("point", point2);

      // cleanup
      Os::Remove(fileName);
      Os::Remove(h5fileName);
    }

    // Create a Study Object by giving its name and compression level
    {
      Study study(fileName, 5);
      study.setStorageManager(XMLH5StorageManager(fileName));
      Point point(2, 1.0);
      study.add("point", point);
      study.save();
      Study study2(fileName);
      study2.setStorageManager(XMLH5StorageManager(fileName));
      study2.load();
      Point point2;
      study2.fillObject("point", point2);

      // cleanup
      Os::Remove(fileName);
      Os::Remove(h5fileName);
    }

    // Create a Study Object
    Study study;
    study.setStorageManager(XMLH5StorageManager(fileName));


    // Add a PersistentObject to the Study (here a Point)
    Point numericalPoint(3, 0.);
    numericalPoint[0] = 10.;
    numericalPoint[1] = 11.;
    numericalPoint[2] = 12.;
    numericalPoint.setName("point");
    study.add("Good", numericalPoint);

    // Add another PersistentObject to the Study (here a Sample)
    Sample numericalSample(3, 2);
    Point p2(2, 0.);
    p2[0] = 100.;
    p2[1] = 200.;
    numericalSample[0] = p2;
    p2.setName("One");
    Point p3(2, 0.);
    p3[0] = 101.;
    p3[1] = 201.;
    numericalSample[1] = p3;
    p3.setName("Two");
    Point p4(2, 0.);
    p4[0] = 102.;
    p4[1] = 202.;
    numericalSample[2] = p4;
    p4.setName("Three");
    study.add("mySample", numericalSample);

    // Create a Point that we will try to reinstaciate after reloading
    Point namedPoint(2, 1000.);
    namedPoint.setName("point");
    study.add("namedPoint", namedPoint);

    // Add a point with a description
    PointWithDescription numericalPointWithDescription(numericalPoint);
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
      // Instantiate one distribution object
      UnsignedInteger dim = 1;
      Point meanPoint(dim, 1.0);
      meanPoint[0] = 0.5;
      Point sigma(dim, 1.0);
      sigma[0] = 2.0;
      CorrelationMatrix R = IdentityMatrix(dim);
      Normal distribution1(meanPoint, sigma, R);

      // Instantiate another distribution object
      meanPoint[0] = -1.5;
      sigma[0] = 4.0;
      Normal distribution2(meanPoint, sigma, R);

      // Test for sampling
      UnsignedInteger size = 2000;
      UnsignedInteger nPoints = 20;
      Sample sample1(distribution1.getSample( size ));
      Sample sample2(distribution2.getSample( size ));

      // Construct empirical CDF for each sample
      Sample data1(nPoints, 2), data2(nPoints, 2);
      Point cursor1(2);
      Point cursor2(2);
      Scalar count1;
      Scalar count2;

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
    ProbabilitySimulationResult simulationResult(ThresholdEvent(), 0.5, 0.01, 150, 4);
    simulationResult.setName("probabilitySimulationResult");
    study.add("probabilitySimulationResult", simulationResult);

    Arcsine arcsine(5.2, 11.6);
    study.add("arcsine", arcsine);

    Beta beta(3.0, 2.0, -1.0, 4.0);
    study.add("beta", beta);

    Chi chi(1.5);
    study.add("chi", chi);

    ChiSquare chiSquare(1.5);
    study.add("chiSquare", chiSquare);

    Dirichlet dirichlet;
    {
      UnsignedInteger dim = 2;
      Point theta(dim + 1);
      for (UnsignedInteger i = 0; i <= dim; i++) theta[i] = 1.0 + (i + 1.0) / 4.0;
      dirichlet = Dirichlet(theta);
      study.add("dirichlet", dirichlet);
    }

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

    InverseNormal inverseNormal(2.0, 0.5);
    study.add("inverseNormal", inverseNormal);

    KernelSmoothing kernelSmoothing;
    kernelSmoothing.build(independentCopula.getSample(20));
    study.add("kernelSmoothing", kernelSmoothing);

    Laplace laplace(0.5, 1.0 / 1.5);
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
      Sample x(3, 1);
      x[0][0] = 1.0;
      x[1][0] = 2.0;
      x[2][0] = 3.0;
      Point p(3);
      p[0] = 0.3;
      p[1] = 0.1;
      p[2] = 0.6;
      userDefined = UserDefined(x, p);
      study.add("userDefined", userDefined);
    }

    // Create a WeibullMin distribution
    WeibullMin weibull(2.0, 1.5, -0.5);
    study.add("weibullMin", weibull);

    // Create a NormalCopula distribution
    NormalCopula normalCopula;
    {
      CorrelationMatrix R(3);
      R(0, 1) = 0.5;
      R(1, 2) = 0.5;
      normalCopula = NormalCopula(R);
      study.add("normalCopula", normalCopula);
    }

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

    // Create an analytical Function
    SymbolicFunction analytical;
    {
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
      analytical  = SymbolicFunction(input, formulas);
      analytical.setName("analytical");
      study.add("analytical", analytical);
    }

    // Create an Event Object
    ThresholdEvent event;
    {
      const Point mu = {101, 202, 303};
      RandomVector vect(Normal(mu, Point(3, 1e-6), CorrelationMatrix(3)));
      CompositeRandomVector output2(analytical.getMarginal(0), vect);
      event = ThresholdEvent(output2, Less(), 50);
    }
    study.add("randomVector", event);

    // Create a StandardEvent Object
    StandardEvent standardEvent(event);
    study.add("standardEvent", standardEvent);

    // Create a TaylorExpansionMoments algorithm
    TaylorExpansionMoments taylorExpansionMoments;
    {
      RandomVector antecedent(IndependentCopula(analytical.getInputDimension()));
      antecedent.setName("antecedent");
      CompositeRandomVector composite(analytical, antecedent);
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
    cobyla.setMaximumCallsNumber(100);
    cobyla.setMaximumAbsoluteError(1.0e-10);
    cobyla.setMaximumRelativeError(1.0e-10);
    cobyla.setMaximumResidualError(1.0e-10);
    cobyla.setMaximumConstraintError(1.0e-10);

    // Create a TNC algorithm
    OptimizationAlgorithm tnc(new TNC());
    {
      Interval bounds(Point(3, -3.0), Point(3, 5.0));
      Description input2(3);
      input2[0] = "x";
      input2[1] = "y";
      input2[2] = "z";
      Description output2(1);
      output2[0] = "d";
      Description formula2(output2.getSize());
      formula2[0] = "(x+2*y^2+3*z^3)/6";
      SymbolicFunction model(input2, formula2);
      model.setName("complex");

      OptimizationProblem problem(model);
      problem.setBounds(bounds);
      problem.setMinimization(true);

      tnc.setProblem(problem);
      tnc.setStartingPoint(Point(3, 1.0));
    }
    study.add("tnc", tnc);

    // Create a SORM algorithm
    SORM sorm(abdoRackwitz, event, Point(3, 4.));
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
      SymbolicFunction model(input2, formula2);
      model.setName("sum");
      RandomVector input3(Normal(2));
      input3.setName("input");
      CompositeRandomVector output3(model, input3);
      output3.setName("output");
      ThresholdEvent event2(output3, Greater(), 1.0);
      event.setName("failureEvent");
      Point designPoint(2, 0.0);
      designPoint[0] = 1.0;
      formResult = FORMResult(Point(2, 1.0), event2, false);
      formResult.setName("formResult");
      formResult.getImportanceFactors();
      formResult.getImportanceFactors(AnalyticalResult::CLASSICAL);
      formResult.getImportanceFactors(AnalyticalResult::PHYSICAL);
      formResult.getEventProbabilitySensitivity();

      sormResult = SORMResult (Point(2, 1.0), event2, false);
      sormResult.setName("sormResult");
      sormResult.getEventProbabilityBreitung();
      sormResult.getEventProbabilityHohenbichler();
      sormResult.getEventProbabilityTvedt();
      sormResult.getGeneralisedReliabilityIndexBreitung();
      sormResult.getGeneralisedReliabilityIndexHohenbichler();
      sormResult.getGeneralisedReliabilityIndexTvedt();
    }
    study.add("formResult", formResult);
    study.add("sormResult", sormResult);

    // Create an ARMACoefficients
    ARMACoefficients armaCoefficients;
    {
      const UnsignedInteger dim2 = 2;
      SquareMatrix squareMatrix1(dim2);
      squareMatrix1(0, 0) = 1.;
      squareMatrix1(1, 0) = 2.;
      squareMatrix1(0, 1) = 3.;
      squareMatrix1(1, 1) = 4.;
      double s = 3.;
      SquareMatrix squareMatrix2 = squareMatrix1.operator * (s) ;
      double t = 1.5;
      SquareMatrix squareMatrix3 = squareMatrix1.operator / (t) ;
      const UnsignedInteger size = 3;
      armaCoefficients = ARMACoefficients(size, dim2);
      armaCoefficients[0] = squareMatrix1 ;
      armaCoefficients[1] = squareMatrix2 ;
      armaCoefficients[2] = squareMatrix3 ;
    }
    study.add("armaCoefficients", armaCoefficients);

    // Create an ARMAState
    ARMAState armaState;
    {
      const UnsignedInteger dim = 1;
      const UnsignedInteger p = 6;
      const UnsignedInteger q = 4;
      Sample valuesX(p, dim);
      Sample valuesEpsilon(q, dim);
      for(UnsignedInteger j = 0 ; j < dim ; ++ j)
      {
        for(UnsignedInteger i = 0 ; i < p ; ++ i)
        {
          valuesX[i][j] = 2.0 * i + 3.0 * j + 1.0;
        }
        for(UnsignedInteger i = 0 ; i < q; ++ i)
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
      const UnsignedInteger points = 8;
      const Scalar tMin = 0.0;
      const Scalar tStep = 1.0 / (points - 1);
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

    // Create a WhittleFactory
    WhittleFactory whittleFactory(1, 2);
    study.add("whittleFactory", whittleFactory);

    // Create an ARMA
    ARMA arma;
    {
      const UnsignedInteger dim = 2;
      SquareMatrix squareMatrix1(dim);
      squareMatrix1(0, 0) = 0.2 ;
      squareMatrix1(1, 0) = 0.3 ;
      squareMatrix1(0, 1) = 0.7 ;
      squareMatrix1(1, 1) = 0.4 ;
      SquareMatrix squareMatrix2(dim);
      squareMatrix2(0, 0) = 0.1;
      squareMatrix2(1, 0) = 0.0;
      squareMatrix2(0, 1) = 0.0;
      squareMatrix2(1, 1) = 0.5;
      const UnsignedInteger p = 1;
      const UnsignedInteger q = 1;
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
      Collection<Function> coll;
      Indices cumulBasis;
      Indices partialBasis[dimension];
      for ( UnsignedInteger j = 0; j < dimension; ++ j )
      {
        SymbolicFunction ei(input, output[j]);
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
    // Create an AbsoluteExponential
    AbsoluteExponential absoluteExponential;
    {
      const UnsignedInteger defaultDimension = 1;
      const UnsignedInteger inputDimension = 1;
      Point amplitude(defaultDimension, 1.0);
      Point scale(inputDimension, 1.0);
      cauchyModel = CauchyModel (scale, amplitude);
      absoluteExponential = AbsoluteExponential(scale, amplitude);
    }
    study.add("cauchyModel", cauchyModel);
    study.add("absoluteExponential", absoluteExponential);

    // Create a SpectralGaussianProcess
    SpectralGaussianProcess spectralGaussianProcess(cauchyModel, regularGrid);
    study.add("spectralGaussianProcess", spectralGaussianProcess);

    // Create an  UserDefinedSpectralModel
    UserDefinedSpectralModel userDefinedSpectralModel;
    {
      UnsignedInteger size = 5;
      UserDefinedSpectralModel::HermitianMatrixCollection dspCollection(size);
      RegularGrid frequencyGrid(0.0, 2.0 / size, size);
      for (UnsignedInteger i = 0; i < size; ++i)
        dspCollection[i] = cauchyModel(frequencyGrid.getValue(i));
      userDefinedSpectralModel = UserDefinedSpectralModel(frequencyGrid, dspCollection);
    }
    study.add("userDefinedSpectralModel", userDefinedSpectralModel);

    // Create a GaussianProcess
    GaussianProcess gaussianProcess(absoluteExponential, regularGrid);
    study.add("gaussianProcess", gaussianProcess);

    // Create a CompositeProcess Object
    CompositeProcess compositeProcess;
    {
      Scalar Tmin = 0.0;
      Scalar deltaT = 0.1;
      UnsignedInteger steps = 11;
      RegularGrid timeGrid(Tmin, deltaT, steps);
      ARMA myARMAProcess;
      myARMAProcess.setTimeGrid(timeGrid);
      Description inputDescription(1, "x");
      Description formula(1, "2 * x + 5");
      SymbolicFunction myOneDimensionalFunction(inputDescription, formula);
      ValueFunction myFunction(myOneDimensionalFunction, timeGrid);
      compositeProcess = CompositeProcess (myFunction, myARMAProcess);
    }
    study.add("compositeProcess", compositeProcess);

    // Create a Domain Object
    Domain domain;
    {
      UnsignedInteger dim = 2;
      Point a(dim, -1.0);
      Point b(dim, 2.0);
      domain  = Interval(a, b);
    }
    study.add("domain", domain);

    // Create an DomainEvent
    DomainEvent eventDomain;
    {
      UnsignedInteger dim = 2;
      Normal distribution(dim);
      RandomVector X(distribution);
      Description inVars(dim);
      for (UnsignedInteger i = 0; i < dim; ++i) inVars[i] = OSS() << "x" << i;
      SymbolicFunction model(inVars, inVars);
      CompositeRandomVector Y(model, X);
      eventDomain = DomainEvent(Y, Interval(dim));
    }
    study.add("domainEvent", eventDomain);

    // Create an ProcessEvent
    ProcessEvent eventProcess;
    {
      UnsignedInteger dim = 2;
      Normal distribution(dim);

      WhiteNoise X(distribution);
      eventProcess = ProcessEvent(X, Interval(dim));
    }
    study.add("processEvent", eventProcess);

    // Create a ConstantStep
    ConstantStep constantStep;
    // Create a BlendedStep
    BlendedStep blendedStep;
    {
      UnsignedInteger dimension = 2;
      Point epsilon( dimension, 1e-5 );
      constantStep = ConstantStep(epsilon);
      Point eta( dimension, 1.0 );
      blendedStep = BlendedStep(epsilon, eta);
    }
    study.add("constantStep", constantStep);
    study.add("blendedStep", blendedStep);

    // Create a FunctionalBasisProcess
    FunctionalBasisProcess functionalBasisProcess;
    {
      UnsignedInteger basisDimension = 10;
      Collection<Function> basis2(basisDimension);
      Collection<Distribution> coefficients(basisDimension);
      for (UnsignedInteger i = 0; i < basisDimension; ++i)
      {
        basis2[i] = SymbolicFunction("x", String(OSS() << "sin(" << i << "*x)"));
        coefficients[i] = Normal(0.0, (1.0 + i));
      }
      functionalBasisProcess = FunctionalBasisProcess(ComposedDistribution(coefficients), basis2);
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
    squareComplexMatrix(0, 0) = Complex(1.0, 1.0) ;
    squareComplexMatrix(0, 1) = Complex(3.0, 1.0) ;
    squareComplexMatrix(1, 0) = Complex(0.0, 1.0) ;
    squareComplexMatrix(1, 1) = Complex(5.0, 1.0) ;
    study.add("squareComplexMatrix", squareComplexMatrix);

    // Create a TriangularComplexMatrix
    TriangularComplexMatrix triangularComplexMatrix(2);
    triangularComplexMatrix(0, 0) = 1. ;
    triangularComplexMatrix(1, 0) = Complex(3., 1.0) ;
    triangularComplexMatrix(1, 1) = 4. ;
    study.add("triangularComplexMatrix", triangularComplexMatrix);

    // Create an HermitianMatrix
    HermitianMatrix hermitianMatrix(2);
    study.add("hermitianMatrix", hermitianMatrix);

    // Create a Mixture
    Mixture mixture;
    {
      UnsignedInteger dimension = 3;
      Point meanPoint(dimension, 1.0);
      meanPoint[0] = 0.5;
      meanPoint[1] = -0.5;
      Point sigma(dimension, 1.0);
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
      meanPoint += Point(dimension, 1.0);
      aCollection.add( Normal(meanPoint, sigma, R) );
      meanPoint += Point(dimension, 1.0);
      aCollection.add( Normal(meanPoint, sigma, R) );

      mixture = Mixture(aCollection, Point(aCollection.getSize(), 2.0));
    }
    study.add("mixture", mixture);

    // Create a MixtureClassifier
    MixtureClassifier mixtureClassifier(mixture);
    study.add("mixtureClassifier", mixtureClassifier);

    // Create a MonteCarlo
    MonteCarloExperiment experiment;
    ProbabilitySimulationAlgorithm monteCarlo(event, experiment);
    monteCarlo.setMaximumOuterSampling(250);
    monteCarlo.setBlockSize(4);
    monteCarlo.setMaximumCoefficientOfVariation(0.1);
    study.add("probabilitySimulationAlgorithm", monteCarlo);

    // Create a RandomWalk
    RandomWalk randomWalk;
    {
      Distribution dist = Uniform();
      Point origin(dist.getDimension());
      randomWalk = RandomWalk(origin, dist);
    }
    study.add("randomWalk", randomWalk);

    // Create a TestResult
    TestResult testResult;
    {
      String type = "testResult";
      Bool testBool = 1;
      Scalar testVal = 0.372;
      Scalar testThres = 0.42;
      Scalar statistic = 0.59;
      testResult = TestResult(type, testBool, testVal, testThres, statistic);
    }
    study.add("testResult", testResult);

    // Create a StrongMaximumTest
    StrongMaximumTest strongMaximumTest;
    {
      UnsignedInteger dim = analytical.getInputDimension();
      double seuil(10);
      Point designPoint(dim, 0.0);
      double C(0.3);
      designPoint[0] = - sqrt(seuil) + C;
      Point pseudoDesignPoint(dim, 0.0);
      pseudoDesignPoint[0] = sqrt(seuil) + C;
      Scalar importanceLevel = 0.01;
      Scalar accuracyLevel = 2;
      Scalar confidenceLevel = 0.999999;
      strongMaximumTest = StrongMaximumTest(standardEvent, designPoint, importanceLevel, accuracyLevel, confidenceLevel);
    }
    study.add("strongMaximumTest", strongMaximumTest);

    // Create an ExpertMixture
    ExpertMixture expertMixture;
    {
      Mixture::DistributionCollection aCollection;
      CorrelationMatrix R(2);
      R(0, 1) = -0.99;
      Point mean(2);
      mean[0] = -1.0;
      mean[1] = 1.0;
      aCollection.add( Normal(mean, Point(2, 1.0), R) );
      R(0, 1) = 0.99;
      mean[0] = 1.0;
      aCollection.add( Normal(mean, Point(2, 1.0), R) );
      Mixture distribution(aCollection, Point(aCollection.getSize(), 1.0));
      MixtureClassifier classifier(distribution);
      Collection<Function> experts(0);
      experts.add(SymbolicFunction("x", "-x"));
      experts.add(SymbolicFunction("x", "x"));
      expertMixture = ExpertMixture(experts, classifier);
    }
    study.add("expertMixture", expertMixture);

    // Create a CovarianceModel
    UserDefinedCovarianceModel userDefinedCovarianceModel;
    UserDefinedStationaryCovarianceModel userDefinedStationaryCovarianceModel;
    {
      const UnsignedInteger dimension = 1;
      const UnsignedInteger inputDimension = 1;
      Point amplitude(dimension);
      Point scale(inputDimension);
      CorrelationMatrix spatialCorrelation(dimension);
      for (UnsignedInteger index = 0 ; index < dimension; ++index)
      {
        amplitude[index] = 1.0 ;
        if (index > 0) spatialCorrelation(index, index - 1) = 1.0 / index;
      }
      for (UnsignedInteger index = 0 ; index < inputDimension; ++index)
        scale[index] = (index + 1.0) / dimension ;
      ExponentialModel referenceModel(scale, amplitude, spatialCorrelation);
      UnsignedInteger size = 20;
      RegularGrid timeGrid(0.0, 0.1, size);
      {
        const UnsignedInteger collectionSize = size * (size + 1) / 2;
        UserDefinedStationaryCovarianceModel::SquareMatrixCollection covarianceCollection(collectionSize);
        CovarianceMatrix covariance(size);
        UnsignedInteger k = 0;
        for (UnsignedInteger i = 0; i < timeGrid.getN(); ++i)
        {
          const Scalar t = timeGrid.getValue(i);
          for (UnsignedInteger j = 0; j <= i; ++ j)
          {
            const Scalar s = timeGrid.getValue(j);
            covarianceCollection[k] = referenceModel(t, s);
            covariance(i, j) = covarianceCollection[k](0, 0);
            k++;
          }
        }
        userDefinedCovarianceModel = UserDefinedCovarianceModel(timeGrid, covariance);
      }
      {
        UserDefinedStationaryCovarianceModel::SquareMatrixCollection covarianceCollection(size);
        for (UnsignedInteger i = 0; i < size; ++i)
        {
          const Scalar t = timeGrid.getValue(i);
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
    study2.setStorageManager(XMLH5StorageManager(fileName));
    study2.load();
    //     fullprint << "saved  Study = " << study << std::endl;
    //     fullprint << "loaded Study = " << study2    << std::endl;

    Point namedPoint2;
    study2.fillObjectByName( namedPoint2, "point" );
    fullprint << "saved  Point = " << namedPoint  << std::endl;
    fullprint << "loaded Point = " << namedPoint2 << std::endl;


    // Type
    compare<Point >( numericalPoint, study2, "Good");
    compare<PointWithDescription >( numericalPointWithDescription, study2, "pDesc");
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
    compare<Sample >( numericalSample, study2, "mySample");
    compare<RandomGeneratorState>( randomGeneratorState, study2 );

    // Func
    compare<Function >( analytical, study2, "analytical");

    // Optim
    compare<AbdoRackwitz >( abdoRackwitz, study2 );
    compare<SQP >( sqp, study2, "sqp" );
    compare<Cobyla >( cobyla, study2, "cobyla" );
    compare<OptimizationAlgorithm >( tnc, study2, "tnc" );

    // Model
    //compare<RandomVector >( event, study2 );
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
    compare<WeibullMin>(weibull, study2 );

    // Simulation
    compare<ProbabilitySimulationAlgorithm>(monteCarlo, study2 );
    compare<ProbabilitySimulationResult >( simulationResult, study2 );

    // Analytical
    compare<SORM >( sorm, study2, "sorm");
    compare<FORMResult >( formResult, study2, "formResult");
    compare<SORMResult >( sormResult, study2,  "sormResult");

    compare<TaylorExpansionMoments >( taylorExpansionMoments, study2 );

    // Process
    compare<ARMACoefficients >( armaCoefficients, study2, "armaCoefficients");
    compare<ARMAState >( armaState, study2, "armaState");
    compare<WhiteNoise >( whiteNoise, study2 );
    compare<WhittleFactory >( whittleFactory, study2 );
    compare<ARMA >( arma, study2, "arma");
    compare<Basis >( basis, study2 );
    compare<BasisSequence >( basisSequence, study2 );
    compare<UniVariatePolynomial >( uniVariatePolynomial, study2 );
    compare<CauchyModel >( cauchyModel, study2 );
    compare<AbsoluteExponential >( absoluteExponential, study2 );
    compare<CompositeProcess >( compositeProcess, study2 );
    compare<SpectralGaussianProcess >( spectralGaussianProcess, study2 );
    compare<GaussianProcess >( gaussianProcess, study2 );
    compare<Domain >( domain, study2 );
    compare<DomainEvent >( eventDomain, study2 );
    compare<ProcessEvent >( eventProcess, study2 );
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
    Os::Remove(h5fileName);

    // Create a Study Object by giving its name
    {
      Study study3(fileName);
      study3.setStorageManager(XMLH5StorageManager(fileName));
      Point point(4);
      point[0] = std::numeric_limits<Scalar>::quiet_NaN();
      point[1] = sqrt(-1.0);
      point[2] = std::numeric_limits<Scalar>::infinity();
      point[3] = -std::numeric_limits<Scalar>::infinity();
      study3.add("point", point);
      study3.save();
      Study study4(fileName);
      study4.setStorageManager(XMLH5StorageManager(fileName));
      study4.load();
      Point point2;
      study3.fillObject("point", point2);
      for (UnsignedInteger j = 0; j < point2.getDimension(); ++ j)
      {
        std::cout << "j=" << j;
        std::cout << " isnormal=" << SpecFunc::IsNormal(point2[j]);
        std::cout << " isnan=" << std::isnan(point2[j]);
        std::cout << " isinf=" << std::isinf(point2[j]) << std::endl;
      }
      // cleanup
      Os::Remove(fileName);
      Os::Remove(h5fileName);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }




  return ExitCode::Success;
}
