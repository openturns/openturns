// SWIG file MinCopula.i

%{
#include "MinCopula.hxx"
%}

%include MinCopula_doc.i

%include MinCopula.hxx
namespace OT { %extend MinCopula { MinCopula(const MinCopula & other) { return new OT::MinCopula(other); } } }
