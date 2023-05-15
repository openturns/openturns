//                                               -*- C++ -*-
/**
 *  @brief Exception thrown during a Sample evaluation to retrieve partial results
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_BATCHFAILEDEXCEPTION_HXX
#define OPENTURNS_BATCHFAILEDEXCEPTION_HXX

#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API BatchFailedException : public Exception
{

public:

  BatchFailedException(const PointInSourceFile & point,
                       const Indices & failedIndices,
                       const Description & errorDescription,
                       const Indices & succeededIndices,
                       const Sample & outputSample)
  : Exception(point),
    failedIndices_(failedIndices),
    errorDescription_(errorDescription),
    succeededIndices_(succeededIndices),
    outputSample_(outputSample)
  {
    if (failedIndices_.getSize() != errorDescription.getSize())
      throw InvalidArgumentException(HERE) << "indices size must match error size";
    if (succeededIndices_.getSize() != outputSample.getSize())
      throw InvalidArgumentException(HERE) << "indices size must match output size";
  }

  virtual ~BatchFailedException() throw() {}

  template <class T> BatchFailedException & operator << (T obj)
  {
    this->Exception::operator << (obj);
    return *this;
  }

  /** Failed indices accessor */
  void setFailedIndices(const Indices & failedIndices) {failedIndices_ = failedIndices;}
  Indices getFailedIndices() const {return failedIndices_;}

  /** Error messages accessor */
  void setErrorDescription(const Description & errorDescription) {errorDescription_ = errorDescription;}
  Description getErrorDescription() const {return errorDescription_;}

  /** Succeeded indices accessor */
  void setSucceededIndices(const Indices & succeededIndices) {succeededIndices_ = succeededIndices;}
  Indices getSucceededIndices() const {return succeededIndices_;}

  /** Succeeded evaluations accessor */
  void setOutputSample(const Sample & outputSample) {outputSample_ = outputSample;}
  Sample getOutputSample() const {return outputSample_;}

private:
  Indices failedIndices_;
  Description errorDescription_;
  Indices succeededIndices_;
  Sample outputSample_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BATCHFAILEDEXCEPTION_HXX */
