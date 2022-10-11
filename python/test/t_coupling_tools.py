#! /usr/bin/env python

import openturns.coupling_tools as ct
import os
import time
import sys
import openturns.testing as ott
import subprocess

wanted_lines = "# ooo\nE=@E\nE1=@E\nFE1=@F#oo\nZ=@Z@Z\n# ooo\n"
semi_parsed = "# ooo\nE=2\nE1=2\nFE1=@F#oo\nZ=@Z@Z\n# ooo\n"
parsed = "# ooo\nE=1.6\nE1=1.6\nFE1=5#oo\nZ=66\n# ooo\n"

# how many Mo for perf file
howbig = 128
max_time = howbig // 5
# enable it manually, see #600
max_time = sys.maxsize


def create_template():
    template_name = "template.in"

    template_handle = open(template_name, "wb")
    template_handle.write(wanted_lines.encode())
    template_handle.close()

    return template_name


def create_big_template():
    template_name = "template_big.in"

    template_handle = open(template_name, "wb")

    print(("create template file of " + str(howbig) + "Mo"))

    template_handle.write(wanted_lines.encode())
    for i in range(howbig):
        for i in range(1024):
            # line of 1024 octets
            template_handle.write(b"u" * 1024)
            template_handle.write(b"\n")
    template_handle.write(b"# ooo\n")
    template_handle.close()

    return template_name


def remove_file(filename, quiet=False):
    if quiet:
        try:
            os.remove(filename)
        except:
            pass
    else:
        os.remove(filename)


def check_outfile(filename, wanted_result):
    """wanted_result: a string"""
    is_ok = True
    handle = open(filename)
    for wanted_line, result_line in zip(wanted_result.splitlines(True), handle):
        if wanted_line != result_line:
            print(
                (
                    "Aaaaarg, result is not what we wanted (result:"
                    + result_line
                    + ", should be:"
                    + wanted_line.decode()
                    + ")"
                )
            )
            is_ok = False
    handle.close()

    if is_ok:
        print(("check " + filename + ": ok"))
    else:
        exit(1)
    # return is_ok


def check_replace():
    print(("=== " + sys._getframe().f_code.co_name))
    print("= check replace std")
    template = create_template()
    template_out = template + ".replaced"
    ct.replace(infile=template, outfile=template_out, tokens=["@E"], values=[2])
    check_outfile(template_out, semi_parsed)
    remove_file(template_out)
    remove_file(template)

    print("= check replace more vars")
    template = create_template()
    ct.replace(
        infile=template,
        outfile=template_out,
        tokens=["@E", "@F", "@Z"],
        values=[1.6, 5, 6],
    )
    check_outfile(template_out, parsed)
    remove_file(template_out)
    remove_file(template)

    print("= check replace inplace")
    template = create_template()
    ct.replace(
        infile=template, outfile=template, tokens=["@E", "@F", "@Z"], values=[1.6, 5, 6]
    )
    check_outfile(template, parsed)
    remove_file(template)

    print("= check replace inplace with None")
    template = create_template()
    ct.replace(
        infile=template, outfile=None, tokens=["@E", "@F", "@Z"], values=[1.6, 5, 6]
    )
    check_outfile(template, parsed)
    remove_file(template)

    print("= check replace big template")
    start_time = time.time()
    template = create_big_template()
    sys.stderr.write(
        "big template created in : " + str(time.time() - start_time) + "s\n"
    )

    template_out = template + ".replaced"
    start_time = time.time()
    ct.replace(infile=template, outfile=template_out, tokens=["@E"], values=[2])
    time_to_parse = str(int(time.time() - start_time))
    check_outfile(template_out, semi_parsed)
    remove_file(template_out)
    remove_file(template)
    sys.stderr.write("parsed template in: " + time_to_parse + "s\n")
    # parsed template=3G -> 25s on bx (ssd, core i7@2.5GHz)
    if int(time_to_parse) > max_time:
        print(
            ("time to get token took too long (should be " + str(max_time) + "s max)")
        )
        exit(1)
    else:
        print("check replace big template: ok")


def create_results(tokens, values=None, big=False):
    filename = "results.out"

    handle = open(filename, "wb")

    if big:
        print(("create file of " + str(howbig) + "Mo"))

        for i in range(howbig):
            for i in range(1024):
                # line of 1024 octets
                handle.write(b"u" * 1024)
                handle.write(b"\n")
        handle.write(b"# ooo\n")

    if values == None:
        handle.write(tokens.encode())
    else:
        n = 0
        for t, v in zip(tokens, values):
            handle.write((t + str(v)).encode())
            # go to next line sometimes
            if n % 3 == 0:
                handle.write(b"\n")
            n += 1
    handle.close()

    return filename


