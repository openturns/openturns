// SWIG file FORM.i

%{
#include "FORM.hxx"
%}

%include FORM_doc.i

%include FORM.hxx
namespace OT{ %extend FORM { FORM(const FORM & other) { return new OT::FORM(other); } } }
