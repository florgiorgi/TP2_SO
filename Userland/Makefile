include Makefile.inc

SAMPLE_DATA=shell.bin

all: terminal linearFDisp parabDisp testMem processR processW back processRW processWR prod cons

terminal:
	cd shell; make all

linearFDisp:
	cd linearGraph; make all

parabDisp:
	cd parabolicGraph; make all

testMem:
	cd testMemoryManager; make all

processR:
	cd processRead; make all

processW:
	cd processWrite; make all

back:
	cd background; make all

processRW:
	cd processReadAndWrite; make all

processWR:
	cd processWriteAndRead; make all

prod:
	cd producer; make all

cons:
	cd consumer; make all

clean:
	cd shell; make clean
	cd linearGraph; make clean
	cd parabolicGraph; make clean
	cd testMemoryManager; make clean
	cd processRead; make clean
	cd processWrite; make clean
	cd processReadAndWrite; make clean
	cd processWriteAndRead; make clean
	cd background; make clean
	cd producer; make clean
	cd consumer; make clean
	rm -rf *.bin


.PHONY: terminal all clean
