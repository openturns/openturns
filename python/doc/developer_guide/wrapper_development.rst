Wrapper development
===================

Pure python wrappers
--------------------

Python wrappers aim to be an easy way for wrapping external code. The
external code can be an analytical mathematical formula or a coupling
involving several computational codes dedicated to the resolution of a
very complex physical problem.

Python wrappers are not the best solution if your external code last
less than a microseconds and if you need to resolve billions of points.
In that case, for better performance, consider using the library
wrapper. In any other cases, Python wrapper is the recommended choice.
For further details on speed optimization see paragraph [speedo].

On OpenTURNS, two Python wrappers are available to wrap an external
code:

-  the PythonFunction is a simple monothreaded Python wrapper.

-  the DistributedPythonFunction is a Python wrapper than can launch
   code in parallel on the local machine or deploy it among several
   computers.

These two methods will be described in the following sections.

PythonFunction
~~~~~~~~~~~~~~

A PythonFunction is a NumericalMathFunction where the ``_exec`` or
``_exec_sample`` function are launched in a Python interpreter. Here is
an example of how to implement it:

Some explanations of the code :

-  The ``compute_point`` function constructs the out point from the in
   point. The in and out array size must correspond to the sizes set in
   the PythonFunction constructor. In this example, X will be an array
   of size 2 and Y must be an array of size 1. The output point can be a
   Python list, an  NumericalPoint or a Numpy array.

-  Construct the PythonFunction by passing function reference.

The ``_exec_sample`` function can be implemented to speedup the compute
using vectorization on large sample. It receives an input sample and
must return an output sample. For further details on speed optimization
see paragraph [speedo]. Here is an example using ``_exec_sample``
function:

The output sample can be a NumericalSample, a Python list of list or a
Numpy array of array. This function is optional. If ``_exec_sample`` is
not implemented and  must compute a sample, the ``_exec`` function is
called several time: once for each point of the sample. On contrary, if
only ``_exec_sample`` is given and a point must be computed, the point
is inserted in a sample of size 1, computed through ``_exec_sample`` and
extracted from the result sample.

The PythonFunction is quite simple to use. When used with
coupling\_tools module, it can wrap external program easily too.

External code coupling tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Simple example
^^^^^^^^^^^^^^

Here is a simple example of wrapper where compute is made in an external
program with the help of openturns.coupling\_tools module:

Some explanations of the code :

-  ``coupling_tools.replace`` replace ``@E`` and ``@F`` occurence found
   in ``input_template.py`` file and write the result to ``infile``
   file. ``X[0]`` value will replace ``'@E'`` token and ``X[1]`` will
   replace ``'@F'`` token.

-  The external program is launched. The input filename is passed by
   parameters to the program.

-  ``coupling_tools.get`` get the value following ``'Z='`` token in
   ``output.py`` file.

Template file example:

External program example:

More examples
^^^^^^^^^^^^^

The replace function
''''''''''''''''''''

can edit file in place. It can format values in anyway. Actually, values
can be of type “string”, if not, they are converted using str() Python
function:

--------------

*replace(infile, outfile, tokens, values, encoding=default\_encoding)*

--------------

*infile*: template file that will be parsed

*outfile*: file that will received the template parsed. If equal to None
or to *infile*, the result file will be moved to infile

*tokens*: a list of regex that will be replaced

*values*: list of values (can be string, float, ...) that will replace
the tokens. The list must have the same size as tokens

*encoding*: the file encoding, as string, i.e. ascii, latin\_1, utf\_8,
...

::

      replace(outfile='input_template.py', infile=None,
      tokens=['@E', '@F'], values=['%.2f' % 5.2569, 'toto'])

::

      replace(outfile='input_template.py', infile=None,
      tokens=['@E', '@F'], values=['%.2f' % 5.2569, 'toto'])

The input\_template.py file will then be modify like this :

::

      E=5.25
      F=toto

Be careful with overlapping tokens:

::

      # if input_template.py = 'E=@E EE=@EE')
      replace(infile="input_template.py",
      outfile="None",
      tokens=["@E", "@EE"],
      values=[1, 2])
      # => raise exception!! -> @EE token not found!
      # (this is due to the first pass with token "@E" that modify
      # "input_template.py" like this : 'E=1 EE=1E')

Solution to overlapping tokens: put longest tokens first:

::

      # template.in = 'E=@E EE=@EE')
      replace(infile="template.in",
      outfile="prgm_data.in",
      tokens=["@EE", "@E"],
      values=[2, 1])
      # => prgm_data.in = 'E=1 EE=2')

The execute function
''''''''''''''''''''

can launch an external code.

--------------

*execute(cmd, workdir=None, is\_shell=False, shell\_exe=None,
hide\_win=True, check\_exit\_code=True, get\_stdout=False,
get\_stderr=False)*

--------------

*cmd*: a string representing the command. e.g.: ’ls -l /home’

*workdir*: set the current directory of the executed command

*is\_shell*: if set to True, the command is started in a shell (bash).
default: False.

