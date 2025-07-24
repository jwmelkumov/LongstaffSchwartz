#!/usr/bin/env python3

import argparse
import pandas as pd
import matplotlib.pyplot as plt

plt.rcParams["text.usetex"] = False 

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("csv",
                    help="CSV written by LongstaffSchwartz"
                         "(<symbol>_<start>_<maturity>_sim.csv)")
    ap.add_argument("--outfile", default="sim_paths.png",
                    help="File name for PNG output (default sim_paths.png)")
    ap.add_argument("--dpi", type=int, default=400,
                    help="PNG resolution (default 400)")
    args = ap.parse_args()

    df = pd.read_csv(args.csv, header=None) 
    df.plot(legend=False, linewidth=0.4, alpha=0.7)
    plt.xlabel(r"\textbf{Time step}")
    plt.ylabel(r"\textbf{Asset price}")
    plt.tight_layout()
    plt.savefig(args.outfile, dpi=args.dpi)

if __name__ == "__main__":
    main()

