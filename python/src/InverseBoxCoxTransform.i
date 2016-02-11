// SWIG file InverseBoxCoxTransform.i

%{
#include "openturns/InverseBoxCoxTransform.hxx"
%}

%include InverseBoxCoxTransform_doc.i

%include openturns/InverseBoxCoxTransform.hxx
namespace OT { %extend InverseBoxCoxTransform { InverseBoxCoxTransform(const InverseBoxCoxTransform & other) { return new OT::InverseBoxCoxTransform(other); } } }