*shell\_exe*: path to the shell. e.g. /bin/zsh. default: None:
/bin/bash.

*hide\_win*: hide cmd.exe popup on windows

*check\_exit\_code*: if set to True: raise a RuntimeError exception if
return code of process != 0

*get\_stdout*: whether standard output of the command is returned

*get\_stderr*: whether standard error of the command is returned

--------------

the exit code of the command

the stdout data if get\_stdout parameter is set

the stderr data if get\_stderr parameter is set

The get\_value function
'''''''''''''''''''''''

can deal with several type of output file.

--------------

*get\_value(filename, token=None, skip\_token=0, skip\_line=0,
skip\_col=0, encoding=default\_encoding)*

--------------

*filename*: a file that will be parsed

*token*: a regex that will be searched. The value right after the token
is returned. Default: None (no token searched)

*skip\_token*: the number of tokens that will be skipped before getting
the value. If set to != 0, the corresponding token parameter must not be
equal to None. If skip\_tokens < 0: count tokens backward from the end
of the file. Default: 0: no token skipped

*skip\_line*: number of lines skipped from the token found. If
corresponding token equal None, skip from the beginning of the file. If
corresponding token != None, skip from the token. If skip\_line < 0:
count lines backward from the token or from the end of the file. Be
careful: a last empty line is taken into account too. Default: 0: no
line skipped

*skip\_col*: number of columns skipped from the token found. If
corresponding token = None, skip words from the beginning of the line.
If corresponding token != None, skip words from the token. If skip\_col
< 0: count col backward from the end of the line or from the token.
Default: 0: no column skipped

*encoding*: the file encoding, as string, i.e. ascii, latin\_1, utf\_8,
...

--------------

a real value

-  content of the results.out file used for the following examples

   ::

           1  2  3  04  5  6
           7  8  9  10
           11 12 13 14

           @Y1= 11.11celcius
           @Y2= -0.89
           @Y1= 22.22
           @Y1= 33.33

           line1: 100 101 102
           line2: 200 201 202
           line3: 300 301 302
         

-  | search token, the value right after the token is returned:

   ::

           Y = get_value('results.out', token='@Y1=') # 11.11
         

-  | skip lines and columns (useful for array search):

   ::

           get_value('results.out', skip_line=1, skip_col=2) # 9
         

-  | skip lines and columns backward (be careful: if there is an empty
     line at the end of the file, it is taken into account. i.e. this
     last empty line will be reached using skip\_line=-1):

   ::

           get_value('results.out', skip_line=-2, skip_col=-2) # 201
         

-  | search the 3rd appearance of the token:

   ::

           get_value('results.out', token='@Y1=', skip_token=2) # 33.33
         

-  | search the 2nd appearance of the token from the end of the file:

   ::

           get_value('results.out', token='@Y1=', skip_token=-2) # 22.22
         

-  | search a token and then skip lines and columns from this token:

   ::

           get_value('results.out', token='@Y1=', skip_line=5, skip_col=-2) # 101
         

-  | search the 2nd token and then skip lines and columns from this
     token:

   ::

           get_value('results.out', token='@Y1=', skip_token=1, skip_line=5, skip_col=1) # 300
         

The get function
''''''''''''''''

| works actually the same way the get\_value function do, but on several
  parameters:

--------------

*get(filename, tokens=None, skip\_tokens=None, skip\_lines=None,
skip\_cols=None, encoding=default\_encoding)*

--------------

*filename*: a file that will be parsed

*tokens*: see [getvalue] function

*skip\_tokens*: see [getvalue] function

*skip\_lines*: see [getvalue] function

*skip\_cols*: see [getvalue] function

*encoding*: the file encoding, as string, i.e. ascii, latin\_1, utf\_8,
...

--------------

a list of real values.

::

      get('results.out', tokens=['@Y1=', '@Y2'], skip_lines=[5, 0], skip_cols=[-2, 0]) # [101, -0.89]

