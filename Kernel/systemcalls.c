#include "converter.h"
#include "interrupts.h"
#include "heap.h"
#include "keyboard_driver.h"
#include "pipe.h"
#include "process.h"
#include "scheduler.h"
#include "string.h"
#include "time.h"
#include "types.h"
#include "video_driver.h"
#include "mutex.h"

static void * linearGraph = (void *) 0x800000;
static void * parabolicGraph = (void *) 0x900000;
static void * processRead = (void *) 0xA00000;
static void * testMemoryManager = (void *) 0xB00000;
static void * processWrite = (void *) 0xC00000;
static void * background = (void *) 0xD00000;
static void * processReadAndWrite = (void *) 0xE00000;
static void * processWriteAndRead = (void *) 0xF00000;
static void * producer = (void *) 0xF10000;
static void * consumer = (void *) 0xF20000;

typedef qword (*sys)(qword rsi, qword rdx, qword rcx, qword r8, qword r9);

static sys sysCalls[26]; 

void sys_write(qword buffer, qword size, qword rcx, qword r8, qword r9) {
	print_char(buffer);
}

void sys_clear(qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	clear_screen();
}

void sys_read(qword file, qword buffer, qword size, qword r8, qword r9){
	readBuffer((char*) buffer,(int) size);   
}

void sys_fontColor(qword color, qword rdx, qword rcx, qword r8, qword r9){
	changeFontColor(color);  
}

void sys_nextLine(qword rsi, qword rdx, qword rcx, qword r8, qword r9){
    nextLine();
}

void sys_delete(qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	delete();
}

void sys_pixel(qword x, qword y, qword rcx, qword r8, qword r9){
    draw_pixel(x, y);
}

void sys_time(qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	printTime();
}

qword sys_malloc(qword size, qword rdx, qword rcx, qword r8, qword r9){
	void * pointer = malloc_heap(size);
	return (qword) pointer;
}

void sys_printHex(qword pointer, qword rdx, qword rcx, qword r8, qword r9){
	printHex(pointer);
}

int sys_createProcess(qword processName, qword rdx, qword rcx, qword r8, qword r9){
	char * process = (char *) processName;
	if(strcmp(process,"linearGraph&")){
		createProcess(linearGraph, process);
	} else if(strcmp(process,"parabolicGraph&")){
		createProcess(parabolicGraph, process);
	} else if(strcmp(process,"processRead&")){
		createProcess(processRead, process);
	} else if(strcmp(process,"processRead")){
		createProcess(processRead, process);
	} else if(strcmp(process,"testMemoryManager&")){
		createProcess(testMemoryManager, process);
	} else if(strcmp(process,"processWrite")){
		createProcess(processWrite, process);
	} else if(strcmp(process,"background")){
		createProcess(background, process);
	} else if(strcmp(process,"background&")){
		createProcess(background, process);
	} else if(strcmp(process,"processReadAndWrite")){
		createProcess(processReadAndWrite, process);
	} else if(strcmp(process,"processWriteAndRead&")){
		createProcess(processWriteAndRead, process);
	} else if(strcmp(process,"processWriteAndRead")){
		createProcess(processWriteAndRead, process);
	} else if(strcmp(process,"producer")){
		createProcess(producer, process);
	} else if(strcmp(process,"consumer&")){
		createProcess(consumer, process);
	} else
		return -1;
	return 0;
}

void sys_ls(qword pointer, qword rdx, qword rcx, qword r8, qword r9){
	printAllCurrentProcess();
}

void sys_pkill(qword pid, qword rdx, qword rcx, qword r8, qword r9){
	disableTickInter();	
	removeProcess(getCurrentPid());
	nextLine();
}

qword sys_pipeCreate(qword connectingProcessName, qword rdx, qword rcx, qword r8, qword r9){
	int connectingProcessPID = getProcessFromName((char *) connectingProcessName);
	int callingProcessPID = getCurrentPid();
	ProcessSlot * callingProcess = getProcessFromPid(callingProcessPID);

	callingProcess->process.pipes[callingProcess->process.pipeIndex] = createPipe(callingProcessPID,connectingProcessPID,(char *) connectingProcessName);
	callingProcess->process.pipeIndex++;
	return (qword) callingProcess->process.pipes[callingProcess->process.pipeIndex - 1]->pipePID;
}


