// SWIG file MarshallOlkinCopula.i

%{
#include "openturns/MarshallOlkinCopula.hxx"
%}

%include MarshallOlkinCopula_doc.i

%include openturns/MarshallOlkinCopula.hxx
namespace OT { %extend MarshallOlkinCopula { MarshallOlkinCopula(const MarshallOlkinCopula & other) { return new OT::MarshallOlkinCopula(other); } } }
