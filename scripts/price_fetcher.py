#!/usr/bin/env python3

import argparse
from datetime import datetime, timedelta
from urllib.parse import quote_plus
import pandas as pd
import requests

def fetch_close_series(ticker: str, start: str, end: str) -> pd.Series:
    start_ts = int(datetime.strptime(start, "%Y-%m-%d").timestamp())
    end_ts   = int((datetime.strptime(end, "%Y-%m-%d") + timedelta(days=1)).timestamp())

    url = (
        "https://query1.finance.yahoo.com/v8/finance/chart/"
        f"{quote_plus(ticker)}?period1={start_ts}&period2={end_ts}"
        "&interval=1d&events=history"
    )
    headers = {
        "User-Agent": (
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
            "AppleWebKit/537.36 (KHTML, like Gecko) "
            "Chrome/115.0 Safari/537.36"
        )
    }
    resp = requests.get(url, headers=headers, timeout=30)
    resp.raise_for_status()
    data = resp.json()

    if data.get("chart", {}).get("error"):
        msg = data["chart"]["error"].get("description", "Yahoo error")
        raise RuntimeError(msg)

    result = data["chart"]["result"][0]
    ts     = result.get("timestamp", [])
    closes = result.get("indicators", {}).get("quote", [{}])[0].get("close", [])

    idx = pd.to_datetime(ts, unit="s").normalize()
    s   = pd.Series(closes, index=idx).dropna().sort_index()

    if s.empty:
        raise RuntimeError("No price data returned")

    return s


def main() -> None:
    today = datetime.utcnow().date()
    ap = argparse.ArgumentParser()
    ap.add_argument("symbol")
    ap.add_argument("--lookback", type=int, default=252,
                    help="Trading days of history (default 252)")
    args = ap.parse_args()

    start = (today - timedelta(days=int(args.lookback * 1.5))).strftime("%Y-%m-%d")
    end   = str(today)

    series = fetch_close_series(args.symbol, start, end)

    series = series.iloc[-args.lookback :]

    for price in series:
        print(price)


if __name__ == "__main__":
    main()

