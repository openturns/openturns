// SWIG file NormalCopula.i

%{
#include "NormalCopula.hxx"
%}

%include NormalCopula_doc.i

%include NormalCopula.hxx
namespace OT { %extend NormalCopula { NormalCopula(const NormalCopula & other) { return new OT::NormalCopula(other); } } }
