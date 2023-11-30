//                                               -*- C++ -*-
/**
 *  @brief The test file of class Drawable for standard methods
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
  setRandomGenerator();

  try
  {
    fullprint << "Named colors=" << Drawable::GetValidColors() << std::endl;
    fullprint << "RGB colors (31, 63, 127)=" << Drawable::ConvertFromRGB(31UL, 63UL, 127UL) << std::endl;
    fullprint << "RGB colors (0.1, 0.2, 0.3)=" << Drawable::ConvertFromRGB(0.1, 0.2, 0.3) << std::endl;
    fullprint << "RGBA colors (31, 63, 127, 191)=" << Drawable::ConvertFromRGBA(31UL, 63UL, 127UL, 191UL) << std::endl;
    fullprint << "RGBA colors (0.1, 0.2, 0.3, 0.4)=" << Drawable::ConvertFromRGBA(0.1, 0.2, 0.3, 0.4) << std::endl;
    fullprint << "HSV colors (215.0, 0.2, 0.3)=" << Drawable::ConvertFromHSV(215.0, 0.2, 0.3) << std::endl;
    fullprint << "HSVA colors (215.0, 0.2, 0.3, 0.4)=" << Drawable::ConvertFromHSVA(215.0, 0.2, 0.3, 0.4) << std::endl;
    fullprint << "HSV (215.0, 0.2, 0.3) to RGB=" << Drawable::ConvertFromHSVIntoRGB(215.0, 0.2, 0.3) << std::endl;
    fullprint << "RGB (44, 160, 72) to HSV=" << Drawable::ConvertFromRGBIntoHSV(44UL, 160UL, 72UL) << std::endl;
    fullprint << "RGB (44, 72, 160) to HSV=" << Drawable::ConvertFromRGBIntoHSV(44UL, 72UL, 160UL) << std::endl;
    fullprint << "RGB (160, 44, 72) to HSV=" << Drawable::ConvertFromRGBIntoHSV(160UL, 44UL, 72UL) << std::endl;
    fullprint << "RGB (44.0/255.0, 160.0/255.0, 72.0/255.0) to HSV=" << Drawable::ConvertFromRGBIntoHSV(44.0 / 255.0, 160.0 / 255.0, 72.0 / 255.0) << std::endl;
    fullprint << "RGB (44.0/255.0, 72.0/255.0, 160.0/255.0) to HSV=" << Drawable::ConvertFromRGBIntoHSV(44.0 / 255.0, 72.0 / 255.0, 160.0 / 255.0) << std::endl;
    fullprint << "RGB (160.0/255.0, 44.0/255.0, 72.0/255.0) to HSV=" << Drawable::ConvertFromRGBIntoHSV(160.0 / 255.0, 44.0 / 255.0, 72.0 / 255.0) << std::endl;
    fullprint << "Line styles=" << Drawable::GetValidLineStyles() << std::endl;
    fullprint << "Point styles=" << Drawable::GetValidPointStyles() << std::endl;
    fullprint << "Fill styles=" << Drawable::GetValidFillStyles() << std::endl;
    fullprint << "BuildDefaultPalette=" << Drawable::BuildDefaultPalette(10) << std::endl;
    fullprint << "BuildRainbowPalette=" << Drawable::BuildRainbowPalette(10) << std::endl;
    fullprint << "BuildTableauPalette=" << Drawable::BuildTableauPalette(10) << std::endl;
    fullprint << "BuildDefaultPalette=" << Drawable::BuildDefaultPalette(20) << std::endl;
    fullprint << "BuildTableauPalette=" << Drawable::BuildTableauPalette(20) << std::endl;
    fullprint << "BuildViridisPalette=" << Drawable::BuildViridisPalette(20) << std::endl;
    fullprint << "BuildGrayScalePalette=" << Drawable::BuildGrayScalePalette(20) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
