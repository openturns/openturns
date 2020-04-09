#                                               -*- Python -*-
#
# @brief Gives functions that help coupling against external code,
#   .i.e: manipulate template file.
#
# Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#

"""
External code helpers.

Provides several functions to ease wrapping of an external code:
- replace: allows to replace a value in template file
- execute: run an external code
- get: parse values from a result file
"""

import re
import shutil
import os
import shlex
import subprocess
import sys

debug = False
default_encoding = sys.getdefaultencoding()


def check_param(obj, obj_type):
    """Assert obj as type obj_type."""
    # int casts into long ; long is deprecated
    if (obj_type == int) and (sys.version_info[0] < 3):
        obj_type = long

    try:
        obj_type(obj)
    except:
        raise AssertionError("error: parameter (" +
                             str(type(obj)) + ": " + str(obj) +
                             ") must be of " + str(obj_type) + "!")


def replace(infile, outfile, tokens, values, formats=None, encoding=default_encoding):
    """
    Replace values in a file using delimiters.

    Parameters
    ----------
    infile : str
        Name of template file that will be parsed.
    outfile : str
        Name of file that will received the template parsed.
        If equal to *None* or to *infile*, the file will be replaced in-place.
    tokens : list of str
        Regexes that will be replaced.
        When dealing with overlapping tokens, longest tokens should go first.
    values : list
        Values (can be string, float, ...) that will replace
        the tokens. The list must have the same size as tokens.
    formats : list of str, optional
        A list of formats for printing the values in the parsed files at
        tokens locations
        see https://docs.python.org/2/library/string.html#formatspec
    encoding : str, optional
        File encoding
        see http://docs.python.org/2/library/codecs.html#codec-base-classes

    Raises
    ------
    AssertionError
        parameters badly set
    EOFError
        a token has not been found

    Examples
    --------
    >>> import openturns.coupling_tools as ct
    >>> # write a template file
    >>> with open('prgm.dat.in', 'w') as f:
    ...     count = f.write('E=@E_VAR F=-PF-')
    >>> # replace tokens from template
    >>> ct.replace('prgm.dat.in', 'prgm.dat',
    ...     tokens=["@E_VAR", '-PF-'], values=[1.4, '5'])
    >>> # display file
    >>> with open('prgm.dat', 'r') as f:
    ...     print(f.read())
    E=1.4 F=5
    """
    if len(tokens) != len(values):
        raise AssertionError("Error: tokens size is != values size!")
    check_param(tokens, list)
    check_param(values, list)
    if formats is not None:
        check_param(formats, list)
        if len(values) != len(formats):
            raise AssertionError("Error: values size is != formats size!")
    else:
        formats = ['{0}'] * len(values)

    inplace = False
    if (outfile is None) or (infile == outfile):
        inplace = True
        outfile = infile + ".temporary_outfile"

    infile_handle = open(infile, 'rb')
    outfile_handle = open(outfile, 'wb')

    regex_tokens = []
    found_tokens = []
    for token in tokens:
        regex_tokens.append(re.compile(token))
        found_tokens.append(False)

    for line in infile_handle:
        line = line.decode(encoding)
        i = 0
        for regex_token in regex_tokens:
            found = regex_token.search(line)
            while found is not None:
                found_tokens[i] = True
                line = line.replace(
                    found.group(0), formats[i].format(values[i]))
                found = regex_token.search(line)
            i += 1

        outfile_handle.write(line.encode(encoding))

    infile_handle.close()
    outfile_handle.close()

    for token, found_token in zip(tokens, found_tokens):
        if not found_token:
            raise EOFError("Error: token '" + token + "' not found!")

    if inplace:
        os.remove(infile)
        shutil.move(outfile, infile)


