// SWIG file Pie.i

%{
#include "openturns/Pie.hxx"
%}

%include Pie_doc.i

%include openturns/Pie.hxx
namespace OT { %extend Pie { Pie(const Pie & other) { return new OT::Pie(other); } } }
