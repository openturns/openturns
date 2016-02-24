// SWIG file FaureSequence.i

%{
#include "openturns/FaureSequence.hxx"
%}

%include FaureSequence_doc.i

%include openturns/FaureSequence.hxx
namespace OT{ %extend FaureSequence { FaureSequence(const FaureSequence & other) { return new OT::FaureSequence(other); } } }
