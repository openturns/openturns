// SWIG file CMinpack.i

%{
#include "openturns/CMinpack.hxx"
%}

%include CMinpack_doc.i

%include openturns/CMinpack.hxx

namespace OT { %extend CMinpack { CMinpack(const CMinpack & other) { return new OT::CMinpack(other); } } }

