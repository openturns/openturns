// SWIG file NormalCopula.i

%{
#include "openturns/NormalCopula.hxx"
%}

%include NormalCopula_doc.i

%include openturns/NormalCopula.hxx
namespace OT { %extend NormalCopula { NormalCopula(const NormalCopula & other) { return new OT::NormalCopula(other); } } }
