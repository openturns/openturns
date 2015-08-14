// SWIG file FaureSequence.i

%{
#include "FaureSequence.hxx"
%}

%include FaureSequence_doc.i

%include FaureSequence.hxx
namespace OT{ %extend FaureSequence { FaureSequence(const FaureSequence & other) { return new OT::FaureSequence(other); } } }
