// SWIG file MultiStart.i

%{
#include "openturns/MultiStart.hxx"
%}

%include MultiStart_doc.i

%include openturns/MultiStart.hxx
namespace OT{ %extend MultiStart { MultiStart(const MultiStart & other) { return new OT::MultiStart(other); } } }

