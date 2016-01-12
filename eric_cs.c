#include "eric_cs.h"

#include "./inc/basetype.h"
#include "./inc/ProjectDiff.h"


typedef void (*EricFunPtr_0)(void);
typedef uint32  EricContext;

typedef struct  {
    uint32 S_PSW;
    uint32 S_IPSW;
    uint32 S_IPC;

    //p1
    uint32 S_SP;

    //R0~R25
    uint32 S_R[26];

    // r28~ r30
    uint32 S_FP;
    uint32 S_GP;
    uint32 S_LP;
}EricReg;

typedef struct  {
    EricFunPtr_0 fun;
    EricContext context;

}EricTask;

EricTask t1,t2;

uint32 getRegister()
{
    uint32 tmp_reg;

    __asm__ __volatile__(
            "move  %0,$sp" :"=&r"(tmp_reg)
            );
   return tmp_reg;
}

void PrintReg(EricReg* reg)
{
    DEBUG_MSG("\n RegAddr=%08X", (uint32)(reg) );
    DEBUG_MSG("\n S_PSW=%08X", reg->S_PSW);
    DEBUG_MSG("\n S_SP=%08X", reg->S_SP);
    DEBUG_MSG("\n S_FP=%08X", reg->S_FP);
    DEBUG_MSG("\n S_GP=%08X", reg->S_GP);
    DEBUG_MSG("\n S_LP=%08X", reg->S_LP);
}


#define ERIC_SP_RESERVE_FOR_PARAM (24)

uint32 eric_init_context(uint32 entry, EricContext sp_base )
{

    uint32 sp_addr = sp_base - sizeof(EricReg) - (ERIC_SP_RESERVE_FOR_PARAM*2);
    uint32 regAddr = sp_addr + ERIC_SP_RESERVE_FOR_PARAM;

    EricReg *regs =  (EricReg *)(regAddr);;
    uint32  tmp_reg;

    __asm__ __volatile__(
            "mfsr  %0,$PSW\n\t" :"=&r"(tmp_reg));
    regs->S_PSW = tmp_reg;
    __asm__ __volatile__(
            "mfsr  %0,$IPSW\n\t" :"=&r"(tmp_reg));
    regs->S_IPSW = tmp_reg;
    __asm__ __volatile__(
            "mfsr  %0,$IPC\n\t" :"=&r"(tmp_reg));
    regs->S_IPC = tmp_reg;

    __asm__ __volatile__(
            "move  %0,$gp\n\t" :"=&r"(tmp_reg));
    regs->S_GP = tmp_reg;

    regs->S_SP = sp_base;
    regs->S_FP = sp_base;
    regs->S_LP = entry; // ex: fun_addr
    regs->S_R[0] = 0;   // function register

    return sp_addr;
}

void task_2()
{
    while( 1 ) {
        DEBUG_MSG("\nT2-1");
        eric_ContextSwitch(&t1.context, &t2.context);
        DEBUG_MSG("\nT2-2");
        eric_ContextSwitch(&t2.context, &t2.context);
        DEBUG_MSG("\nT2-3");
    }
}

void task_1()
{
    while( 1 ) {
        DEBUG_MSG("\nT1-1");
        eric_ContextSwitch(&t2.context, &t1.context);
        DEBUG_MSG("\nT1-2");
        eric_ContextSwitch(&t2.context, &t1.context);
        DEBUG_MSG("\nT1-3");
    }
}

void eric_comtextSwitchTest()
{
#define T1_STACK_BASE (0x80001000)
#define T2_STACK_BASE (0x80002000)
#define MainContext (0x80003000)

    // init all stack
    uint32 i = 0;
    for(i = 0x80000000; i < 0x80003000; i += 4) {
        *((uint32 *)( i)) = 0;
    }

    t1.context = eric_init_context( (uint32)&task_1, T1_STACK_BASE);
    t2.context = eric_init_context( (uint32)&task_2, T2_STACK_BASE);

    eric_ContextSwitch(&t1.context,  (EricContext*)MainContext);

    DEBUG_MSG("\nFinish");
}
