//                                               -*- C++ -*-
/**
 *  @brief Factory for linear model
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <fstream>
#include <cstdlib>

#include "openturns/LinearModelFactory.hxx"
#include "openturns/LinearModel.hxx"
#include "openturns/Path.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"


BEGIN_NAMESPACE_OPENTURNS

/* Constructor */
LinearModelFactory::LinearModelFactory()
{
  // Nothing to do
}

/* LinearModel creation */
LinearModel LinearModelFactory::build(const Sample & samplePred,
                                      const Sample & sampleLab,
                                      const NumericalScalar levelValue) const
{
  if (samplePred.getSize() != sampleLab.getSize()) throw InvalidArgumentException(HERE) << "Error: predictors sample must have the same size than the laboratory sample";
  String predictorFileName(samplePred.storeToTemporaryFile());
  String laboratoryFileName(sampleLab.storeToTemporaryFile());
  String resultFileName(Path::BuildTemporaryFileName("RResult.txt.XXXXXX"));
  String commandFileName(Path::BuildTemporaryFileName("RCmd.R.XXXXXX"));
  std::ofstream cmdFile(commandFileName.c_str(), std::ios::out);
  // Fill-in the command file
  cmdFile << "library(rot)" << std::endl;
  cmdFile << "options(digits=17)" << std::endl;
  cmdFile << "options(warn=-1)" << std::endl;
  cmdFile << "options(stringsAsFactors = F)" << std::endl;
  cmdFile << "samplePred <- data.matrix(read.table(\"" << predictorFileName << "\"))" << std::endl;
  cmdFile << "sampleLab <- data.matrix(read.table(\"" << laboratoryFileName << "\"))" << std::endl;
  cmdFile << "res <- computeLinearModel(samplePred, sampleLab, " << levelValue << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$parameterEstimate, res$confidenceIntervalLow, res$confidenceIntervalHigh, res$pValues, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::Get("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << RExecutable << " --no-save --silent < \"" << commandFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In LinearModelFactory::build(const Sample & samplePred, const Sample & sampleLab, const NumericalScalar levelValue) const: needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  int returnCode(Os::ExecuteCommand(systemCommand));
  if (returnCode != 0) throw InternalException(HERE) << "Error: unable to execute the system command " << String(systemCommand) << " returned code is " << returnCode;
  // Parse result file
  std::ifstream resultFile(resultFileName.c_str(), std::ios::in);
  UnsignedInteger dimension = samplePred.getDimension() + 1;
  Point regression(dimension);
  // Read the regression parameters
  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    resultFile >> regression[i];
  }
  // Read the lower bounds of the intervals
  Point lowerBounds(dimension);
  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    resultFile >> lowerBounds[i];
  }
  // Read the upper bounds of the intervals
  Point upperBounds(dimension);
  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    resultFile >> upperBounds[i];
  }
  Interval confidenceIntervals(lowerBounds, upperBounds);

  // Read the p-values of the coefficients
  NumericalScalarPersistentCollection pValues(dimension);
  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    resultFile >> pValues[i];
  }
  resultFile.close();

  Os::Remove(predictorFileName);
  Os::Remove(laboratoryFileName);
  Os::Remove(resultFileName);
  Os::Remove(commandFileName);

  return LinearModel(regression, confidenceIntervals, pValues);
}

END_NAMESPACE_OPENTURNS

