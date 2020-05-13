// SWIG file SaltelliSobolIndices.i

%{
#include "openturns/SaltelliSobolIndices.hxx"
%}

%include SaltelliSobolIndices_doc.i

%include openturns/SaltelliSobolIndices.hxx
namespace OT{ %extend SaltelliSobolIndices { SaltelliSobolIndices(const SaltelliSobolIndices & other) { return new OT::SaltelliSobolIndices(other); } } }
