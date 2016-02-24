// SWIG file Study.i

%{
#include "openturns/Study.hxx"
%}

%ignore OT::Study::operator=(const Study &);

%include Study_doc.i

%include openturns/Study.hxx
namespace OT{ %extend Study { Study(const Study & other) { return new OT::Study(other); } } }
