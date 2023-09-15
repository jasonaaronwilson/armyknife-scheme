all: armyknife-scheme

#CC = clang
CC = gcc

# debug information and O3 some optimization is necessary for tail
# calls.
CC_FLAGS=-g -rdynamic -O3

SRC_C = allocate.c \
	array.c \
	byte-array.c \
	closure.c \
	environment.c \
	evaluator.c \
	fatal-error.c \
	global-environment.c \
	io.c \
	main.c \
	pair.c \
	primitive.c \
	printer.c \
	reader.c \
	string-util.c

SRC_GENERATED_H = \
	allocate.h \
	array.h \
	byte-array.h \
	closure.h \
	environment.h \
	evaluator.h \
	fatal-error.h \
	global-environment.h \
	io.h \
	pair.h \
	primitive.h \
	printer.h \
	reader.h

SRC_H = ${SRC_GENERATED_H} \
	boolean.h \
	optional.h \
	scheme-symbol.h \
	string-util.h

armyknife-scheme: generate-header-files ${SRC_C} ${SRC_H}
	${CC} ${CC_FLAGS} ${SRC_C} -o armyknife-scheme
	stat --format=%s armyknife-scheme

generate-header-files: ${SRC_C}
	../c-single-source-file/generate-header-file ${SRC_C}

SYMBOL_HASH_SRC_C=string-util.c symbol-hash-main.c allocate.c fatal-error.c
SYMBOL_HASH_SRC_H=string-util.h

symbol-hash: ${SYMBOL_HASH_SRC_C} ${SYMBOL_HASH_SRC_H} 
	${CC} ${CC_FLAGS} ${SYMBOL_HASH_SRC_C} -o symbol-hash

format:
	clang-format -i ${SRC_C} ${SRC_H}

CLEAN_BINARIES = \
	a.out armyknife-scheme symbol-hash

clean:
	rm -rf *~ docs/*~ tests/*~ scheme/*~ ${CLEAN_BINARIES} TAGS doxygen-docs ${SRC_GENERATED_H}

diff: clean
	git difftool HEAD

how-big: clean
	cat ${SRC_C} ${SRC_H} | wc --lines

tags:
	etags ${SRC_C} ${SRC_H}

## TESTS = ./tests/nop-test.sh \
## 	./tests/numbers-test.sh \
## 	./tests/alignment.sh \
## 	./tests/count-down-loop.sh
## 
## #
## # I'm trying to find a sensible test strategy. Tests should look
## # pretty simple and run fast.
## #	./tests/integer-binary-operators.sh
## #
## 
## test: armyknife-scheme
## 	./run-tests.sh ${TESTS}

docs:
	doxygen


