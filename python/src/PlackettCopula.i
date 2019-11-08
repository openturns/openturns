// SWIG file PlackettCopula.i

%{
#include "openturns/PlackettCopula.hxx"
%}

%include PlackettCopula_doc.i

%include openturns/PlackettCopula.hxx
namespace OT { %extend PlackettCopula { PlackettCopula(const PlackettCopula & other) { return new OT::PlackettCopula(other); } } }
