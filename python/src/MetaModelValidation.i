// SWIG file MetaModelValidation.i

%{
#include "MetaModelValidation.hxx"
%}

%include MetaModelValidation_doc.i

%include MetaModelValidation.hxx

namespace OT {%extend MetaModelValidation {MetaModelValidation(const MetaModelValidation & other){return new OT::MetaModelValidation(other);}}}
