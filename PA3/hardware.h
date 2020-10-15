
void TRAP_KERNEL(UserContext exception);
void TRAP_CLOCK(UserContext exception);
void TRAP_ILLEGAL(UserContext exception);
void TRAP_MEMORY(UserContext exception);
void TRAP_MATH(UserContext exception);
void TRAP_TTY_TRANSMIT(UserContext exception);
void TRAP_TTY_RECIEVE(UserContext exception);
void TRAP_DISK(UserContext exception);


//Macros for page addresses and page number
int *PAGESIZE;
int *PAGEOFFSET;
int *PAGEMASK;
int *PAGESHIFT;
int *UP_TO_PAGE;
int *DOWN_TO_PAGE;



struct UserContext{
    int vector;
    int code;
    void *addr;
    void *pc;
    void *sp;
    void *ebp;
    long regs[8];
}UC;