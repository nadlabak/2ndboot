/* Now we are running with MMU enabled
 * r0 should be virtual pointer to bundle.
 * r1 should be physical pointer to image target
 * r2 should be chunk size
 * let's assume that resulted image won't overwrite crucial TTBLs and stack
 * since we are rewriting kernel text it shouldn't happen,
 * but we can't go back to running linux.
 */

	cpsid   ifa                    @ we don't need any interrupts here
	sub	r14, r15, #0xc
@	b _main
	stmfd	sp!, {r0-r2}
	mov     r3, #2                 @ skip size and checksum
	add	r1, r1, #0x30000000    @ phys to virt
merge_loop:
	ldr     r4, [r0, r3, lsl #2]   @ VA of chunk
	cmp     r4, #0
	beq	unpacked               @ done
	ldr	r2, [sp, #8]           @ chunk size
copy_loop:
	ldmia	r4!, {r5-r12}          @ chunk size should be 0x20-aligned
	stmia	r1!, {r5-r12}
	subs	r2, r2, #0x20
	bne	copy_loop
	add	r3, r3, #1
	b	merge_loop
unpacked:                              @ ok, we did it. now moto's kernel is corrupted badly, so no way back.
	ldmfd	sp!, {r4-r6}
	mov	r0, r5                 @ image target
	ldr	r1, [r4]               @ image size
	ldr	r2, [r4, #4]           @ image checksum

	mov 	r4, #0
	mcr     p15, 0, r4, c7, c10, 4 @ memory barrier
	mcr     p15, 0, r4, c13, c0, 0 @ set zero PID

	mcr     p15, 0, r4, c7, c5,  0 @ invalidate I cache
	mcr     p15, 0, r4, c7, c14, 0 @ clean & invalidate D cache

        mrc     p15, 0, r7, c1, c0, 0
        bic     r7, r7, #0x0005        @ - dcache & mmu
        bic     r7, r7, #0x1800        @ - icache & branch prediction
        mcr     p15, 0, r7, c1, c0, 0

	mcr     p15, 0, r4, c7, c5, 4  @ flush prefetch buffer
	nop
	nop
	nop

	mcr     p15, 0, r4, c7, c5,  0 @ invalidate I cache
	mcr     p15, 0, r4, c7, c14, 0 @ clean & invalidate D cache
	mcr     p15, 0, r4, c8, c5, 0  @ invalidate ITLB
	mcr     p15, 0, r4, c8, c6, 0  @ invalidate DTLB
	mov	r3, r14
/* Now MMU is turned off
 * r0 - physical address of image target
 * r1 - image size
 * r2 - image checksum
p * r3 - boot image base
 */
_start: b _main
/* This funcs shouldn't touch r0, r1, r2, r3 */
/* r3 - image base */
fixup_got:
	ldr r4, got_start
	ldr r5, got_end
	add r4, r4, r3
	add r5, r5, r3
fixup_loop:
	cmp r4, r5
	beq fixup_end
	ldr r6, [r4]
	add r6, r6, r3
	str r6, [r4], #4
	b fixup_loop
fixup_end:
	bx lr

clean_bss:
	ldr r4, bss_start
	ldr r5, bss_end
	add r4, r4, r3
	add r5, r5, r3
	mov r6, #0
clean_loop:
	cmp r4, r5
	beq clean_end
	str r6, [r4], #4
	b clean_loop
clean_end:
	bx lr

got_start: .word __got_start__
got_end:   .word __got_end__
bss_start: .word __bss_start__
bss_end:   .word __bss_end__
boot_size: .word __boot_size__
sp_svc:	.word 0x90400200
sp_sys: .word 0x903FFE00
sp_und: .word 0x903FFC00
sp_abt: .word 0x903FFA00
sp_fiq: .word 0x903FD800
sp_irq: .word 0x903FE800
_main:
	cps #0x13 @ supervisor
	ldr sp, sp_svc
	cps #0x1f @ system
	ldr sp, sp_sys
	cps #0x1b @ undef
	ldr sp, sp_und
	cps #0x17 @ abort
	ldr sp, sp_abt
	cps #0x11  @ fiq
	ldr sp, sp_fiq
	cps #0x12  @ irq
	ldr sp, sp_irq

	cps #0x13
	bl fixup_got
	bl clean_bss
	bl main
forever:.word 0xeafffffe
.global _start
.global got_start
.global got_end
.global bss_start
.global bss_end
.global boot_size
