// SWIG file ZipfMandelbrot.i

%{
#include "ZipfMandelbrot.hxx"
%}

%include ZipfMandelbrot_doc.i

%include ZipfMandelbrot.hxx
namespace OT { %extend ZipfMandelbrot { ZipfMandelbrot(const ZipfMandelbrot & other) { return new OT::ZipfMandelbrot(other); } } }
