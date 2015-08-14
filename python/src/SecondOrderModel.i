// SWIG file SecondOrderModel.i

%{
#include "SecondOrderModel.hxx"
%}

OTTypedInterfaceObjectHelper(SecondOrderModel)

%include SecondOrderModel.hxx
namespace OT{ %extend SecondOrderModel { SecondOrderModel(const SecondOrderModel & other) { return new OT::SecondOrderModel(other); } } }
