// SWIG file ClassifierImplementation.i

%{
#include "ClassifierImplementation.hxx"
%}

%include ClassifierImplementation_doc.i

%include ClassifierImplementation.hxx
namespace OT { %extend ClassifierImplementation { ClassifierImplementation(const ClassifierImplementation & other) { return new OT::ClassifierImplementation(other); } } }
