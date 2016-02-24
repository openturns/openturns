// SWIG file FORM.i

%{
#include "openturns/FORM.hxx"
%}

%include FORM_doc.i

%include openturns/FORM.hxx
namespace OT{ %extend FORM { FORM(const FORM & other) { return new OT::FORM(other); } } }
