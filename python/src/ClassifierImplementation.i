// SWIG file ClassifierImplementation.i

%{
#include "openturns/ClassifierImplementation.hxx"
%}

%include ClassifierImplementation_doc.i

%include openturns/ClassifierImplementation.hxx
namespace OT { %extend ClassifierImplementation { ClassifierImplementation(const ClassifierImplementation & other) { return new OT::ClassifierImplementation(other); } } }
