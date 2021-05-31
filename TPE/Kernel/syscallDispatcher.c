#include <time.h>
#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <stddef.h>
#include <naiveConsole.h>
#include <sysCallDispatcher.h>
#include <lib.h>

typedef uint64_t (*PSysCall) (uint64_t, uint64_t, uint64_t);


static PSysCall sysCalls[255] = {&sysRead, &sysWrite, &sysGetRegs, &sysGetMem, &sysGetDateTime};

void sysCallDispatcher (uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx){
    PSysCall sysCall = sysCalls[rax];
    if (sysCall != 0)
        return sysCall(rdi, rsi, rdx);
}

int sysRead (int fd, char * buff, uint16_t amount){
    if(fd != 0) {
        return -1;
    }
    return readFromKeyboard(buff, amount);
}

int sysWrite(int fd, char * buff, uint64_t count){
    char att;
    if (fd == 1)
        att = 0x07;
    else if (fd == 2)
        att = 0x04;
    else{
        int pos = getCursor();
        printString("Error in file descriptor", pos, 0x07, 0);
        ncNewline();
        return -1;
    }
    int i;
    for (i = 0; i < count || buff[i] == '\0'; i++)
        printChar(buff[i], att, 0);
    return i;

}

/*void sysGetRegs(int fd, char * buff, uint64_t count){
    char * registers[16] = {"RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: "};
    for (int i = 0; i < 16; i++){
        write(1, registers[i], 5);
        uint64_t reg = getRegister(i);
        char buffer[20];
        char * hexa = intToHexa(reg, buffer, 8);
        write(1, hexa, 10);
        ncNewline();
    }
}
*/


static uint64_t regs[17] = {0};


uint64_t * getRegs(){
    return regs;
}


uint64_t sysGetRegs(uint64_t buffer, uint64_t rdx, uint64_t rcx){
    long long *array = (long long *)buffer;
    for (int i = 0; i < 15; i++) {
        array[i] = regs[i];
    }
    
    return 0;

}



void sysGetMem(char * buff, uint64_t address, uint64_t amount){
    for(int i = 0; i < amount; i++){
        int position = address + i;
        buff[i] = asmGetByte(position);
    }
    
}


uint64_t sysGetDateTime(uint64_t id, uint64_t rdx, uint64_t rcx){
    uint64_t data = getDateTime(id);
    return (data >> 4) * 10 + (data & 0x0F);
}