def check_results(ok_values, values):
    if ok_values == values:
        print("ok")
    else:
        print(("Error: found: " + str(values) + " should be: " + str(ok_values)))
        exit(1)


def check_get_line_col():
    print(("=== " + sys._getframe().f_code.co_name))
    content = """01 02 03 04 05 06 07 08 09
11 12 13 14 15 16 17 18 19
21 22 23 24 25 26 27 28 29
31 32 33 34 35 36 37 38 39
"""
    result_file = create_results(content)

    value = 1
    result = ct.get_line_col(result_file)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 3
    result = ct.get_line_col(result_file, skip_col=2)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 11
    result = ct.get_line_col(result_file, 1)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 16
    result = ct.get_line_col(result_file, 1, 5)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 9
    result = ct.get_line_col(result_file, skip_col=-1)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 17
    result = ct.get_line_col(result_file, 1, -3)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 31
    result = ct.get_line_col(result_file, -1)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 14
    result = ct.get_line_col(result_file, -3, -6)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 3
    result = ct.get_line_col(result_file, seek=6)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 23
    result = ct.get_line_col(result_file, skip_line=1, skip_col=2, seek=30)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))
    value = 31
    result = ct.get_line_col(result_file, skip_line=-1, seek=-100)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))

    # ct.debug = True
    value = 21
    just_before_line_ret = 80
    sys.stderr.write(
        "char at pos "
        + str(just_before_line_ret)
        + ":->"
        + content[just_before_line_ret]
        + "<-\n"
    )
    result = ct.get_line_col(result_file, skip_line=-1, seek=-just_before_line_ret)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))

    value = 21
    just_after_line_ret = just_before_line_ret + 1
    sys.stderr.write(
        "char at pos "
        + str(just_after_line_ret)
        + ":->"
        + content[just_after_line_ret]
        + "<-\n"
    )
    result = ct.get_line_col(result_file, skip_line=-2, seek=-just_after_line_ret)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))

    try:
        result = ct.get_line_col(result_file, skip_line=4)
    except:
        pass
    else:
        raise Exception("! should have fail !")

    try:
        result = ct.get_line_col(result_file, skip_line=-5)
    except:
        pass
    else:
        raise Exception("! should have fail !")

    os.remove(result_file)

    # test with a last empty line
    content = """01 02 03 04 05 06 07 08 09
11 12 13 14 15 16 17 18 19

"""
    result_file = create_results(content)

    value = 19
    result = ct.get_line_col(result_file, skip_line=-2, skip_col=-1)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))

    value = 1
    result = ct.get_line_col(result_file, skip_line=-3)
    if value != result:
        raise Exception("! got " + str(result) + " instead of " + str(value))

    try:
        result = ct.get_line_col(result_file, skip_line=-1)
    except:
        pass
    else:
        raise Exception("! should have fail !")

    os.remove(result_file)

    print("ok")


def check_get():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = ["@Y1=", "@Y2="]
    values = [5.4, 6.5]
    result_file = create_results(tokens, values)
    results = ct.get(filename=result_file, tokens=tokens)
    check_results(values, results)
    remove(result_file)

    # Y = ct.get(outfile="output.py", tokens=["@Y1=", "@Y2="],
    #                       occurrence=[0, 0], col=[0, 3], line=[0, 2])


def check_get_token_escaped():
    print(("=== " + sys._getframe().f_code.co_name))
    result_file = "results.out"
    handle = open(result_file, "wb")
    handle.write(b"FOO (BAR\n")
    handle.write(b"1.0 1.1 1.2 1.3\n")
    handle.write(b"2.0 2.1 2.2 2.3\n")
    handle.close()
    results = ct.get(
        filename=result_file, tokens=[r"FOO \(BAR"], skip_lines=[1], skip_cols=[1]
    )
    check_results([1.1], results)
    os.remove(result_file)


def check_get_regex():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = ["@E=", "02=", " 01 = "]
    values = [-9.55555e5, 8, 5.4]
    result_file = create_results(tokens, values)
    results = ct.get_regex(
        filename=result_file, patterns=[r"@E=(\R)", r"02\s*=\s*(\I)\s*", r"01 =\s*(\R)"]
    )
    check_results(values, results)
    remove_file(result_file)


def check_get_regex_perf():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = ["@E=", "02=", " 01 = "]
    values = [-9.55555e5, 8, 5.4]
    start_time = time.time()
    result_file = create_results(tokens, values, big=True)
    sys.stderr.write("big file created in : " + str(time.time() - start_time) + "s\n")
    start_time = time.time()
    results = ct.get_regex(
        filename=result_file, patterns=[r"@E=(\R)", r"02\s*=\s*(\I)\s*", r"01 =\s*(\R)"]
    )
    time_to_parse = str(int(time.time() - start_time))
    check_results(values, results)
    remove_file(result_file)
    # get file=3G -> 16s on bx (ssd, core i7@2.5GHz)
    sys.stderr.write("get regex in file in: " + time_to_parse + "s\n")
    if int(time_to_parse) > max_time:
        print(
            ("time to get token took too long (should be " + str(max_time) + "s max)")
        )
        exit(1)
    else:
        print("get regex in file: ok")


