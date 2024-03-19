// SWIG file LinearModelValidation.i

%{
#include "openturns/LinearModelValidation.hxx"
%}

%include LinearModelValidation_doc.i

%include openturns/LinearModelValidation.hxx

namespace OT {%extend LinearModelValidation {LinearModelValidation(const LinearModelValidation & other){return new OT::LinearModelValidation(other);}}}
