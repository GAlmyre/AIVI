#We consider that we have a file, named stats.txt,
#in which, on each line we have three values
#frameNumber MSE PSNR
#
#This file, passed as parameter to gnuplot :
# gnuplot example3.gnuplot
# will save the curves of the MSE against time, and PSNR against time
# to the file out.png

set terminal png
set output "ENT_ENTe.png"

set xlabel "Frames"
set ylabel "ENT,ENTe"
#set xrange [0:110]
set yrange [0: 10]
set xtics 10
set ytics 1
set style line 1 lw 5
set style line 2 lw 5
plot 'stats.txt' using 1:4 with lines title 'ENT', 'stats.txt' using 1:5 with lines title 'ENTe'