def check_get_tokens():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = ["@E=", " pp", ",virg", " normal="]
    values = [-9.55555e6, 56.666, -12345678912.2, 0]
    result_file = create_results(tokens, values)
    results = ct.get(filename=result_file, tokens=tokens)
    check_results(values, results)
    remove_file(result_file)


def check_get_tokens_skip():
    print(("=== " + sys._getframe().f_code.co_name))
    content = "@E=99 @E=-9.55555E6 pp88 pp 56.666,virg-12345678912.2 normal=0"
    values = [-9.55555e6, 56.666, -12345678912.2, 0]
    result_file = create_results(content)
    results = ct.get(
        filename=result_file,
        tokens=["@E=", "pp", ",virg", "normal="],
        skip_tokens=[1, -1, 0, 0],
    )
    check_results(values, results)
    remove_file(result_file)

    print(("=== " + sys._getframe().f_code.co_name + "2"))
    tokens = (
        "@E=99 @E=7899 pp88 pp pp\n"
        "pp999 pp56.666E-9pp,virg-12345678912.2 uu88 uuuu\n"
        "uu999uu\n"
    )
    values = [99, 56.666e-9, -12345678912.2, 999]
    result_file = create_results(tokens)
    results = ct.get(
        filename=result_file,
        tokens=["@E=", "pp", ",virg", "uu"],
        skip_tokens=[0, 4, -1, 3],
    )
    check_results(values, results)
    remove_file(result_file)


def check_get_array():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = (
        "11.0E-9 22.0crap 33.0 44.0 55.0\n"
        "11.1 22.1 33.1 44.1\n"
        "11.2 22.2 33.2 44.2"
    )
    values = [11.0e-9, 22.0, 55.0, 11.1, 33.2, 22.2, 33.2]
    result_file = create_results(tokens)
    results = ct.get(
        filename=result_file,
        skip_lines=[0, 0, 0, 1, 2, 2, -1],
        skip_cols=[0, 1, -1, 0, 2, 1, -2],
    )
    check_results(values, results)
    remove_file(result_file)


def check_get_tokens_line_col():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = (
        "11.0E-9 22.0crap 33.0 44.0 55.0\n"
        "11.1 22.1 33.1 44.1 middle\n"
        "11.2 22.2 33.2 44.2\n"
        "@E=1111.1E11 666"
    )
    values = [1111.1e11, 22.0, 33.1, 666, 33.2, 44.1, 55.0]
    result_file = create_results(tokens)
    results = ct.get(
        filename=result_file,
        tokens=["@E=", None, "@E=", "@E=", "middle", "middle", "middle"],
        skip_lines=[0, 0, -2, 0, 1, 0, -1],
        skip_cols=[0, 1, 2, 1, -2, -1, -1],
    )
    check_results(values, results)
    remove_file(result_file)


def check_get_tokens_skip_line_col():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = (
        "11.0E-9 22.0crap 33.0 44.0 55.0\n"
        "11.1 22.1 33.1 44.1 middle\n"
        "11.2 22.2 middle 33.2 44.2\n"
        "@E=1111.1E11 666\n"
        "@E=999 8888 @E=95\n"
    )
    values = [1111.1e11, 33.2, 666, 8888, 8888, 666]
    result_file = create_results(tokens)
    results = ct.get(
        filename=result_file,
        tokens=["@E=", "middle", "middle", "@E=", "@E=", "@E="],
        skip_tokens=[0, 1, -1, -2, -1, -2],
        skip_lines=[0, 0, 1, 0, 0, -1],
        skip_cols=[0, 0, 1, 1, -1, -1],
    )
    check_results(values, results)
    remove_file(result_file)


def check_get_tokens_perf():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = ["@E=", " pp", ",virg", " normal="]
    values = [-9.55555e6, 56.666, -12345678912.2, 0]
    tokens = ["@E="]
    values = [-9.55555e6]
    start_time = time.time()
    result_file = create_results(tokens, values, big=True)
    sys.stderr.write("big file created in : " + str(time.time() - start_time) + "s\n")
    start_time = time.time()
    results = ct.get(filename=result_file, tokens=tokens)
    time_to_parse = str(int(time.time() - start_time))
    check_results(values, results)
    remove_file(result_file)
    # get file=3G -> 18s on bx (ssd, core i7@2.5GHz)
    sys.stderr.write("get token in file in: " + time_to_parse + "s\n")
    if int(time_to_parse) > max_time:
        print(
            ("time to get token took too long (should be " + str(max_time) + "s max)")
        )
        exit(1)
    else:
        print("get tokens: ok")


