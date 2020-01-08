//                                               -*- C++ -*-
/**
 *  @brief The test file of class DiscreteMarkovChain
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "openturns/IdentityMatrix.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    const Scalar Tmin = 0.0;
    const Scalar deltaT = 0.1;
    const UnsignedInteger N = 20;

    /* Initialization of the RegularGrid timeGrid1*/
    RegularGrid timeGrid(Tmin, deltaT, N);

    /* Process creation from int and matrix */
    DiscreteMarkovChain processIntSqMat(1, IdentityMatrix(2));
    fullprint << "### Process from int + SquareMatrix constructor:" << std::endl << processIntSqMat << std::endl << std::endl;

    /* Process creation from default constructor */
    DiscreteMarkovChain processDefault;
    fullprint << "### Process from default constructor:" << std::endl << processDefault << std::endl << std::endl;

    /* Transition matrix choice */
    /* See https://fr.wikipedia.org/wiki/Cha%C3%AEne_de_Markov#Exemple_:_Doudou_le_hamster */
    /* First line is set to [0.0, 0.5, 0.5] for the sake of demonstration on a small number of time steps */
    SquareMatrix transition(3, 3);
    transition(0, 0) = 0.00;
    transition(1, 0) = 0.70;
    transition(2, 0) = 0.80;
    transition(0, 1) = 0.50;
    transition(1, 1) = 0.00;
    transition(2, 1) = 0.00;
    transition(0, 2) = 0.50;
    transition(1, 2) = 0.30;
    transition(2, 2) = 0.20;
    fullprint << "### Transition matrix:" << std::endl << transition << std::endl << std::endl;

    /* Origin choice (UserDefined distribution)*/
    Sample states(3, 1);
    states(0, 0) = 0;
    states(1, 0) = 1;
    states(2, 0) = 2;

    Point weights(3, 0);
    weights[0] = 0.50;
    weights[1] = 0.15;
    weights[2] = 0.35;

    UserDefined origin(states, weights);

    /* Transition matrix accessor */
    processDefault.setTransitionMatrix(transition);

    /* Origin accessor */
    processDefault.setOrigin(origin);

    /* Setting the timeGrid */
    processDefault.setTimeGrid(timeGrid);

    /* Print of the process */
    fullprint << "### Modified process:" << std::endl << processDefault << std::endl << std::endl;

    /* Initialization of the TimeSeries */
    TimeSeries timeSerie = processDefault.getRealization();

    /* Full prints */
    fullprint << "### Time serie:" << std::endl << timeSerie << std::endl << std::endl;
    // Some steps further
    UnsignedInteger stepNumber = 20;
    fullprint << "### One future:" << std::endl << processDefault.getFuture(stepNumber) << std::endl << std::endl;
    UnsignedInteger size = 3;
    fullprint << "### Some futures:" << std::endl << processDefault.getFuture(stepNumber, size) << std::endl << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
