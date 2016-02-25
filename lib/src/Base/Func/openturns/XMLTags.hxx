//                                               -*- C++ -*-
/**
 *  @brief This file lists all the tags used into the XML files
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_XMLTAGS_HXX
#define OPENTURNS_XMLTAGS_HXX

#include "openturns/OTtypes.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Tag list */
const char * const XMLTag_command           = "command";
const char * const XMLTag_comment           = "comment";
const char * const XMLTag_computed_gradient = "computed-gradient";
const char * const XMLTag_data              = "data";
const char * const XMLTag_description       = "description";
const char * const XMLTag_external_code     = "external-code";
const char * const XMLTag_file              = "file";
const char * const XMLTag_format            = "format";
const char * const XMLTag_from              = "from";
const char * const XMLTag_function          = "function";
const char * const XMLTag_gradient          = "gradient";
const char * const XMLTag_hessian           = "hessian";
const char * const XMLTag_id                = "id";
const char * const XMLTag_in_data_transfer  = "in-data-transfer";
const char * const XMLTag_library           = "library";
const char * const XMLTag_link              = "link";
const char * const XMLTag_mode              = "mode";
const char * const XMLTag_name              = "name";
const char * const XMLTag_out_data_transfer = "out-data-transfer";
const char * const XMLTag_path              = "path";
const char * const XMLTag_provided          = "provided";
const char * const XMLTag_regexp            = "regexp";
const char * const XMLTag_state             = "state";
const char * const XMLTag_subst             = "subst";
const char * const XMLTag_to                = "to";
const char * const XMLTag_type              = "type";
const char * const XMLTag_unit              = "unit";
const char * const XMLTag_user_prefix       = "user-prefix";
const char * const XMLTag_variable          = "variable";
const char * const XMLTag_variable_list     = "variable-list";
const char * const XMLTag_version           = "version";
const char * const XMLTag_wrap_mode         = "wrap-mode";
const char * const XMLTag_wrapper           = "wrapper";

/* Attribute value list */
const char * const XMLAttr_no               = "no";
const char * const XMLAttr_yes              = "yes";
const char * const XMLAttr_in               = "in";
const char * const XMLAttr_out              = "out";
const char * const XMLAttr_dynamic_link     = "dynamic-link";
const char * const XMLAttr_static_link      = "static-link";
const char * const XMLAttr_fork             = "fork";
const char * const XMLAttr_files            = "files";
const char * const XMLAttr_pipe             = "pipe";
const char * const XMLAttr_arguments        = "arguments";
const char * const XMLAttr_socket           = "socket";
const char * const XMLAttr_corba            = "corba";
const char * const XMLAttr_shared           = "shared";
const char * const XMLAttr_specific         = "specific";
const char * const XMLAttr_line             = "line";
const char * const XMLAttr_regexp           = "regexp";

END_NAMESPACE_OPENTURNS


#endif /* OPENTURNS_XMLTAGS_HXX */