def check_get_tokens_skip_perf():
    print(("=== " + sys._getframe().f_code.co_name))
    content = "@E=99 @E=-9.55555E6 pp88 pp 56.666,virg-12345678912.2 normal=0"
    values = [-9.55555e6, 56.666, -12345678912.2, 0]
    content = "@E=99 @E=-9.55555E6 pp88 pp 56.666,virg-12345678912.2 normal=0"
    values = [-9.55555e6]
    start_time = time.time()
    result_file = create_results(content, big=True)
    sys.stderr.write("big file created in : " + str(time.time() - start_time) + "s\n")
    start_time = time.time()
    results = ct.get(filename=result_file, tokens=["@E="], skip_tokens=[-1])
    time_to_parse = str(int(time.time() - start_time))
    check_results(values, results)
    remove_file(result_file)
    # get file=3G -> 21s on bx (ssd, core i7@2.5GHz)
    sys.stderr.write("get token skip in file in: " + time_to_parse + "s\n")
    if int(time_to_parse) > max_time:
        print(
            ("time to get token took too long (should be " + str(max_time) + "s max)")
        )
        exit(1)
    else:
        print("get tokens skip: ok")


def check_get_line_col_perf():
    print(("=== " + sys._getframe().f_code.co_name))
    tokens = (
        "11.0E-9 22.0 33.0 44.0 55.0\n"
        "11.1 22.1 33.1 44.1\n"
        "11.2 22.2 33.2 44.2"
        "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
        "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
        "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    )
    values = [22.0]
    start_time = time.time()
    result_file = create_results(tokens, big=True)
    sys.stderr.write("big file created in : " + str(time.time() - start_time) + "s\n")
    start_time = time.time()
    results = ct.get(filename=result_file, skip_lines=[-92], skip_cols=[1])
    time_to_parse = str(int(time.time() - start_time))
    check_results(values, results)
    remove_file(result_file)
    # get file=3G -> 15s on bx (ssd, core i7@2.5GHz)
    sys.stderr.write("get token skip line col in file in: " + time_to_parse + "s\n")
    if int(time_to_parse) > max_time:
        print(
            ("time to get token took too long (should be " + str(max_time) + "s max)")
        )
        exit(1)
    else:
        print("get line col: ok")


def check_execute():
    print(("=== " + sys._getframe().f_code.co_name))
    # ensure previous print is print before following command output
    sys.stdout.flush()

    if sys.platform.startswith("win"):
        ct.execute("cmd.exe /c echo hello")
    else:
        ct.execute("/bin/echo hello")

    ct.execute("echo hi", shell=True)

    if sys.platform.startswith("win"):
        ct.execute("echo hi", shell=True, hide_win=False)
    else:
        ct.execute("echo hi", shell=True, executable="/bin/bash")

    cp = ct.execute("echo hello", shell=True, capture_output=True)
    if cp.returncode != 0 or not cp.stdout.decode().startswith("hello"):
        raise Exception("ct.execute error!")

    cp = ct.execute("echo hello", shell=True, capture_output=True)
    if (
        cp.returncode != 0
        or not cp.stdout.decode().startswith("hello")
        or len(cp.stderr) > 0
    ):
        raise Exception("ct.execute error!")

    # we expect a subclass of CalledProcessError with the error stream in the exception message
    try:
        cp = ct.execute(
            sys.executable + " zebuebceb745az4f801m", shell=True, capture_output=True
        )
        raise Exception("should throw CalledProcessError")
    except subprocess.CalledProcessError:
        pass

    print("execute ok")


def check_get_value():
    with open("result.txt", "w") as f:
        f.write("name,X0,X1,X2,X3,X4,X5\n")
        f.write("val,0.125,1.1,2.3,3.5,4.7,5.99\n")
    ott.assert_almost_equal(
        ct.get_value("result.txt", skip_line=1, skip_col=3, col_sep=","), 2.3
    )
    ott.assert_almost_equal(
        ct.get_value("result.txt", skip_line=1, skip_col=-1, col_sep=","), 5.99
    )
    os.remove("result.txt")


check_execute()
check_replace()
check_get_token_escaped()
check_get_regex()
check_get_regex_perf()
check_get_line_col()
check_get_tokens()
check_get_tokens_skip()
check_get_array()
check_get_tokens_line_col()
check_get_tokens_skip_line_col()
check_get_tokens_perf()
check_get_tokens_skip_perf()
check_get_line_col_perf()
check_get_value()