The get\_regex function
'''''''''''''''''''''''

| parses the outfile. It is provided for backward compatibility:

--------------

*get\_regex(filename, patterns)*

--------------

*filename*: the file to parse

*patterns*: a list of patterns that will permit to get the values. \\\\R
and \\\\I can be used to match float and integer. \\\\s can be used to
match any whitespace character (= [ \\\\t\\\\n\\\\r\\\\f\\\\v]) \\\\S
can be used to match any non-whitespace character. The value to be
searched must be surrounded by ’(’ and ’)’ (see example).

--------------

a list of values corresponding to each pattern. If nothing has been
found, the corresponding value is set to None.

::

      Y = get_regex('results.out', patterns=['@Y2=(\R)']) # -0.89

Reference
^^^^^^^^^

Most up to date coupling tools module documentation is available through
docstring in Python console:

::

      import openturns as ot
      help(ot.coupling_tools.get_value)

Or in IPython console:

::

      ot.coupling_tools.replace?

Performance considerations[speedo]
----------------------------------

Two differents cases can be encounter when wrapping code: the wrapping
code is an analytical mathematical formula or it is an external code (an
external process).

Analytical formula
~~~~~~~~~~~~~~~~~~

A benchmark involving the differents wrapping methods available from
 has been done using a dummy Analytical formula.

Benchmark sources
^^^^^^^^^^^^^^^^^

Optimizations of any parts of this benchmark are welcome.

-  Benchmark of PythonFunction using \_exec function:

   ::

        big_sample = ot.Normal(2).getSample(1000*1000)
        import openturns as ot

        def _exec( X ):
            return [math.cos(pow(X[0]+1, 2)) - math.sin(X[1])]

            model = ot.PythonFunction(2, 1, _exec)
            # start timer
            out_sample = model( big_sample )
            # stop timer

-  Benchmark of PythonFunction using \_exec\_sample function:

   ::

        def _exec_sample( Xs ):
            import numpy as np
            XsT = np.array(Xs).T
            return np.atleast_2d(np.cos(np.power(xT[0]+1, 2)) - np.sin(xT[1])).T

            model = ot.PythonFunction(2, 1, func_sample=_exec_sample)


-  Benchmark of Analytical (muParser) function:

   ::

        model = ot.NumericalMathFunction( ('x0','x1'), ('y',),
        ('cos((x0+1) ^ 2) - sin(x1)',) )


The benchmark is done on a bi XEON E5520 (Nehalem 16\*2.27GHz, HT
activated) with 12Go RAM.

Benchmark results
^^^^^^^^^^^^^^^^^

:

The sample containing 1 million of points is allocated in 0.282s.

+---------------------------------+---------+-----------------------------------+
| wrapper type                    | time    | comparison with fastest wrapper   |
+---------------------------------+---------+-----------------------------------+
| PythonFunction \_exec           | 7.1s    | x157                              |
+---------------------------------+---------+-----------------------------------+
| PythonFunction \_exec\_sample   | 1.3s    | x30                               |
+---------------------------------+---------+-----------------------------------+
| Analytical (muParser)           | 0.43s   | x10                               |
+---------------------------------+---------+-----------------------------------+

The previous results are linear to the size of the sample.

-  muParser is the 2nd fastest (10 times slower than the first).

   The muParser library used is not multithreaded. Embedding a
   parallel version of muParser could give better results.

-  Using an optimized \_exec\_sample python function through numpy gives
   better results (6x faster) than a simple \_exec python function, but
   it is still much slower than the compiled library (30 times slower).

   Note that neither Python nor NumPy are multithreaded.

Conclusion
^^^^^^^^^^

PythonFunction is the easiest and more adaptable wrapper but it’s the
slowest too. So, if you need to compute samples containing less than a
million of points, PythonFunction is the good choice as the speed
difference between wrappers will not be noticeable: every wrappers will
compute the sample in less than a second. Otherwise choose muParser.

External process
~~~~~~~~~~~~~~~~

Normal program
^^^^^^^^^^^^^^

For usual program (compute time of 1s and above), inner wrapper
complexity/overhead are not an issue cause the external program compute
time will be the main part of the whole compute time. Sample can be
computed faster by launching this external program in parallel.

-  PythonFunction can not launch the \_exec function in parallel.

-  the DistributedPythonFunction from otdistfunc module can launch
   external program on each core of the local Machine or on each core of
   several remote machine.

The DistributedPythonFunction is the best choice as it combine the ease
of use of Python with the ability to deploy compute on a cluster of
computers.

Tiny program
^^^^^^^^^^^^

If the external process compute time is really fast (< 0.1s),  wrapper
point’s launch time (overhead) becomes important.

If performance are an issue, one should first consider that the external
process is perhaps fast because it does something simple: can it be
easily reimplemented in Python? If the code is not too complex, execute
Python code inside a PythonFunction is usually much faster than the time
to start the external process ( 1000x). Here is a naive example of
external process (scilab) vs PythonFunction.

-  The following scilab script takes 0.07s per point:

   ``$ scilab -nb -nwni -f code.sce``

   ::

           // code.sce
           exec("input.data", -1)
           y = x1 + x2;
           f = mopen("result.data", "wt");
           mfprintf(f, "y = %.20e", y);
           file("close", f);
           quit
         

-  Conversion to Python of the scilab script. It takes now 0.00001s per
   point:

   ::

        def _exec( X ):
            return X[0] + X[1]
            model = ot.PythonFunction(2, 1, _exec)


If you still need to launch tiny external process, slow overhead and
parallel ability are the important factors of the wrapper. Comparison of
the differents wrapper compute time with a sample of size 1000 and an
external code that last 0.07s per point on a 8 cores computer:

-  PythonFunction overhead is really slow (0.000004s) but can not launch
   the \_exec function in parallel.

   :math:`(0.000004+0.07)*1000 => 70s`

-  DistributedPythonFunction overhead is near 0.05s and can launch
   external program in parallel.

   :math:`(0.05+0.07)*1000 (/8core) => 15s`

-  PythonFunction that reimplement the external program.

   :math:`(0.00001)*1000 => 0.01s`
