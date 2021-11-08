// SWIG file Drawable.i

%{
#include "openturns/Drawable.hxx"
%}

%include Drawable_doc.i

OTDefaultCollectionConvertFunctions(Drawable)

OTTypedInterfaceObjectHelper(Drawable)
OTTypedCollectionInterfaceObjectHelper(Drawable)

%include openturns/Drawable.hxx

%pythoncode %{
def Drawable__repr_png_(self):
    """Get the PNG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'png':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
Drawable._repr_png_ = Drawable__repr_png_

def Drawable__repr_svg_(self):
    """Get the SVG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'svg':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
Drawable._repr_svg_ = Drawable__repr_svg_

def Drawable__repr_html_(self):
    """Get the HTML representation."""
    raise NotImplementedError
Drawable._repr_html_ = Drawable__repr_html_
%}

namespace OT{

%extend Drawable {

Drawable(const Drawable & other) { return new OT::Drawable(other); }

Drawable(PyObject * pyObj)
{
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  return new OT::Drawable( OT::convert<OT::_PyObject_,OT::Drawable>(pyObj) );
}

}
}
