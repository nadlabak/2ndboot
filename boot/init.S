.macro barrier, reg
	mcr p15, 0, \reg, c7, c10, 4
.endm
.macro cpwait, reg
	mrc p15, 0, \reg, c2, c0, 0
	mov \reg, \reg
	sub pc, pc, #4
.endm
.macro flush_tlb, reg
	mov \reg, #0
	mcr p15, 0, \reg, c8, c5, 0
	mcr p15, 0, \reg, c8, c6, 0
	cpwait \reg
.endm

/* Now we are running with MMU enabled _and_ phys != virt!
 * r0 - pointer to set of scattered buffers.
 * r1 - count of buffers
 * r2 - pointer to new trnaslation table
 */
	cpsid   ifa
	sub	r14, r15, #0xc         @ virt addr of entry point
	sub	r14, r14, #0x30000000

	mov	r3, #0

	mcr	p15, 0, r3, c2, c0, 2  @ load new translation table
	mcr	p15, 0, r2, c2, c0, 0
	cpwait	r3
	flush_tlb r2
	
	sub	r2, pc, #0             @ now we are switching to physical adress for smooth mmu turnoff.
	sub	pc, r2, #0x30000000

	b	__with_mmu             @ at that place, pc is physical.
tags_table:
@	kernel image
	.word 0x1                      @ tag
	.word 0x90008000	       @ target
	.word 0x0                      @ size
	.word 0x0                      @ cheksum
@	initramfs
	.word 0x2
	.word 0x90800000
	.word 0x0
	.word 0x0
@	motflattree
	.word 0x3
	.word 0x91000000
	.word 0x0
	.word 0x0
@	cmdline
	.word 0x4
	.word 0x91100000
	.word 0x0
	.word 0x0
@	usbfw
	.word 0x5
	.word 0x91110000
	.word 0x0
	.word 0x0
@	ending zero
	.word 0x0
	.word 0x0
	.word 0x0
	.word 0x0
virt_sp:
	.word 0xc2000000
