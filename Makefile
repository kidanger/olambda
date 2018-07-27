
LDFLAGS=-lm -ltiff -ljpeg -lpng -loctave -loctinterp

olambda: main.o iio.o
	$(CXX) $^ -o $@ ${LDFLAGS}

clean:
	-rm iio.o main.o olambda

