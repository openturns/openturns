// SWIG file Mesh.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & coordinates ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & coordinates %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::Bool OT::Mesh::checkPointInSimplexWithCoordinates(const OT::NumericalPoint & point,
						      const OT::UnsignedInteger index,
						      OT::NumericalPoint & coordinates) const;

%{
#include "Mesh.hxx"
%}

%include Mesh_doc.i

%include Mesh.hxx

namespace OT { %extend Mesh { Mesh(const Mesh & other) { return new OT::Mesh(other); } } }
