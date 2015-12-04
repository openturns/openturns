// SWIG file MeshFactoryImplementation.i

%{
#include "MeshFactoryImplementation.hxx"
%}

%include MeshFactoryImplementation_doc.i

%include MeshFactoryImplementation.hxx
namespace OT {%extend MeshFactoryImplementation {MeshFactoryImplementation(const MeshFactoryImplementation & other){return new OT::MeshFactoryImplementation(other);}}}
