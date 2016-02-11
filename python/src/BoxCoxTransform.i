// SWIG file BoxCoxTransform.i

%{
#include "openturns/BoxCoxTransform.hxx"
%}

%include  BoxCoxTransform_doc.i

%include openturns/BoxCoxTransform.hxx
namespace OT { %extend BoxCoxTransform { BoxCoxTransform(const BoxCoxTransform & other) { return new OT::BoxCoxTransform(other); } } }
