// SWIG file IndependentCopula.i

%{
#include "openturns/IndependentCopula.hxx"
%}

%include IndependentCopula_doc.i

%include openturns/IndependentCopula.hxx
namespace OT { %extend IndependentCopula { IndependentCopula(const IndependentCopula & other) { return new OT::IndependentCopula(other); } } }
