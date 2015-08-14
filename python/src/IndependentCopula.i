// SWIG file IndependentCopula.i

%{
#include "IndependentCopula.hxx"
%}

%include IndependentCopula_doc.i

%include IndependentCopula.hxx
namespace OT { %extend IndependentCopula { IndependentCopula(const IndependentCopula & other) { return new OT::IndependentCopula(other); } } }
