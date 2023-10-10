// SWIG file common_module.i

%pythonbegin %{
from sys import version_info as _python_version_info
if _python_version_info >= (3, 8, 0):
    import os
    for p in os.environ["PATH"].split(';'):
        try:
            os.add_dll_directory(p)
        except:
            continue
%}

%module(package="openturns", docstring="Utility classes.") common
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%include TBB.i
%include Object.i
%include PersistentObject.i
%include Pointer.i
%include InterfaceObject.i
%include TypedInterfaceObject.i
%include TypedCollectionInterfaceObject.i
%include Study.i
%include StorageManager.i
#ifdef OPENTURNS_HAVE_LIBXML2
%include XMLStorageManager.i
#endif
#if defined(OPENTURNS_HAVE_HDF5) && defined(OPENTURNS_HAVE_LIBXML2)
%include XMLH5StorageManager.i
#else
%pythoncode %{
class XMLH5StorageManager:
    pass
%}
#endif
%include TTY.i
%include Log.i
%include Path.i
%include Catalog.i
%include ResourceMap.i
%include ComparisonOperatorImplementation.i
%include ComparisonOperator.i
%include Equal.i
%include Greater.i
%include GreaterOrEqual.i
%include Less.i
%include LessOrEqual.i

/* At last we include template definitions */
%include BaseCommonTemplateDefs.i
