#!/bin/sh
#
# generate_performance_chart.sh,v 4.1 2001/08/19 20:37:39 coryan Exp
#

gnuplot <<_EOF_ >/dev/null 2>&1
    set xdata time
    set timefmt '%Y/%m/%d-%H:%M'
    set xlabel 'Date (YYYYMMDD)'
    set ylabel 'Throughput (Requests/Second)'
    set terminal png small color
    set yrange [0:10000]
    set output "$2"
    plot '$1' using 1:2 title '$3' w l
    exit
_EOF_
