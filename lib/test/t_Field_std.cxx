//                                               -*- C++ -*-
/**
 *  @brief The test file of class Field for drawing methods
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

typedef Collection<TimeSeries> TimeSeriesCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    PlatformInfo::SetNumericalPrecision(6);

    Sample vertices(8, 3);
    vertices(1, 2) = 1.0;
    vertices(2, 1) = 1.0;
    vertices(3, 1) = 1.0;
    vertices(3, 2) = 1.0;
    vertices(4, 0) = 1.0;
    vertices(5, 0) = 1.0;
    vertices(5, 2) = 1.0;
    vertices(6, 0) = 1.0;
    vertices(6, 1) = 1.0;
    vertices(7, 0) = 1.0;
    vertices(7, 1) = 1.0;
    vertices(7, 2) = 1.0;

    IndicesCollection simplicies(6, 4);
    simplicies(0, 1) = 1;
    simplicies(0, 2) = 2;
    simplicies(0, 3) = 4;
    simplicies(1, 0) = 3;
    simplicies(1, 1) = 5;
    simplicies(1, 2) = 6;
    simplicies(1, 3) = 7;
    simplicies(2, 0) = 1;
    simplicies(2, 1) = 2;
    simplicies(2, 2) = 3;
    simplicies(2, 3) = 6;
    simplicies(3, 0) = 1;
    simplicies(3, 1) = 2;
    simplicies(3, 2) = 4;
    simplicies(3, 3) = 6;
    simplicies(4, 0) = 1;
    simplicies(4, 1) = 3;
    simplicies(4, 2) = 5;
    simplicies(4, 3) = 6;
    simplicies(5, 0) = 1;
    simplicies(5, 1) = 4;
    simplicies(5, 2) = 5;
    simplicies(5, 3) = 6;

    Mesh mesh3D(vertices, simplicies);

    UnsignedInteger s = 3;
    Sample values(Normal(s).getSample(mesh3D.getVerticesNumber()));
    Field field(mesh3D, values);
    KDTree tree(vertices);
    fullprint << "field=" << field << std::endl;
    fullprint << "input dim=" << field.getInputDimension() << std::endl;
    fullprint << "value[4]=" << field.getValueAtIndex(4) << std::endl;
    fullprint << "value[4, 0]=" << field(4, 0) << std::endl;
    fullprint << "nearest[2]=" << field.getValueAtIndex(tree.query(field[2])) << std::endl;
    fullprint << "mesh=" << field.getMesh() << std::endl;
    fullprint << "input mean=" << field.getInputMean() << std::endl;
    fullprint << "deformed=" << field.asDeformedMesh() << std::endl;
    fullprint << "description=" << field.getDescription() << std::endl;

    String fname("field.vtk");
    field.exportToVTKFile(fname);

    Field scalarField(mesh3D, Sample(vertices.getSize(), Point(1, 1.0)));
    Indices x(1, 1);
    Indices y(3);
    y[0] = 0;
    y[1] = 2;
    y[2] = 3;
    fullprint << "deformed with complement=" << scalarField.asDeformedMesh(x, y) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
