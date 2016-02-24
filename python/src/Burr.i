// SWIG file Burr.i

%{
#include "openturns/Burr.hxx"
%}

%include Burr_doc.i

%include openturns/Burr.hxx
namespace OT { %extend Burr { Burr(const Burr & other) { return new OT::Burr(other); } } }
