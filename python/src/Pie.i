// SWIG file Pie.i

%{
#include "Pie.hxx"
%}

%include Pie_doc.i

%include Pie.hxx
namespace OT { %extend Pie { Pie(const Pie & other) { return new OT::Pie(other); } } }
