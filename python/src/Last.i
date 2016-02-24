// SWIG file Last.i

%{
#include "openturns/Last.hxx"
%}

%include Last_doc.i

%include openturns/Last.hxx
namespace OT{ %extend Last { Last(const Last & other) { return new OT::Last(other); } } }
