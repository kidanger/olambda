
CURDIR=$(shell pwd)
CXXFLAGS+=-I ${CURDIR}/octave-4.4.1/include/octave-4.4.1/
LDFLAGS=-lm -ltiff -ljpeg -lpng \
		-L ${CURDIR}/octave-4.4.1/lib/octave/4.4.1 \
		-Wl,-rpath=${CURDIR}/octave-4.4.1/lib/octave/4.4.1 -l octave -l octinterp

default: olambda

octave-4.4.1.tar.gz:
	wget https://ftp.igh.cnrs.fr/pub/gnu/octave/octave-4.4.1.tar.gz

octave-4.4.1/configure: octave-4.4.1.tar.gz
	tar xf $<

octave-4.4.1/Makefile: octave-4.4.1/configure
	sh -c "cd octave-4.4.1; ./configure --prefix=${CURDIR}/octave-4.4.1"

octave-4.4.1/include/: octave-4.4.1/Makefile
	$(MAKE) -C octave-4.4.1 install

olambda: main.o iio.o octave-4.4.1/include/
	$(CXX) main.o iio.o -o $@ ${LDFLAGS}

clean:
	-rm iio.o main.o olambda

