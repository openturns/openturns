// SWIG file Study.i

%{
#include "Study.hxx"
%}

%ignore OT::Study::operator=(const Study &);

%include Study_doc.i

%include Study.hxx
namespace OT{ %extend Study { Study(const Study & other) { return new OT::Study(other); } } }
