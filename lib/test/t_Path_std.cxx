//                                               -*- C++ -*-
/**
 *  @brief The test file of class ResourceMap for standard methods
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // As the result of these methods is installation dependent, don't check the output
    fullprint << "Installation directory=" << Path::GetInstallationDirectory() << std::endl;
    Path::DirectoryList configDirectoryList(Path::GetConfigDirectoryList());
    for (UnsignedInteger i = 0; i < configDirectoryList.size(); ++i)
      fullprint << "configDirectoryList[" << i << "]=" << configDirectoryList[i] << std::endl;
    Path::DirectoryList list(2);
    FileName directory1(Path::CreateTemporaryDirectory("testDirectory1"));
    fullprint << "Directory 1=" << directory1 << std::endl;
    list[0] = directory1;
    FileName directory2(Path::CreateTemporaryDirectory("testDirectory2"));
    fullprint << "Directory 2=" << directory2 << std::endl;
    list[1] = directory2;
    // Create a file in dir2
    std::ofstream testFile(FileName(directory2 + FileName("/testFile")).c_str());
    testFile << "test" << std::endl;
    testFile.close();
    FileName findName(Path::FindFileByNameInDirectoryList("testFile", list));
    fullprint << "Find file=" << findName << std::endl;
    FileName fileName(Path::BuildTemporaryFileName("testFile"));
    fullprint << "Temporary file name=" << fileName << std::endl;
    Os::DeleteDirectory(directory1);
    Os::DeleteDirectory(directory2);
  }
  catch (FileOpenException & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }




  return ExitCode::Success;
}