def execute(cmd, workdir=None, is_shell=False, shell_exe=None, hide_win=True,
            check_exit_code=True, get_stdout=False, get_stderr=False,
            timeout=None):
    """
    Launch an external process.

    Parameters
    ----------
    cmd : str
        Command line to execute, e.g.: "echo 42"
    workdir : str
        Current directory of the executed command.
    is_shell : bool, default=False
        If set to True, the command is started in a shell (bash).
    shell_exe : str, default=False
        path to the shell. e.g. /bin/zsh.
    hide_win : str, default=True
        Hide cmd.exe popup on windows platform.
    check_exit_code : bool, default=True
        If set to True: raise RuntimeError if return code of process != 0
    get_stdout : bool, default=False
        Whether the standard output of the command is returned
    get_stderr : bool, default=False
        Whether the standard error of the command is returned
    timeout : int
        Process timeout (Python >=3.3 only)
        On timeout and if psutil is available the children of the process
        are killed before the process itself

    Returns
    -------
    ret : int
        The exit code of the command
    stdout_data : str
        The stdout data if get_stdout parameter is set
    stderr_data : str
        The stderr data if get_stderr parameter is set

    Raises
    ------
    RuntimeError
        could not run

    Examples
    --------
    >>> import openturns.coupling_tools as ct
    >>> ret, stdout = ct.execute('echo 42', get_stdout=True, is_shell=True)
    >>> ret
    0
    >>> int(stdout)
    42
    """
    # split cmd if not in a shell before passing it to os.execvp()
    process_args = cmd if is_shell else shlex.split(cmd)

    # override startupinfo to hide windows console
    startupinfo = None
    if hide_win and sys.platform.startswith('win'):
        startupinfo = subprocess.STARTUPINFO()
        if hasattr(subprocess, 'STARTF_USESHOWWINDOW'):
            startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        else:
            startupinfo.dwFlags |= 1

    stdout = subprocess.PIPE if get_stdout else None
    stderr = subprocess.PIPE if get_stderr else None
    proc = subprocess.Popen(process_args, shell=is_shell, cwd=workdir,
                            executable=shell_exe, stdout=stdout, stderr=stderr,
                            startupinfo=startupinfo)
    if sys.version_info >= (3, 3,):
        try:
            stdout_data, stderr_data = proc.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            # kill children is psutil is available
            try:
                import psutil
            except ImportError:
                pass
            else:
                parent = psutil.Process(proc.pid)
                for child in parent.children(recursive=True):
                    child.kill()
            proc.kill()
            stdout_data, stderr_data = proc.communicate()
            raise RuntimeError('Command "' + cmd + '" times out after ' +
                               str(timeout) + 's')
    else:
        stdout_data, stderr_data = proc.communicate()
    ret = proc.poll()

    # check return code
    if check_exit_code and ret != 0:
        # append sample of stderr to the exception message if available
        err_msg = (':\n' + stderr_data[:200].decode()
                   ) if stderr_data is not None else ''
        raise RuntimeError('Command "' + cmd +
                           '" returned exit code ' + str(ret) + err_msg)

    if get_stdout and get_stderr:
        return ret, stdout_data, stderr_data
    elif get_stdout:
        return ret, stdout_data
    elif get_stderr:
        return ret, stderr_data
    return ret


def get_regex(filename, patterns, encoding=default_encoding):
    """
    Get values from a file using regex.

    Parameters
    ----------
    filename : str
        The name of the file to parse
    patterns : list of str
        Regex patterns that will permit to get the values
        see https://docs.python.org/2/library/re.html for available patterns
        The value to be searched must be surrounded by parenthesis.
    encoding : str
        File encoding
        see http://docs.python.org/2/library/codecs.html#codec-base-classes

    Returns
    -------
    results : list of float
        Each value corresponds to each pattern.
        If nothing has been found, the corresponding value is set to None.

    Raises
    ------
    AssertionError
        parameters badly set
    EOFError
        no value found

    Examples
    --------
    >>> import openturns.coupling_tools as ct
    >>> # write an output file
    >>> with open('results.out', 'w') as f:
    ...     count = f.write('@E=-9.5E3')
    >>> # parse file with regex
    >>> ct.get_regex('results.out', patterns=['@E=(\R)'])
    [-9500.0]
    """
    if not isinstance(patterns, list) or len(patterns) == 0:
        raise AssertionError("error: patterns parameter badly set!")

    results = [None] * len(patterns)

    re_patterns = []
    for pattern in patterns:
        # OT-like shortcuts
        pattern = pattern.replace('\R',
                                  '[+-]? *(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?')
        pattern = pattern.replace('\I',
                                  '[+-]? *\d+')

        re_patterns.append(re.compile(pattern))

    file_handle = open(filename, 'rb')

    for line in file_handle:
        line = line.decode(encoding)
        i = 0
        for re_pattern in re_patterns:
            match = re_pattern.search(line)
            if match:
                results[i] = float(match.group(1))
            i += 1

    file_handle.close()

    for result, pattern in zip(results, patterns):
        if result is None:
            raise EOFError('error: no pattern (' + pattern + ') found!')

    return results


