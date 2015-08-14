//                                               -*- C++ -*-
/**
 *  @brief The class XMLWrapperErrorHandler handles error of the DOM parser
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
#ifndef OPENTURNS_XMLWRAPPERERRORHANDLER_HXX
#define OPENTURNS_XMLWRAPPERERRORHANDLER_HXX

#include <iostream>              // for std::ostream
#include "OTprivate.hxx"
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class XMLWrapperErrorHandler
 *
 * @brief The class XMLWrapperErrorHandler handles error of the DOM parser
 * @internal
 */

class OT_API XMLWrapperErrorHandler : public XERCES_CPP_NAMESPACE_QUALIFIER DOMErrorHandler
{
public:

  /*
   * Default constructor
   */
  XMLWrapperErrorHandler();

  /* Destructor */
  //virtual ~XMLWrapperErrorHandler();

  /* Accessor */
  bool hasSeenErrors() const;
  String getErrorMessage() const;

  /* Error cleaner */
  void resetErrors();

  /* DOM 3 ErrorHandler interface implementation */
  bool handleError(const XERCES_CPP_NAMESPACE_QUALIFIER DOMError & domError);

protected:

private:

  /* Remember if errors were seen */
  Bool sawErrors_;

  /* Get the last error message */
  String errorMessage_;




}

; /* class XMLWrapperErrorHandler */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_XMLWRAPPERERRORHANDLER_HXX */
