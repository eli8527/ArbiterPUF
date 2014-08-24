//******************************************************************
/*

  Schreiben Sie Ihren CRYPTO Assembler Quelltext in diese Datei!
  
  Setzen Sie den Quelltext der CRYPTO Verschlüsselung hinter die Sprungmarke 'CRYPTO_enc'
  und den Quelltext der Entschlüsselung hinter die Sprungmarke 'CRYPTO_dec'.
  Den beiden Funktionen wird jeweils ein 16-Bit Zeiger auf den 128-Bit Klar- bzw. Geheimtext,
  ein zweiter 16-Bit Zeiger auf den 128-Bit CRYPTO Rundenschlüssel sowie ein dritter Zeiger auf
  512 freie Bytes im SRAM übergeben.
  Achten Sie darauf, daß das Ergebnis der Ver- bzw. Entschlüsselung in den Array
  geschrieben wird, auf den der erSTe Zeiger zeigt.

  WICHTIG: Der 1. Zeiger wird in dem Registerpaar r24(lo),r25(hi), der 2. Zeiger in dem
           RegiSTerpaar in dem Registerpaar r22(lo),r23(hi) und der dritte Zeiger in dem
           Paar r20(lo),r21(hi) übergeben. Siehe die AVR Libc Konventionen
           unter http://savannah.nongnu.org/download/avr-libc/doc/avr-libc-user-manual/ !
           
  Beachten Sie die unterschiedlichen Assembler Konventionen (gcc AVR Assembler <-> AVR Studio 4 Assembler)
  
  AVR Studio 4                       gcc AVR Assembler
  .asm (Datei)                       .S (Datei)
  .def (Direktive)                   .set (Direktive)
  .db                                .byte
  .org                               .balign
  hi                                 hi8
  lo                                 lo8

*/
//******************************************************************


.set xl,26
.set xh,27
.set yl,28
.set yh,29
.set zl,30
.set zh,31
.set sph,0x3e
.set spl,0x3d
.set ramend,0x45f


;--------------------------------------------------------------------
;begin encrypt
;points to Plaintext in registers (r24 (lo),r25 (hi))
;Zeiger auf den 168 Byte Rundenschlüsselspeicher im Registerpaar (r22 (lo),r23 (hi))
;Zeiger auf den 512 Byte freien SRAM im Registerpaar (r20 (lo),r21 (hi))

.global	CRYPTO_enc
.type	CRYPTO_enc,@function
CRYPTO_enc:

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
    push r30
    push r31



;Hier muß der CRYPTO Verschlüsselungsquelltext rein!



    pop r31
    pop r30
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	clr r1
        ret

;end encrypt


;--------------------------------------------------------------------
;begin Decrypt
;Zeiger auf den Plaintext im Registerpaar (r24 (lo),r25 (hi))
;Zeiger auf den 168 Byte Rundenschlüsselspeicher im Registerpaar (r22 (lo),r23 (hi))
;Zeiger auf den 512 Byte freien SRAM im Registerpaar (r20 (lo),r21 (hi))

.global	CRYPTO_dec
.type	CRYPTO_dec,@function
CRYPTO_dec:

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
    push r30
    push r31



;Hier muß der CRYPTO Entschlüsselungsquelltext rein!



    pop r31
    pop r30
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	clr r1
    ret

;end decrypt

;--------------------------------------------------------------------

;--------------------------------------------------------------------
;begin Decrypt
;Zeiger auf den Schlüssel im Registerpaar (r24 (lo),r25 (hi))
;Zeiger auf den 168 Byte Rundenschlüsselspeicher im Registerpaar (r22 (lo),r23 (hi))
;Zeiger auf den 512 Byte freien SRAM im Registerpaar (r20 (lo),r21 (hi))

.global	schedule_key
.type	schedule_key,@function
schedule_key:

	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
    push r30
    push r31



;Hier muß der CRYPTO Key Scheduling Quelltext rein!



    pop r31
    pop r30
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	clr r1
    ret

;end schedule_key

;--------------------------------------------------------------------