def get_real_from_line(line):
    """
    Try to get a real value from the beginning of a text line.

    Parameters
    ----------
    line : str
        Line to parse

    Returns
    -------
    value : float or int
        Found value. Raise an exception if nothing found.
    """
    result = None
    # print "token=" + str(token) + ', skip_token=' + str(skip_token) + ','\
    #        'skip_line=' + str(skip_line) + ', skip_col=' + str(skip_col)

    # \S*: spaces are allowed at the beginning of the real
    real_regex = '\s*[+-]? *(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?'
    # real_regex = '[+-]? *(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?'
    re_real = re.compile(real_regex)

    # get the value
    match = re_real.match(line)
    if match:
        result = float(line[match.start():match.end()])

    if result is None:
        raise EOFError('error: real not found at the beginning of this line: '
                       '(' + line + ')!')

    return result


def read_line(handle, seek=0, encoding=default_encoding):
    """
    Get next line.

    Parameters
    ----------
    seek : int
        if seek < 0: stop reading seek before the end of the file
    encoding : str
        file encoding, see http://docs.python.org/2/library/codecs.html#codec-base-classes
    """
    line = handle.readline().decode(encoding)

    if seek < 0 and line != '':
        if handle.tell() > -seek:
            if debug:
                sys.stderr.write('line before cut ->' + line + '<-\n')

            # cut the end of the line
            line = line[:len(line) - (handle.tell() + seek)]
            # after cut, if the line is empty, add a \n to behave like
            # readline() python function
            if line == '':
                line = '\n'
            # any further read will return ''
            handle.seek(0, os.SEEK_END)

            if debug:
                sys.stderr.write('line after cut ->' + line + '<-\n')

    if debug:
        sys.stderr.write('read_line: ->' + line + '<-\n')

    return line


def get_line_col(filename, skip_line=0, skip_col=0, col_sep=None, seek=0, encoding=default_encoding):
    """
    Get a value at specific line/columns coordinates.

    Parameters
    ----------
    filename : str
        The name of the file that will be parsed
    skip_line : int, default=0
        Number of lines skipped
        If skip_line < 0: count lines backward from the end of the file.
        Be careful: a last empty line is taken into account too!
        Default: 0: no line skipped
    skip_col : int, default=0
        Number of columns skipped from the beginning or end of the line.
        If skip_col < 0: count col backward from the end of the line.
        Default: 0: no column skipped
    col_sep : str
        Column separator
        Default: None: whitespace separator, see str.split
    seek : int, default=0
        if > 0, consider the file starts at pos seek.
        if < 0, consider the file ends at pos -seek (and NOT (end-(-seek))!).
        Default: 0: consider the whole file.
    encoding : str
        File encoding
        see http://docs.python.org/2/library/codecs.html#codec-base-classes

    Returns
    -------
    result : float
        Value to retrieve

    Examples
    --------
    >>> import openturns.coupling_tools as ct
    >>> with open('results.out', 'w') as f:
    ...     count = f.write('1.1 1.2 1.3 1.4')
    >>> ct.get_line_col(filename='results.out', skip_col=2)
    1.3
    """
    check_param(filename, str)
    check_param(skip_line, int)
    check_param(skip_col, int)
    check_param(seek, int)

    handle = open(filename, 'rb')
    if seek > 0:
        handle.seek(seek)

    if debug:
        sys.stderr.write('get_line_col(skip_line=' + str(skip_line) +
                         ', skip_col=' + str(skip_col) + ', seek=' + str(seek) + ')\n')

    # skip line backward
    if skip_line < 0:
        # cache position of each beginning of line
        # last elt of the list is the last inserted
        lines_cache = []
        # determine number of elt to cache
        lines_cache_size = -skip_line

        # build lines cache
        previous_pos = handle.tell()
        line = read_line(handle, seek, encoding)
        while line != '':
            # append to cache
            lines_cache.append(previous_pos)
            if len(lines_cache) > lines_cache_size:
                # todo: list not optimized to del before the end
                del lines_cache[0]
            previous_pos = handle.tell()

            line = read_line(handle, seek, encoding)
            if debug:
                sys.stderr.write('lines_cache: ' + str(lines_cache) + '\n')

        if len(lines_cache) < lines_cache_size:
            err_msg = 'error: the file has less than ' + str(lines_cache_size) +\
                      ' lines!'
            handle.close()
            raise EOFError(err_msg)
        else:
            handle.seek(lines_cache[0])
            line_found = read_line(handle, seek, encoding)
            if debug:
                sys.stderr.write('line found: ->' + line_found + '<-\n')
    # skip line forward
    else:
        while skip_line >= 0:
            line = read_line(handle, seek, encoding)
            if skip_line > 0 and line == '':
                handle.close()
                raise EOFError(
                    'error: the file has less lines than "skip_line"!')
            skip_line -= 1
        line_found = line

    handle.close()

    # get the good col
    if skip_col != 0:
        try:
            line_found = line_found.split(col_sep)[skip_col]
        except:
            raise EOFError('error: value not found on this line: (' +
                           line_found + ')!')

    # get the value
    result = get_real_from_line(line_found)

    return result


