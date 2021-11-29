// SWIG file Pagmo.i

%{
#include "openturns/Pagmo.hxx"
%}

%include Pagmo_doc.i

%include openturns/Pagmo.hxx
namespace OT{ %extend Pagmo { Pagmo(const Pagmo & other) { return new OT::Pagmo(other); } } }

