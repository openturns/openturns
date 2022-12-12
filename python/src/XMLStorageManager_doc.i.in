%feature("docstring") OT::XMLStorageManager
"Storage manager that drives a XML file.

This class allows one to read and write objects from an XML file.

Parameters
----------
fileName : str
    Name of the file containing the study.
compressionLevel : int, optional
    Compression level of the resulting XML file, in the range 0,...,9 (0=no compression, 9=best compression).
    The compression is done only if the XML lib has been compiled with zlib support.
    The default value is given by 'XMLStorageManager-DefaultCompressionLevel' in :class:`~openturns.ResourceMap`.

See also
--------
StorageManager

Examples
--------
>>> import openturns as ot
>>> # Create a study:
>>> myStudy = ot.Study()
>>> myStudy.setStorageManager(ot.XMLStorageManager('myStudy.xml'))  # doctest: +SKIP
>>> myStudy2 = ot.Study()  # doctest: +SKIP
>>> myStudy2.setStorageManager(ot.XMLStorageManager('myStudy2.xml.gz', 5))  # doctest: +SKIP
>>> # Add an object in the study:
>>> simulationResult = ot.ProbabilitySimulationResult(ot.ThresholdEvent(), 0.5, 0.01, 150, 4)  # doctest: +SKIP
>>> myStudy.add('simulationResult', simulationResult)  # doctest: +SKIP
>>> # Save the Study in the file myStudy.xml:
>>> myStudy.save()  # doctest: +SKIP"

// ---------------------------------------------------------------------

%feature("docstring") OT::XMLStorageManager::getFileName
"Get the name of the file containing the study.

Returns
-------
fileName : str
    Name of the file containing the study."

// ---------------------------------------------------------------------

%feature("docstring") OT::XMLStorageManager::setFileName
"Set the name of the file containing the study.

Parameters
----------
fileName : str
    Name of the file containing the study."