def get_value(filename, token=None, skip_token=0, skip_line=0, skip_col=0, col_sep=None, encoding=default_encoding):
    """
    Get a value from a file using a delimiter and/or offsets.

    This function is optimized to be rather fast and takes low memory on human
    readable file.

    Parameters
    ----------
    filename : str
        The name of the file that will be parsed
    token : str
        A regex that will be searched.
        The value right after the token is returned.
        Default: None (no token searched)
    skip_token : int
        The number of tokens that will be skipped before getting
        the value. If set to != 0, the corresponding token parameter must
        not be equal to None.
        If skip_tokens < 0: count tokens backward from the end of the file.
        Default: 0: no token skipped
    skip_line : int
        Number of lines skipped from the token found.
        If corresponding token equal None, skip from the beginning of the file.
        If corresponding token != None, skip from the token.
        If skip_line < 0: count lines backward from the token or from the end
        of the file. Be careful: a last empty line is taken into account too.
        Default: 0: no line skipped
    skip_col : int
        Number of columns skipped from the token found.
        If corresponding token = None, skip words from the beginning
        of the line.
        If corresponding token != None, skip words from the token.
        If skip_col < 0: count col backward from the end of the line or from
        the token.
        Default: 0: no column skipped
    col_sep : str
        Column separator
        Default: None: whitespace separator, see str.split
    encoding : str
        File encoding
        see http://docs.python.org/2/library/codecs.html#codec-base-classes

    Returns
    -------
    value : float
        Value found

    Raises
    ------
    AssertionError
        parameters badly set
    EOFError
        no value found

    Examples
    --------
    using a single token

    >>> import openturns.coupling_tools as ct
    >>> with open('results.out', 'w') as f:
    ...     count = f.write('Y1=2.0, Y2=-6.6E56')
    >>> ct.get_value('results.out', token='Y1=')
    2.0

    using token and skip_tokens

    >>> with open('results.out', 'w') as f:
    ...     count = f.write('Y1=2.6 Y1=6.0 # temperature 2')
    >>> ct.get_value('results.out', token='Y1=', skip_token=1)
    6.0

    using column & line

    >>> with open('results.out', 'w') as f:
    ...     count = f.write('1.1 1.2 1.3 1.4')
    >>> ct.get_value(filename='results.out', skip_col=2)
    1.3
    """
    if debug:
        sys.stderr.write("get_value(token=" + str(token) +
                         ', skip_token=' + str(skip_token) +
                         ', skip_line=' + str(skip_line) +
                         ', skip_col=' + str(skip_col) + ')\n')

    # check parameters
    check_param(filename, str)
    if token is not None:
        check_param(token, str)
    elif skip_token != 0:
        raise AssertionError("error: skip_token parameter needs token "
                             "parameter to be set!")
    check_param(skip_token, int)
    check_param(skip_line, int)
    check_param(skip_col, int)

    result = None
    if not token:
        result = get_line_col(filename, skip_line, skip_col,
                              col_sep=col_sep, encoding=encoding)
    else:
        handle = open(filename, 'rb')

        re_token = re.compile(token)

        # store previous begin of line pos
        line_pos = handle.tell()

        # store token pos [elt 0: start pos of the token, elt 1: end of the
        # token]
        if skip_token >= 0:
            token_pos = None
        else:
            token_pos_cache = []

        line = handle.readline().decode(encoding)
        while line != '':
            for token_match in re_token.finditer(line):
                if skip_token > 0:
                    # token found but it's not the good one
                    skip_token -= 1
                elif skip_token == 0:
                    # token found
                    token_pos = [line_pos + token_match.start(),
                                 line_pos + token_match.end()]
                    if debug:
                        sys.stderr.write(
                            'skip_token == 0, line: ' + line + '\n')
                    break
                else:
                    # token wanted in revert order: we first cache them all
                    token_pos_cache.append([line_pos + token_match.start(),
                                            line_pos + token_match.end()])

            if skip_token >= 0 and token_pos is not None:
                # token found
                break

            line_pos = handle.tell()
            line = handle.readline().decode(encoding)

        # get the token from the cache
        if skip_token < 0 and len(token_pos_cache) >= -skip_token:
            token_pos = token_pos_cache[skip_token]

        if token_pos is None:
            handle.close()
            raise EOFError('error: no token (' + token + ') was found!')

        if skip_line == 0 and skip_col == 0:
            # get the real right after the token
            handle.seek(token_pos[1])
            line = handle.readline().decode(encoding)
            if debug:
                sys.stderr.write('first token, line_found: ' + line + '\n')
            result = get_real_from_line(line)
            handle.close()  # fixme: no multiple close?
        else:
            # get the real by skipping from the token
            if skip_line < 0 or (skip_line == 0 and skip_col < 0):
                # search before the token
                seek_pos = -token_pos[0]
                skip_line -= 1  # skip line from the token
            else:
                # search after the token
                seek_pos = token_pos[1]
            handle.close()
            result = get_line_col(
                filename, skip_line, skip_col, col_sep=col_sep, seek=seek_pos, encoding=encoding)

    return result


