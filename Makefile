
CXXFLAGS=-I /usr/include/octave-4.2.1/ -I /usr/include/octave-4.2.1/octave/
LDFLAGS=-lm -ltiff -ljpeg -lpng /usr/lib/octave/4.2.1/liboctave.so /usr/lib/octave/4.2.1/liboctinterp.so

olambda: main.o iio.o
	$(CXX) $^ -o $@ ${LDFLAGS}

clean:
	-rm iio.o main.o olambda

