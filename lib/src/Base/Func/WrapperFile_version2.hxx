//                                               -*- C++ -*-
/**
 *  @brief This class provides the elementary functions for wrapper file management (version 2)
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "OTprivate.hxx"
#include "OTconfig.hxx"

#ifdef OPENTURNS_HAVE_LIBXML2

BEGIN_NAMESPACE_OPENTURNS

/* Forward declaration */
class WrapperData;
class WrapperFile;
class XMLDoc;

/** Get wrapper DTD file path */
FileName GetDTDFileName_version2();

/** Document parsing function */
WrapperData ParseDocument_version2(const XMLDoc & doc);

/** Make a new document from the internal data */
XMLDoc MakeDocument_version2(const WrapperFile & wrapperFile);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HAVE_LIBXML2 */
