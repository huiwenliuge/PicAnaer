clv = `clang++ --version`
gpp = `g++ --version`
cpp = `c++ --version`
ifneq ("",$(clv))
	CC = clang++
else ifneq ("",$(gpp))
	CC = g++
else ifneq ("",$(cpp))
	CC = c++
endif

ALLDEP = tmp/CNNet.o tmp/debugCNN.o

ALL:$(ALLDEP)
	@echo $(CC)
	$(CC) -o bin/test $(ALLDEP) -lpthread -std=c++11
	-rm $(ALLDEP)

tmp/CNNet.o:
	$(CC) -c src/CNNet.cpp -o tmp/CNNet.o -std=c++11
tmp/debugCNN.o:
	$(CC) -c src/debugCNN.cpp -o tmp/debugCNN.o -std=c++11

.PHONY:clean
clean:
	-rm $(ALLDEP)
