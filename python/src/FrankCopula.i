// SWIG file FrankCopula.i

%{
#include "openturns/FrankCopula.hxx"
%}

%include FrankCopula_doc.i

%include openturns/FrankCopula.hxx
namespace OT { %extend FrankCopula { FrankCopula(const FrankCopula & other) { return new OT::FrankCopula(other); } } }