__with_mmu:
	ldr	sp, virt_sp
	str	r14, [sp, #-4]!

	mov	r3, r0
	mov	r6, r1
__buffers_loop:
	subs	r6, r6, #1
	bmi	__buffers_end
	ldr	r4, [r3], #4
	bic	r5, r4, #1
	eor	r4, r5, r4
	ldr	r0, [r5, #0x8]
	bl	find_tag
	cmp	r0, #0
	movne	r1, r5
	movne	r2, r4
	blne	proccess_buffer
	b	__buffers_loop
__buffers_end:
	b	__turn_off_mmu

find_tag:
	stmfd	sp!, {r1, r2, lr}
	adr	r1, tags_table
__tags_loop:
	ldr	r2, [r1]
	cmp	r2, #0
	moveq	r1, #0
	cmpne	r2, r0
	moveq	r0, r1
	beq	__tags_end
	add	r1, r1, #0x10
	b	__tags_loop
__tags_end:
	ldmfd	sp!, {r1, r2, pc}

proccess_buffer:
	stmfd	sp!, {r1-r3,lr}
	cmp	r2, #0                 @ plain ?

	ldr	r2, [r1, #0x0]         @ size
	str	r2, [r0, #0x8]
	ldr	r2, [r1, #0x4]         @ checksum
	str	r2, [r0, #0xc]

	ldr	r0, [r0, #0x4]         @ target

	beq	__plain
	ldr	r2, [r1, #0x10]        @ chunks count
	ldr	r3, [r1, #0xc]         @ chunks size
	add	r1, r1, #0x14          @ chunks table
	bl	proccess_scattered
	ldmfd	sp!, {r1-r3,pc}
__plain:
	ldr	r2, [r1, #0x0]         @ size
	ldr	r1, [r1, #0xc]         @ source
	bl	proccess_plain
	ldmfd	sp!, {r1-r3,pc}

proccess_scattered:
	stmfd	sp!, {r4-r12,lr}
__chunks_loop:
	subs	r2, r2, #1
	bmi	__chunks_done
	ldr	r4, [r1], #4           @ new chunk
	mov	r5, r3                 @ chunk size
__copy_loop:
	ldmia	r4!, {r6-r12,r14}
	stmia	r0!, {r6-r12,r14}
	subs 	r5, r5, #0x20
	bne	__copy_loop
__copy_done:
	b	__chunks_loop
__chunks_done:
	ldmfd	sp!, {r4-r12,pc}

proccess_plain:
	stmfd	sp!, {r3-r7,lr}
__qwords_loop:
	cmp	r2, #0x10
	blo	__qwords_done
	ldmia	r1!, {r4-r7}
	stmia	r0!, {r4-r7}
	sub	r2, r2, #0x10
	b	__qwords_loop
__qwords_done:
	tst	r2, #0x8
	ldmneia	r1!, {r4,r5}
	stmneia	r0!, {r4,r5}

	tst	r2, #0x4
	ldrne	r4, [r1], #4
	strne	r4, [r0], #4
	
	movs	r2, r2, lsl #31
	ldrcsh	r4, [r1], #2
	strcsh	r4, [r0], #2
	ldrmib	r4, [r1], #1
	strmib	r4, [r0], #1
	
	ldmfd	sp!, {r3-r7,pc}
	
__turn_off_mmu:
	ldr	r0, [sp], #4

	mov	r4, #0

	mcr     p15, 0, r4, c13, c0, 0 @ set zero PID
	mcr     p15, 0, r4, c7, c10, 0 @ clean D cache
	mov	r5, #0x30000000        @ L2CC base

	ldr	r6, [r5, #0x100]       @ disable L2 cache
	bic	r6, r6, #0x1
	str	r6, [r5, #0x100]

	str	r4, [r5, #0x730]       @ L2 cache sync
	mov	r6, #0xff
	str	r6, [r5, #0x7fc]       @ request L2 cache clean and invalidation
l2_cln:	ldr	r6, [r5, #0x7fc]       @ and wait for it.
	cmp	r6, #0
	bne	l2_cln

	mcr     p15, 0, r4, c7, c5,  0 @ invalidate I cache

	mrc     p15, 0, r5, c1, c0, 0
	bic     r5, r5, #0x0005        @ -dcache & -mmu
	mcr     p15, 0, r5, c1, c0, 0

	mcr     p15, 0, r4, c7, c5, 4  @ flush prefetch buffer
	cpwait 	r4

	mcr     p15, 0, r4, c7, c5, 0  @ invalidate I cache
	mcr     p15, 0, r4, c7, c6, 0  @ invalidate D cache
	mcr     p15, 0, r4, c8, c5, 0  @ invalidate ITLB
	mcr     p15, 0, r4, c8, c6, 0  @ invalidate DTLB

/* Now MMU is turned off
 * r0 - physical address of boot_base
 */
_start: b _main
/* r0 - image base */
fixup_got:
	ldr r4, got_start
	ldr r5, got_end
	add r4, r4, r0
	add r5, r5, r0
fixup_loop:
	cmp r4, r5
	beq fixup_end
	ldr r6, [r4]
	add r6, r6, r0
	str r6, [r4], #4
	b fixup_loop
fixup_end:
	bx lr

clean_bss:
	ldr r4, bss_start
	ldr r5, bss_end
	add r4, r4, r0
	add r5, r5, r0
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
sp_svc:	.word 0x92000000
sp_sys: .word 0x91f80000
sp_und: .word 0x91f7f000
sp_abt: .word 0x91f7e000
sp_fiq: .word 0x91f7d000
sp_irq: .word 0x91f7c000
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
	cps #0x13  @ supervisor

	bl fixup_got
	bl clean_bss
	adr r1, tags_table
	bl main
forever:.word 0xeafffffe
.global _start
.global got_start
.global got_end
.global bss_start
.global bss_end
.global boot_size
.global tags_table
