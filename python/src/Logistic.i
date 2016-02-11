// SWIG file Logistic.i

%{
#include "openturns/Logistic.hxx"
%}

%include Logistic_doc.i

%include openturns/Logistic.hxx
namespace OT { %extend Logistic { Logistic(const Logistic & other) { return new OT::Logistic(other); } } }
