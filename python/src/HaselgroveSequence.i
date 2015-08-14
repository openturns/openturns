// SWIG file HaselgroveSequence.i

%{
#include "HaselgroveSequence.hxx"
%}

%include HaselgroveSequence_doc.i

%include HaselgroveSequence.hxx
namespace OT{ %extend HaselgroveSequence { HaselgroveSequence(const HaselgroveSequence & other) { return new OT::HaselgroveSequence(other); } } }
