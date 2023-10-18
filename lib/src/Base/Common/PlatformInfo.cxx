//                                               -*- C++ -*-
/**
 *  @brief The class PlatformInfo gives information about the library
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
#include "openturns/OTconfig.hxx"
#include "openturns/OTconfigureArgs.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PlatformInfo.hxx"

BEGIN_NAMESPACE_OPENTURNS



UnsignedInteger PlatformInfo::Precision_ = 6;

std::map<String, Bool> PlatformInfo::Features_;

PlatformInfo::PlatformInfo()
{
}


String PlatformInfo::GetVersion()
{
  return PACKAGE_VERSION;
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

Description PlatformInfo::GetFeatures()
{
  if (!Features_.size())
  {
#ifdef OPENTURNS_HAVE_NLOPT
    Features_["nlopt"] = true;
#else
    Features_["nlopt"] = false;
#endif

#ifdef OPENTURNS_HAVE_CERES
    Features_["ceres"] = true;
#else
    Features_["ceres"] = false;
#endif

#ifdef OPENTURNS_HAVE_DLIB
    Features_["dlib"] = true;
#else
    Features_["dlib"] = false;
#endif

#ifdef OPENTURNS_HAVE_IPOPT
    Features_["ipopt"] = true;
#else
    Features_["ipopt"] = false;
#endif

#ifdef OPENTURNS_HAVE_BONMIN
    Features_["bonmin"] = true;
#else
    Features_["bonmin"] = false;
#endif

#ifdef OPENTURNS_HAVE_CMINPACK
    Features_["cminpack"] = true;
#else
    Features_["cminpack"] = false;
#endif

#ifdef OPENTURNS_HAVE_PAGMO
    Features_["pagmo"] = true;
#else
    Features_["pagmo"] = false;
#endif

#ifdef OPENTURNS_HAVE_PRIMESIEVE
    Features_["primesieve"] = true;
#else
    Features_["primesieve"] = false;
#endif

#ifdef OPENTURNS_HAVE_TBB
    Features_["tbb"] = true;
#else
    Features_["tbb"] = false;
#endif

#ifdef OPENTURNS_HAVE_LIBXML2
    Features_["libxml2"] = true;
#else
    Features_["libxml2"] = false;
#endif

#ifdef OPENTURNS_HAVE_HMAT
    Features_["hmat"] = true;
#else
    Features_["hmat"] = false;
#endif

#ifdef OPENTURNS_HAVE_HDF5
    Features_["hdf5"] = true;
#else
    Features_["hdf5"] = false;
#endif

#ifdef OPENTURNS_HAVE_SPECTRA
    Features_["spectra"] = true;
#else
    Features_["spectra"] = false;
#endif

#ifdef OPENTURNS_HAVE_MPC
    Features_["mpc"] = true;
#else
    Features_["mpc"] = false;
#endif

#ifdef OPENTURNS_HAVE_MPFR
    Features_["mpfr"] = true;
#else
    Features_["mpfr"] = false;
#endif

#ifdef OPENTURNS_HAVE_BOOST
    Features_["boost"] = true;
#else
    Features_["boost"] = false;
#endif

#ifdef OPENTURNS_HAVE_MUPARSER
    Features_["muparser"] = true;
#else
    Features_["muparser"] = false;
#endif

#if defined(OPENTURNS_HAVE_BISON) && defined(OPENTURNS_HAVE_FLEX)
    Features_["bison"] = true;
#else
    Features_["bison"] = false;
#endif
  }

  Description keys;
  for (std::map<String, Bool>::iterator it = Features_.begin(); it != Features_.end(); ++ it)
    keys.add(it->first);
  return keys;
}

Bool PlatformInfo::HasFeature(const String & feature)
{
  (void)GetFeatures();
  const std::map<String, Bool>::iterator & search = Features_.find(feature);
  if (search != Features_.end())
    return search->second;
  else
    throw InvalidArgumentException(HERE) << "Unknown feature: " << feature;
}

END_NAMESPACE_OPENTURNS
