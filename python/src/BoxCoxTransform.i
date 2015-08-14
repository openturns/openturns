// SWIG file BoxCoxTransform.i

%{
#include "BoxCoxTransform.hxx"
%}

%include  BoxCoxTransform_doc.i

%include BoxCoxTransform.hxx
namespace OT { %extend BoxCoxTransform { BoxCoxTransform(const BoxCoxTransform & other) { return new OT::BoxCoxTransform(other); } } }
