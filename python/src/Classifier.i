// SWIG file Classifier.i

%{
#include "openturns/Classifier.hxx"
%}

%include Classifier_doc.i

OTTypedInterfaceObjectHelper(Classifier)

%copyctor OT::Classifier;

%include openturns/Classifier.hxx

