// SWIG file SamplePartition.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::SamplePartition & clusters ($*ltype temp) %{ temp = OT::SamplePartition(); $1 = &temp; %}
%typemap(argout) OT::SamplePartition & clusters %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SamplePartition(*$1), SWIGTYPE_p_OT__SamplePartition, SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/SamplePartition.hxx"
%}

%include SamplePartition_doc.i

%copyctor OT::SamplePartition;

%include openturns/SamplePartition.hxx

%pythoncode %{
def __SamplePartition_ExtractFromDataFrame(full, partial):
    """
    Extract a partition from a pandas dataframe as a SamplePartition.

    Parameters
    ----------
    full : pandas DataFrame
        Full data
    partial : pandas DataFrame
        Filtered data from the full Dataframe

    Returns
    -------
    partition : :class:`openturns.experimental.SamplePartition`
        The resulting partition
    """

    # retrieve the partial indices wrt to the global indices in the full data
    partial_indices = [i for i, x in enumerate(full.index.isin(partial.index)) if x]

    # convert the full dataframe to a Sample
    full_sample = openturns.typ.Sample.BuildFromDataFrame(full)

    return openturns.experimental.SamplePartition(full_sample, partial_indices)
SamplePartition.ExtractFromDataFrame = __SamplePartition_ExtractFromDataFrame
%}
