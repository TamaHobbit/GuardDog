# This makefile takes all .raw files, turns them into .num (which has frame numbers and no dates), then plots them with gnuplot

%.svg: %.raw
	cat $< | ./stripdates | ./addnr > gnuplotinput.txt
	gnuplot plotdata.gnuplot
	mv gnuplotoutput.svg $@
	rm gnuplotinput.txt

all: $(addsuffix .svg, $(basename $(wildcard *.raw)))

clean:
	rm -f *.svg
	rm -f gnuplotinput.txt
