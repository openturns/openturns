// SWIG file Logistic.i

%{
#include "Logistic.hxx"
%}

%include Logistic_doc.i

%include Logistic.hxx
namespace OT { %extend Logistic { Logistic(const Logistic & other) { return new OT::Logistic(other); } } }
