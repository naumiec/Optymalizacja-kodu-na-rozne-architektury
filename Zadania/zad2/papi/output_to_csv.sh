#!/bin/bash

for f in ./output_ge*.m ./output_o2_ge*.m; do
    echo "Converting $f"
    f_csv="${f%.*}.csv"
    echo "Matrix dimensions: m = n = k, Time (s), Total Cycles, Total Instructions, L1 Cache Misses, L2 Cache Hits, Checksum" > $f_csv
    cat $f | sed '1d' | awk -F, '{print $1 "," $2 "," $3 "," $4 "," $5 "," $6 "," $7}' >> $f_csv
    echo "Saved as $f_csv"
done
