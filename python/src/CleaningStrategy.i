// SWIG file CleaningStrategy.i

%{
#include "openturns/CleaningStrategy.hxx"
%}

%include CleaningStrategy_doc.i

%include openturns/CleaningStrategy.hxx
namespace OT{ %extend CleaningStrategy { CleaningStrategy(const CleaningStrategy & other) { return new OT::CleaningStrategy(other); } } }
