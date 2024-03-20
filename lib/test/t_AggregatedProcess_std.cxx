//                                               -*- C++ -*-
/**
 *  @brief The test file of class CompositeProcess
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    RandomGenerator::SetSeed(0);

    /* Time grid creation */
    Scalar Tmin = 0.0;
    Scalar deltaT = 0.1;
    UnsignedInteger steps = 11;

    /* Initialization of the time grid timeGrid1*/
    RegularGrid timeGrid(Tmin, deltaT, steps);

    {
      /* Default aggregated process creation */
      AggregatedProcess myProcess;

      /* We fix the time grid to the process */
      myProcess.setTimeGrid(timeGrid);

      fullprint << "myProcess=" << myProcess << std::endl;
      fullprint << "myProcess=" << myProcess.__str__() << std::endl;
      fullprint << "a realization=" << myProcess.getRealization() << std::endl;
      Process marginalProcess(myProcess.getMarginal(0));
      fullprint << "a marginal process=" << marginalProcess << std::endl;
    }
    {
      /* Parameter aggregated process creation */
      Collection< Process > coll(0);
      coll.add(WhiteNoise());
      coll.add(ARMA());
      coll.add(GaussianProcess());
      AggregatedProcess myProcess(coll);
      myProcess.setTimeGrid(timeGrid);
      fullprint << "myProcess=" << myProcess << std::endl;
      fullprint << "myProcess=" << myProcess.__str__() << std::endl;
      fullprint << "a realization=" << myProcess.getRealization() << std::endl;
      Process marginalProcess(myProcess.getMarginal(1));
      fullprint << "a marginal process=" << marginalProcess << std::endl;
      Indices indices(2);
      indices[0] = 0;
      indices[1] = 2;
      fullprint << "another marginal process=" << Process(myProcess.getMarginal(indices)) << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
