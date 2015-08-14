// SWIG file SORM.i

%{
#include "SORM.hxx"
%}

%include SORM_doc.i

%include SORM.hxx
namespace OT{ %extend SORM { SORM(const SORM & other) { return new OT::SORM(other); } } }
