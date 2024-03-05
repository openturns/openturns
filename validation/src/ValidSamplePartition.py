#! /usr/bin/env python
# python3 validation/src/ValidSamplePartition.py

import pandas as pd
import pyextremes

fn = "python/src/usecases/wooster.csv"
full = pd.read_csv(fn, index_col=0, parse_dates=True)
full["Temperature"] = full["Temperature"].apply(lambda x: x * -1.0)
winter = full[(full.index.month >= 12) | (full.index.month < 3)]

for u in [-10.0, -20.0]:
    for r in [1, 3]:
        series = pd.Series(winter["Temperature"])
        peaks = pyextremes.get_extremes(series, "POT", threshold=u, r=f"{r}D")
        print(f"u={u} r={r} nc={len(peaks)}")
