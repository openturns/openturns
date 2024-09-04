# Tool to compare testsuite costs
#
# 1. run tests in reference configuration:
# $ make install && make tests && rm -rf Testing && ctest && cp Testing/Temporary/CTestCostData.txt CTestCostData0.txt
#
# 2. run tests with new code:
# $ make install && make tests && rm -rf Testing && ctest && cp Testing/Temporary/CTestCostData.txt CTestCostData1.txt
#
# 3. compare tests costs:
# $ python3 ../utils/costDiff.py CTestCostData0.txt CTestCostData1.txt


import argparse

def parse_cost(path):
    tdata = dict()
    with open(path) as cost1:
        for line in cost1.readlines():
            try:
                tag, reps, cost = line.split(" ")
                if reps == "0":
                    raise ValueError()
                tdata[tag] = float(cost)
            except ValueError:
                pass
    return tdata

def compare_cost(path1, path2):
    data1 = parse_cost(path1)
    data2 = parse_cost(path2)
    diffData = {}
    relDiffData = {}
    n_slower = 0
    n_faster = 0
    a_tol = 8e-1
    r_tol = 1e-1

    for key in data1.keys():
        if data1[key] < a_tol:
            continue
        if key in data2:
            diff = data2[key] - data1[key]
            diffData[key] = diff
            relDiff = diff / data1[key]
            if diff > a_tol and relDiff > r_tol:
                n_slower +=1
            if diff < -a_tol and -relDiff > r_tol:
                n_faster +=1
            relDiffData[key] = relDiff

    for key in diffData.keys():
        print(f"{key} diff {diffData[key]:.3f} slowdown {relDiffData[key]:.3f}")

    n = 10
    print(f"= top {n} SLOWEST tests ============")
    std = sorted(data1.items(), key=lambda item: item[1])
    slowest = dict(reversed(std[-n:]))
    for key in slowest.keys():
        print(f"{key} {data1[key]:.3f}")

    std = sorted(relDiffData.items(), key=lambda item: item[1])
    slower = dict(reversed(std[-n:]))
    faster = dict(std[:n])
    print(f"= top {n} FASTER tests ============")
    for key in faster.keys():
        print(f"{key} diff {diffData[key]:.3f} speedup {-100.0*relDiffData[key]:.3f}% (before={data1[key]:.3f} after={data2[key]:.3f})")

    print(f"= top {n} SLOWER tests ============")
    for key in slower.keys():
        print(f"{key} diff {diffData[key]:.3f} slowdown {100.0*relDiffData[key]:.3f}% (before={data1[key]:.3f} after={data2[key]:.3f})")

    print(f"==============")
    print(f"n_faster={n_faster} n_slower={n_slower}")
    print(f"==============")

def main():
    """
    entry point.
    """
    parser = argparse.ArgumentParser(description="Compare test data")
    parser.add_argument("path1", type=str, help="Path to the cost data (eg CTestCostData.txt)")
    parser.add_argument("path2", type=str, help="Path to the cost data (eg CTestCostData.txt)")
    args = parser.parse_args()
    compare_cost(**vars(args))

if __name__ == "__main__":
    main()
