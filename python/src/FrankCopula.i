// SWIG file FrankCopula.i

%{
#include "FrankCopula.hxx"
%}

%include FrankCopula_doc.i

%include FrankCopula.hxx
namespace OT { %extend FrankCopula { FrankCopula(const FrankCopula & other) { return new OT::FrankCopula(other); } } }
