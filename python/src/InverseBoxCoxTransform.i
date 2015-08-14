// SWIG file InverseBoxCoxTransform.i

%{
#include "InverseBoxCoxTransform.hxx"
%}

%include InverseBoxCoxTransform_doc.i

%include InverseBoxCoxTransform.hxx
namespace OT { %extend InverseBoxCoxTransform { InverseBoxCoxTransform(const InverseBoxCoxTransform & other) { return new OT::InverseBoxCoxTransform(other); } } }
