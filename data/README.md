# Dataset Guide

This project uses a unified text format for all datasets consumed by `DatasetLoader`.

## 1) Standard file format

```txt
# Container_Width Container_Height Container_Depth Max_Weight
100 100 200 5000
# ID Width Height Depth Weight Value
1 10 20 10 50 100
2 15 15 15 30 60
```

Rules:
- Lines starting with `#` are comments.
- Empty/whitespace-only lines are ignored.
- Item record must follow: `ID Width Height Depth Weight Value`.

## 2) Folder structure

- `data/random/`: random datasets (50, 100, 500 items)
- `data/scenarios/`: scenario datasets (`uniform`, `diverse`, `weight_heavy`, `voluminous`)
- `data/edge_cases/`: dirty input cases (invalid size, negative weight, etc.)
- `data/academic/bedbpp/`: parsed real datasets from BED-BPP raw source
- `data/raw/`: raw source files (currently `bedbpp.json`)

## 3) Real dataset source (BED-BPP)

`parse_bedbpp.exe` reads from:
- `data/raw/bedbpp.json`

And writes standardized `.txt` files to:
- `data/academic/bedbpp/`

Notes:
- Parser converts item dimensions from mm to cm to keep runtime practical for this codebase.
- Parser writes `manifest.csv` for traceability (`order_id`, `target`, item count, exported file).
- By default parser exports a representative subset. Use `parse_bedbpp.exe --all` to export all orders.

## 4) Validation behavior in DatasetLoader

- Invalid container header -> fallback default container and warning.
- Negative item weight -> clamped to `0` and warning.
- Non-positive item dimensions -> item skipped.
- Oversized item -> skipped if no orientation can fit into container.
- Malformed data line -> skipped with warning.

## 5) Benchmark master list

`data/all_tests.txt` is auto-generated and used by `benchmark.exe`.
It includes:
- random + scenario + edge case files
- exported BED-BPP files in `data/academic/bedbpp/`

## 6) Quick setup

Run:

```bash
.\setup_data.bat
```

Then benchmark:

```bash
.\benchmark.exe
```
