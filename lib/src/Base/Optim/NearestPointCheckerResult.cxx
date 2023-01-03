//                                               -*- C++ -*-
/**
 *  @brief Result stores the result of a NearestPointChecker
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
#include "openturns/NearestPointChecker.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NearestPointCheckerResult)

/* Default constructor */
NearestPointCheckerResult::NearestPointCheckerResult():
  PersistentObject(),
  verifyingConstraintPoints_(Sample(0, 0)),
  verifyingConstraintValues_(Sample(0, 0)),
  violatingConstraintPoints_(Sample(0, 0)),
  violatingConstraintValues_(Sample(0, 0))
{
  // Nothing to do
}

/* Standard constructor */
NearestPointCheckerResult::NearestPointCheckerResult(const Sample & verifyingConstraintPoints,
    const Sample & verifyingConstraintValues,
    const Sample & violatingConstraintPoints,
    const Sample & violatingConstraintValues):
  PersistentObject(),
  verifyingConstraintPoints_(verifyingConstraintPoints),
  verifyingConstraintValues_(verifyingConstraintValues),
  violatingConstraintPoints_(violatingConstraintPoints),
  violatingConstraintValues_(violatingConstraintValues)
{
  // Nothing to do
}

/* Virtual constructor */
NearestPointCheckerResult * NearestPointCheckerResult::clone() const
{
  return new NearestPointCheckerResult(*this);
}

/* verifyingConstraintPoints accessor */
Sample NearestPointCheckerResult::getVerifyingConstraintPoints() const
{
  return verifyingConstraintPoints_;
}

/* verifyingConstraintValues accessor */
Sample NearestPointCheckerResult::getVerifyingConstraintValues() const
{
  return verifyingConstraintValues_;
}

/* violatingConstraintPoints accessor */
Sample NearestPointCheckerResult::getViolatingConstraintPoints() const
{
  return violatingConstraintPoints_;
}

/* violatingConstraintValues accessor */
Sample NearestPointCheckerResult::getViolatingConstraintValues() const
{
  return violatingConstraintValues_;
}

/* verifyingConstraintPoints accessor */
void NearestPointCheckerResult::setVerifyingConstraintPoints(const Sample & verifyingConstraintPoints)
{
  verifyingConstraintPoints_ = verifyingConstraintPoints;
}

/* verifyingConstraintValues accessor */
void NearestPointCheckerResult::setVerifyingConstraintValues(const Sample & verifyingConstraintValues)
{
  verifyingConstraintValues_ = verifyingConstraintValues;
}

/* violatingConstraintPoints accessor */
void NearestPointCheckerResult::setViolatingConstraintPoints(const Sample & violatingConstraintPoints)
{
  violatingConstraintPoints_ = violatingConstraintPoints;
}

/* violatingConstraintValues accessor */
void NearestPointCheckerResult::setViolatingConstraintValues(const Sample & violatingConstraintValues)
{
  violatingConstraintValues_ = violatingConstraintValues;
}

/* String converter */
String NearestPointCheckerResult::__repr__() const
{
  OSS oss;
  oss << "class=" << NearestPointCheckerResult::GetClassName()
      << " violatingConstraintPoints=" << violatingConstraintPoints_
      << " violatingConstraintValues=" << violatingConstraintValues_
      << " verifyingConstraintPoints=" << verifyingConstraintPoints_
      << " verifyingConstraintValues=" << verifyingConstraintValues_;
  return oss;
}

END_NAMESPACE_OPENTURNS
