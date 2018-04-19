// SWIG file ExtremeValueCopula.i

%{
#include "openturns/ExtremeValueCopula.hxx"
%}

%include ExtremeValueCopula_doc.i

%include openturns/ExtremeValueCopula.hxx
namespace OT { %extend ExtremeValueCopula { ExtremeValueCopula(const ExtremeValueCopula & other) { return new OT::ExtremeValueCopula(other); } } }
