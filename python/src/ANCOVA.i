// SWIG file ANCOVA.i

%{
#include "openturns/ANCOVA.hxx"
%}

%include ANCOVA_doc.i

%include openturns/ANCOVA.hxx
namespace OT{ %extend ANCOVA { ANCOVA(const ANCOVA & other) { return new OT::ANCOVA(other); } } }
