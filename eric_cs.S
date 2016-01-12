#include "../inc/cpu.h"

    .global eric_ContextSwitch
eric_ContextSwitch:

	addi $sp, $sp, -8

	pushm $r28, $r30
	pushm $r0, $r25// push all reg

	// sp
	push $p1

	// save system register
	mfsr $r18, $IPC
	mfsr $r17, $IPSW
	mfsr $r16, $PSW
	pushm $r16, $r18


	addi    $sp, $sp, -24

	sw      $sp, [$r1]

	//======= load context =======

    setgie.d
	lw      $sp, [$r0]
    isb

	addi    $sp, $sp, 24

    popm $r16, $r18
    mtsr $r16, $PSW
    mtsr $r17, $IPSW
    mtsr $r18, $IPC

    pop $p1

    popm $r0, $r25
    popm $r28, $r30

	addi    $sp,  $sp, 8

    dsb
    ret $lp


