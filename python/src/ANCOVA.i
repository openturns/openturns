// SWIG file ANCOVA.i

%{
#include "ANCOVA.hxx"
%}

%include ANCOVA_doc.i

%include ANCOVA.hxx
namespace OT{ %extend ANCOVA { ANCOVA(const ANCOVA & other) { return new OT::ANCOVA(other); } } }
