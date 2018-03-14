// SWIG file Evaluation.i

%{
#include "openturns/Evaluation.hxx"
%}

%include Evaluation_doc.i

OTTypedInterfaceObjectHelper(Evaluation)

%include openturns/Evaluation.hxx
namespace OT{ %extend Evaluation { Evaluation(const Evaluation & other) { return new OT::Evaluation(other); } } }
