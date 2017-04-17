all:

include conf.make
include dep.make

TMP:=/tmp

all: share/opening_book/openings.book test 

test:

ifndef build_without_test
test:  bin/babychess-book
	sh/test
endif

## Non-source makefiles
##
dep.make:  conf.make
	sh/write-dep >$@

conf.make:  
	sh/write-conf >$@

## Clean
##

## Remove everything that is not a source file
develclean:  
	sh/develclean

## Remove everything that is not distributed in the source package 
confclean:  
	sh/confclean

## Remove everything that is not distributed in the source package, except the
## configuration 
distclean:  
	sh/distclean

## Remove everything that can be rebuilt without rebuilding the makefiles 
clean:
	sh/clean

## Distribution
##
dist:  
	sh/dist

## Various
##
share/opening_book/openings.book:  share/opening_book/openings.pgn bin/babychess-book
	bin/babychess-book -c share/opening_book/openings.book -m \
            share/opening_book/openings.pgn -P

po/babychess.pot: src share/notation share/board_theme sh/extract \
            share/opening_book/openings.pgn
	{ sh/write-po-cc ; sh/extract ; } >$(TMP)/babychess.po.cc
	po/update-pot

dep/strings.hh:  sh/write-strings # $(string_files)
	sh/write-strings $(string_files) >$@

bin/test_notation_data.c:  share/test_notation/data sh/write-test-notation-data
	sh/write-test-notation-data <share/test_notation/data >$@

depfiles:  dep/strings.hh

install:
	sh/install
