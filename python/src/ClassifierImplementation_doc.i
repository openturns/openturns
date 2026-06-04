%define OT_Classifier_doc
"Base class for classification algorithms.

See also
--------
MixtureClassifier, ExpertMixture

Notes
-----
The classifier enables to define rules that assign a vector to a particular
class."
%enddef
%feature("docstring") OT::ClassifierImplementation
OT_Classifier_doc

// ---------------------------------------------------------------------

%define OT_Classifier_getNumberOfClasses_doc
"Accessor to the number of classes.

Returns
-------
n_classes : int
    The number of classes"
%enddef
%feature("docstring") OT::ClassifierImplementation::getNumberOfClasses
OT_Classifier_getNumberOfClasses_doc


// ---------------------------------------------------------------------

%define OT_Classifier_classify_doc
"Classify points according to the classifier.

Parameters
----------
input : sequence of float or 2-d a sequence of float
    A point or set of points to classify.

Returns
-------
cls : int or :class:`~openturns.Indices`
    The class index of the input points, or indices of the classes of each points."
%enddef
%feature("docstring") OT::ClassifierImplementation::classify
OT_Classifier_classify_doc

// ---------------------------------------------------------------------

%define OT_Classifier_grade_doc
"Grade points according to the classifier.

Parameters
----------
inputPoint : sequence of float or 2-d a sequence of float
    A point or set of points to grade.
k : int or sequence of int
    The class index, or class indices.

Returns
-------
grade : float or :class:`~openturns.Point`
    Grade or list of grades of each input point with respect to each class index"
%enddef
%feature("docstring") OT::ClassifierImplementation::grade
OT_Classifier_grade_doc

// ---------------------------------------------------------------------

%define OT_Classifier_getDimension_doc
"Accessor to the dimension.

Returns
-------
dim : int
    The dimension of the classifier."
%enddef
%feature("docstring") OT::ClassifierImplementation::getDimension
OT_Classifier_getDimension_doc

// ---------------------------------------------------------------------

%define OT_Classifier_setParallel_doc
"Accessor to the parallel flag.

Parameters
----------
flag : bool
    Logical value telling if the classification and grading are done in parallel. 
"
%enddef
%feature("docstring") OT::ClassifierImplementation::setParallel
OT_Classifier_setParallel_doc

// ---------------------------------------------------------------------

%define OT_Classifier_isParallel_doc
"Accessor to the parallel flag.

Returns
-------
flag : bool
    Logical value telling if the parallel mode has been activated.
"
%enddef
%feature("docstring") OT::ClassifierImplementation::isParallel
OT_Classifier_isParallel_doc