qword sys_pipeWrite(qword pipePID, qword message, qword messageLength, qword r8, qword r9){
	int charsRead;
	int writingProcessPID = getCurrentPid();
	char * messagePointer = (char *) message;
	ProcessSlot * writingProcess = getProcessFromPid(writingProcessPID);
	p_pipe pipePointer = searchPipeByPID(writingProcess,pipePID);
	charsRead = write(pipePointer,messagePointer,messageLength,writingProcessPID);
	return (qword) charsRead;
}

qword sys_pipeRead(qword pipePID, qword messageDestination, qword charsToRead, qword r8, qword r9){
	int readingProcessPID = getCurrentPid();
	char * messageDestinationPointer = (char *) messageDestination;
	ProcessSlot * readingProcess = getProcessFromPid(readingProcessPID);
	p_pipe pipePointer = searchPipeByPID(readingProcess,pipePID);
	int charsRead = read(pipePointer,messageDestinationPointer,charsToRead,readingProcessPID);
	return (qword) charsRead;
}

void sys_pipeClose(qword pipePID, qword operation, qword rcx, qword r8, qword r9){
	int callingProcessPID = getCurrentPid();
	ProcessSlot * callingProcess = getProcessFromPid(callingProcessPID);
	p_pipe pipePointer = searchPipeByPID(callingProcess,pipePID);
	close(pipePointer,operation,callingProcessPID);
}

void sys_pipeOpen(qword pipePID, qword operation, qword rcx, qword r8, qword r9){
	int callingProcessPID = getCurrentPid();
	ProcessSlot * callingProcess = getProcessFromPid(callingProcessPID);
	p_pipe pipePointer = searchPipeByPID(callingProcess,pipePID);
	open(pipePointer,operation,callingProcessPID);
}

qword sys_getPID(qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	return (qword) getCurrentPid();
}

qword sys_getMutex(qword mutexName, qword rdx, qword rcx, qword r8, qword r9){
	int index;

	index = getMutexByName((char *) mutexName);
	if(index == INVALID_INDEX)
		index = getFreeMutex((char *) mutexName);

	return index;
}

void sys_wait(qword index, qword rdx, qword rcx, qword r8, qword r9){
	wait(index);
}

void sys_signal(qword index, qword rdx, qword rcx, qword r8, qword r9){
	signal(index);
}

void sys_freeMutex(qword index, qword rdx, qword rcx, qword r8, qword r9){
	freeMutex(index);
}

void sys_kernelHeader(qword index, qword rdx, qword rcx, qword r8, qword r9){
	printHeaderInfo();
}

void sys_killPID(qword pid, qword rdx, qword rcx, qword r8, qword r9){
	disableTickInter();	
	removeProcessFromTerminal(pid);
	nextLine();
}

void load_systemcalls(){
	sysCalls[1] = (sys) &sys_write;
	sysCalls[2] = (sys) &sys_clear;
	sysCalls[3] = (sys) &sys_read;
	sysCalls[4] = (sys) &sys_fontColor;
	sysCalls[5] = (sys) &sys_nextLine;
	sysCalls[6] = (sys) &sys_getPID;
	sysCalls[7] = (sys) &sys_delete;
	sysCalls[8] = (sys) &sys_pixel;
	sysCalls[9] = (sys) &sys_time;
	sysCalls[10] = (sys) &sys_malloc;
	sysCalls[11] = (sys) &sys_printHex;
	sysCalls[12] = (sys) &sys_createProcess;
	sysCalls[13] = (sys) &sys_ls;
	sysCalls[14] = (sys) &sys_pkill;
	sysCalls[15] = (sys) &sys_pipeCreate;
	sysCalls[16] = (sys) &sys_pipeWrite;
	sysCalls[17] = (sys) &sys_pipeRead;
	sysCalls[18] = (sys) &sys_pipeClose;
	sysCalls[19] = (sys) &sys_pipeOpen;
	sysCalls[20] = (sys) &sys_getMutex;
	sysCalls[21] = (sys) &sys_wait;
	sysCalls[22] = (sys) &sys_signal;
	sysCalls[23] = (sys) &sys_freeMutex;
	sysCalls[24] = (sys) &sys_kernelHeader;
	sysCalls[25] = (sys) &sys_killPID;

	setup_IDT_entry(0x80, (qword) &_irq80Handler); 
}

qword syscallHandler(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	if(rdi < 0 || rdi >= 26)
		return 0;
	
	return sysCalls[rdi](rsi,rdx,rcx,r8,r9);
}
