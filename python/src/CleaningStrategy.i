// SWIG file CleaningStrategy.i

%{
#include "CleaningStrategy.hxx"
%}

%include CleaningStrategy_doc.i

%include CleaningStrategy.hxx
namespace OT{ %extend CleaningStrategy { CleaningStrategy(const CleaningStrategy & other) { return new OT::CleaningStrategy(other); } } }