def get(filename, tokens=None, skip_tokens=None, skip_lines=None, skip_cols=None, encoding=default_encoding):
    """
    Get several values from a file using delimiters and/or offsets.

    This is equivalent to get_value
    Each following parameters must be a list. The first element of each list
    will be used to get the first value, the 2nd ...
    If the parameter is set to None, the list is set to the default value of
    get_value(), i.e skip_lines=None => [0, 0, ...].

    Parameters
    ----------
    filename : str
        The name of the file that will be parsed
    tokens : list of str
        see get_value
    skip_tokens : list of int
        see get_value
    skip_lines : list of int
        see get_value()
    skip_cols : list of int
        see get_value()
    encoding : str
        File encoding, see
        http://docs.python.org/2/library/codecs.html#codec-base-classes

    Returns
    -------
    result : list of float
        Values parsed

    Raises
    ------
    AssertionError
        parameters badly set
    EOFError
        no value found

    Examples
    --------
    using tokens

    >>> import openturns.coupling_tools as ct
    >>> with open('results.out', 'w') as f:
    ...     count = f.write('Y1=2.0, Y2=-6.6E2')
    >>> ct.get(filename='results.out', tokens=['Y1=', 'Y2='])
    [2.0, -660.0]
    """
    # test parameters and determine the number of value to return
    nb_values = None
    if tokens is not None:
        nb_values = len(tokens)

    err_msg = "error: skip_tokens parameter needs tokens parameter to be set!"
    if skip_tokens is not None:
        if nb_values is None:
            raise AssertionError(err_msg)
        elif nb_values != len(skip_tokens):
            raise AssertionError(err_msg)

    if skip_lines is not None:
        if nb_values is None:
            nb_values = len(skip_lines)
        elif nb_values != len(skip_lines):
            raise AssertionError(err_msg)

    if skip_cols is not None:
        if nb_values is None:
            nb_values = len(skip_cols)
        elif nb_values != len(skip_cols):
            raise AssertionError(err_msg)

    if nb_values is None:
        raise AssertionError("error: no parameters have been set!")

    # init properly every list
    if tokens is None:
        tokens = [None] * nb_values
    if skip_tokens is None:
        skip_tokens = [0] * nb_values
    if skip_lines is None:
        skip_lines = [0] * nb_values
    if skip_cols is None:
        skip_cols = [0] * nb_values

    results = [None] * nb_values

    # possible optimization: get values in one pass
    for n in range(nb_values):
        results[n] = get_value(filename,
                               tokens[n],
                               skip_tokens[n],
                               skip_lines[n],
                               skip_cols[n],
                               encoding=encoding)

    return results
