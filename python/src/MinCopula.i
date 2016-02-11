// SWIG file MinCopula.i

%{
#include "openturns/MinCopula.hxx"
%}

%include MinCopula_doc.i

%include openturns/MinCopula.hxx
namespace OT { %extend MinCopula { MinCopula(const MinCopula & other) { return new OT::MinCopula(other); } } }
