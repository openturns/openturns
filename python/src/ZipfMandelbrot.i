// SWIG file ZipfMandelbrot.i

%{
#include "openturns/ZipfMandelbrot.hxx"
%}

%include ZipfMandelbrot_doc.i

%include openturns/ZipfMandelbrot.hxx
namespace OT { %extend ZipfMandelbrot { ZipfMandelbrot(const ZipfMandelbrot & other) { return new OT::ZipfMandelbrot(other); } } }
