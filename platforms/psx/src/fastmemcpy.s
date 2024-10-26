
;*****************************
;*** Fast Lib Replacements ***
;*****************************

	include	gtereg.h


	opt	at+
	section	.text


;******************************************************************************
; New Mem copy
;	a0 = destination address
;	a1 = source address
;       a2 = length
;
	global	MCmemcpy

MCmemcpy:
		move	t0,a0				; t0 = return address
;		addu	t0,a1,a2			; not sure about this  t0 = return address

		beq	zero,a2,@all_done		; zero length
		nop

		andi	v0,a2,$fffffffc			; check size

		beq	zero,v0,@mem_store_bytes        ; check for less than 4
		move	v1,a2				; if memcpy less than 4, need size set up

		andi	v0,a2,$3
		subu	a2,a2,v0			; make an even four
		move	v1,v0

		andi	v0,a1,$3			; check source address non aligned
		beq	zero,v0,@source_align

		andi	v0,a0,$3			; check dest address non aligned
		beq	zero,v0,@dest_align

;
;	must not be aligned
;

@mem_store_byte_words:
		lwr	v0,0(a1)
		lwl	v0,3(a1)
		addiu	a1,a1,$4
		swr	v0,0(a0)
		swl	v0,3(a0)
		addiu	a2,-$4
		bne	zero,a2,@mem_store_byte_words
		addiu	a0,a0,$4

@mem_store_bytes:
		lb	v0,0(a1)
 		addiu	a1,a1,$1
		sb	v0,0(a0)
		addiu	v1,v1,-$1
		bne	zero,v1,@mem_store_bytes
		addiu	a0,a0,$1

@all_done:	move	v0,t0
		jr	ra
		nop

@source_align:	andi	v0,a0,$3			; check dest address aligned
		beq	zero,v0,@mem_store_words
		nop
;
;	source align dest disp
;

@mem_store_half2:
		lw	v0,0(a1)
		addiu	a1,a1,$4
		swr	v0,0(a0)
		swl	v0,3(a0)
		addiu	a2,a2,-$4
		bne	zero,a2,@mem_store_half2
		addiu	a0,a0,$4

		bne	zero,v1,@mem_store_bytes
		move	v0,t0
		jr	ra
		nop


@dest_align:    andi	v0,a1,$3			; check source address aligned
		beq	zero,v0,@mem_store_words
;
;	source disp 2 dest align
;

@mem_store_half1:
		lwr	v0,0(a1)	; get first bit 
		lwl	v0,3(a1)	; get second bit
		addiu	a1,a1,$4
		addiu	a2,a2,-$4
		sw	v0,0(a0)
		bne	zero,a2,@mem_store_half1
		addiu	a0,a0,$4

		bne	zero,v1,@mem_store_bytes
		move	v0,t0
		jr	ra
		nop
;
;	source and dest align
;

@mem_store_words:
		lw	v0,0(a1)		; get first word
		addiu	a1,a1,$4
		sw	v0,0(a0)		; store it
		addiu	a2,a2,-$4
		bne	zero,a2,@mem_store_words
		addi	a0,a0,$4

		bne	zero,v1,@mem_store_bytes
		move	v0,t0
		jr	ra
		nop
