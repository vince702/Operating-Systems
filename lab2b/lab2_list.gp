#! /usr/local/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#   8. avg wait time for lock (ns)
#
# output:
#	lab2b_1.png ... num threads and throughput
#	lab2b_2.png ... num threads and avg wait for lock time
#	lab2b_3.png ... threads and iterations that run (protected) w/o failure
#	lab2b_4.png ... num threads and aggregated throughput sync = m
#.  lab2b_5.png ... num threads and aggregated throughput sync = s
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep -e to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","


set title "Lab2b-1: throughput vs. number of threads for mutex and spin-lock synchronized list operations."
set xlabel "threads"
set logscale x 2
set ylabel "throughput"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list1.csv" using ($2):(1000000000)/($7) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list1.csv" using ($2):(1000000000)/($7) \
	title 'spinlock' with linespoints lc rgb 'blue'



set title "Lab2b-2: mean time per mutex wait and mean time per operation for mutex-synchronized list operations."
set xlabel "threads"
set logscale x 2
set ylabel "time"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,'  lab2b_list2.csv " using ($2):($7) \
	title 'average time per operation' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list2.csv" using ($2):($8) \
	title 'average wait-for-mutex time' with linespoints lc rgb 'blue'




set title "Lab2b-3: successful iterations vs. threads for each synchronization method."
set xlabel "threads"
set logscale x 2
set ylabel "num iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep -e  'list-id-none,[0-9]*,[0-9]*,4' lab2b_list3.csv" using ($2):($3) \
	title 'none' with points lc rgb 'red', \
     "< grep -e 'list-id-s,[0-9]*,[0-9]*,4' lab2b_list3.csv" using ($2):($3) \
	title 'spinlock' with points lc rgb 'blue', \
	 "< grep -e 'list-id-m,[0-9]*,[0-9]*,4' lab2b_list3.csv" using ($2):($3) \
	title 'mutex' with points lc rgb 'green'



set title "Lab2b-4: throughput vs. number of threads for mutex synchronized partitioned lists."
set xlabel "threads"
set logscale x 2
set ylabel "throughput"
set logscale y 10
set output 'lab2b_4.png'

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list4.csv" using ($2):(1000000000)/($7) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,4' lab2b_list4.csv" using ($2):(1000000000)/($7) \
	title '4 lists' with linespoints lc rgb 'blue', \
	"< grep -e 'list-none-m,[0-9]*,1000,8' lab2b_list4.csv" using ($2):(1000000000)/($7) \
	title '8 lists' with linespoints lc rgb 'violet', \
     "< grep -e 'list-none-m,[0-9]*,1000,16' lab2b_list4.csv" using ($2):(1000000000)/($7) \
	title '16 lists' with linespoints lc rgb 'green'


set title "Lab2b-5: throughput vs. number of threads for spinlock synchronized partitioned lists."
set xlabel "threads"
set logscale x 2
set ylabel "throughput"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list5.csv" using ($2):(1000000000)/($7) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,4' lab2b_list5.csv" using ($2):(1000000000)/($7) \
	title '4 lists' with linespoints lc rgb 'blue', \
	"< grep -e 'list-none-s,[0-9]*,1000,8' lab2b_list5.csv" using ($2):(1000000000)/($7) \
	title '8 lists' with linespoints lc rgb 'violet', \
     "< grep -e 'list-none-s,[0-9]*,1000,16' lab2b_list5.csv" using ($2):(1000000000)/($7) \
	title '16 lists' with linespoints lc rgb 'green'

