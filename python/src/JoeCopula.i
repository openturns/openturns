// SWIG file JoeCopula.i

%{
#include "openturns/JoeCopula.hxx"
%}

%include JoeCopula_doc.i

%include openturns/JoeCopula.hxx
namespace OT { %extend JoeCopula { JoeCopula(const JoeCopula & other) { return new OT::JoeCopula(other); } } }
