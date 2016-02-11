// SWIG file Normal.i

%{
#include "openturns/Normal.hxx"
%}

%include Normal_doc.i

%include openturns/Normal.hxx
namespace OT { %extend Normal { Normal(const Normal & other) { return new OT::Normal(other); } } }
