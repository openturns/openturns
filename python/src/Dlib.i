// SWIG file Dlib.i

%{
#include "openturns/Dlib.hxx"
%}

%include Dlib_doc.i

%include openturns/Dlib.hxx

namespace OT { %extend Dlib { Dlib(const Dlib & other) { return new OT::Dlib(other); } } }
