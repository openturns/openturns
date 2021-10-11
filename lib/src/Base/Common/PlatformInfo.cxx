//                                               -*- C++ -*-
/**
 *  @brief The class PlatformInfo gives information about the library
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
#include "openturns/OTconfig.hxx"
#include "openturns/OTconfigureArgs.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PlatformInfo.hxx"

BEGIN_NAMESPACE_OPENTURNS



UnsignedInteger PlatformInfo::Precision_ = 6;

PlatformInfo::PlatformInfo()
{
}


String PlatformInfo::GetVersion()
{
  return PACKAGE_VERSION;
}

String PlatformInfo::GetName()
{
  return PACKAGE_NAME;
}

String PlatformInfo::GetRevision()
{
  return Revision;
}

String PlatformInfo::GetDate()
{
  return ConfigureDate;
}

String PlatformInfo::GetInstallationDirectory()
{
  return InstallationDirectory;
}

UnsignedInteger PlatformInfo::GetNumericalPrecision()
{
  return PlatformInfo::Precision_;
}

void PlatformInfo::SetNumericalPrecision(SignedInteger precision)
{
  if (precision < 0)
    throw InvalidArgumentException(HERE) << "in SetNumericalPrecision, precision must be positive";
  PlatformInfo::Precision_ = precision;
}

END_NAMESPACE_OPENTURNS
