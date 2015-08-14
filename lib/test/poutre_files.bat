rem This file implements the 'poutre' model as an external code.

rem check that input files are here
if not exist poutre_external_infile1 (goto return_error)
if not exist poutre_external_infile2 (goto return_error)
rem fixme: check that input files have correct values
rem for and find function not implemented in wine...

rem return the result that should have been computed
echo d=3.70834294755578995945 > poutre_external_outfile

:return_error

