makefile.new: makefile.header makefile.intermediate
	cat makefile.header makefile.intermediate > makefile.new

makemakefile: makemakefile.cpp
	g++ -o makemakefile makemakefile.cpp

makefile.intermediate: input.txt makemakefile
	cat input.txt | ./makemakefile > makefile.intermediate

clean:
	rm -f makemakefile makefile.new makefile.intermediate
