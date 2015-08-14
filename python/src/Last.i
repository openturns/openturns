// SWIG file Last.i

%{
#include "Last.hxx"
%}

%include Last.hxx
namespace OT{ %extend Last { Last(const Last & other) { return new OT::Last(other); } } }
