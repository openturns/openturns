// SWIG file MetaModelValidation.i

%{
#include "openturns/MetaModelValidation.hxx"
%}

%include MetaModelValidation_doc.i

%include openturns/MetaModelValidation.hxx

namespace OT {%extend MetaModelValidation {MetaModelValidation(const MetaModelValidation & other){return new OT::MetaModelValidation(other);}}}
