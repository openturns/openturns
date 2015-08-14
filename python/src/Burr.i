// SWIG file Burr.i

%{
#include "Burr.hxx"
%}

%include Burr_doc.i

%include Burr.hxx
namespace OT { %extend Burr { Burr(const Burr & other) { return new OT::Burr(other); } } }
