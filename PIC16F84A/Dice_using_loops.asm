;
; -----   DADO ELECTRONICO   -----
;
; Ver: 1.0
; Fecha: 2022-02-22
; Autor: Andrés Garcia Alves
; Microcontrolador: PIC16F84A @4Mhz, 4 ciclos reloj -> 1 ciclo máquina
;
; LEDs:
;   *     *
;   *  *  *
;   *     *
; RB0 -> entrada de interrupciones
; RB1 -> salida a led arriba-izquierda
; RB2 -> salida a led arriba-derecha
; RB3 -> salida a led medio-izquierda
; RB4 -> salida a led central
; RB5 -> salida a led medio-derecha
; RB6 -> salida a led abajo-izquierda
; RB7 -> salida a led abajo-derecha
;

	__CONFIG _CP_OFF & _WDT_OFF & _PWRTE_ON & _XT_OSC

	PROCESSOR 16F84A
	#INCLUDE <P16F84A.INC>

	#DEFINE PIN_0 d'0'


; variables & start
	CBLOCK 0x10
	random
	idx_i
	idx_j
	idx_k
	ENDC

	ORG 	0x00
	goto	setup


; Config inicial
setup
	bsf		STATUS, RP0			; acceder al banco 1 de memoria

	movlw	b'00000001'
	movwf	TRISB				; config RB0 como entrada, RB1-RB7 como salidas

	bcf		STATUS, RP0			; volver al banco 0 de memoria

	movlw	b'00000000'
	movwf	INTCON				; des-habilitar flag de interrupciones globales


; Bucle principal
main
	btfss	PORTB, PIN_0		; ¿estado alto en pin 0?
	goto	main				; no -> esperar
	call	delay_30ms			; rutina anti-rebote, 30 ms

	btfss	PORTB, PIN_0		; ¿sigue en estado alto el pin 0?
	goto	main				; no -> falso positivo, volver a esperar
	call	set_countdown		; si -> random = 6

randomizer_loop
	decf	random, F			; random - 1
	btfsc	STATUS, Z			; ¿llegó a 0?
	call	set_countdown		; si -> random = 6
	nop							; no // instruccion innesesaria, usada por legibilidad

	btfsc	PORTB, PIN_0		; ¿sigue en estado alto el pin 0?
	goto	randomizer_loop		; si -> seguir loopeando

show_dice
	movf	random, W			; mover el número aleatorio a W	
	call	led_table			; devuelve en W su combinacion de pines
	movwf	PORTB				; encender los leds
	call	delay_10s			; congelar los leds, 10 segs

exit_dice
	movlw	d'0'				; mover el número cero a W
	call	led_table			; devuelve en W su combinacion de pines
	movwf	PORTB				; encender los leds

	goto	main

set_countdown
	movlw	d'6'
	movwf	random
	return


; Tabla en ROM con las combinaciones de encendido de los LEDs
;   posiciones en el literal: RB7-RB6-RB5-RB4-RB3-RB2-RB1-INT
led_table
	addwf	PCL, F
	retlw	b'00000001'			; número 0 (leds apagados)
	retlw	b'00010001'			; número 1
	retlw	b'10000011'			; número 2
	retlw	b'10010011'			; número 3
	retlw	b'11000111'			; número 4
	retlw	b'11010111'			; número 5
	retlw	b'11101111'			; número 6


; Subrutina de retardo, 30 milisegundos (aprox)
;   bucle interior: (3 * 100) + 1 = 301
;   bucle exterior: ((301 + 4) * 100) + 1 = 30501 ciclos de máquina
delay_30ms
	movlw	d'100'				; X = 100
	movwf	idx_i
ds30_outer_loop
	movwf	idx_j				; ciclos 1*X		(bucle externo)
ds30_inner_loop
	decfsz	idx_j, F			; ciclos 1*X + 1	(bucle anidado)
	goto	ds30_inner_loop		; ciclos 2*X		(bucle anidado)
	decfsz	idx_i, F			; ciclos 1*X + 1	(bucle externo)
	goto	ds30_outer_loop		; ciclos 2*X		(bucle externo)
	return


; Subrutina de retardo, 10 segundos (aprox)
;   bucle interior: (4 * 250) + 1 = 1001 ciclos de máquina
;   bucle exterior: ((1001 + 5) * 249) + 1 = 250.495 ciclos de máquina
;   bucle principal: ((250495 + 5) * 40 + 1 = 10.020.001 ciclos de máquina
delay_10s
	movlw	d'40'				; X = 40
	movwf	idx_i
ds10_main_loop
	movlw	d'249'				; Y = 249
	movwf	idx_j
ds10_outer_loop
	movlw	d'250'				; Z = 250
	movwf	idx_k				; ciclos 1xY		(bucle externo)
ds10_inner_loop
	nop							; ciclos 1xZ		(bucle anidado)
	decfsz	idx_k, F			; ciclos 1xZ + 1	(bucle anidado)
	goto	ds10_inner_loop		; ciclos 2xZ		(bucle anidado)
	decfsz	idx_j, F			; ciclos 1xY + 1	(bucle externo)
	goto	ds10_outer_loop		; ciclos 2xY		(bucle externo)
	decfsz	idx_i, F			; ciclos 1xX + 1	(bucle principal)
	goto	ds10_main_loop		; ciclos 2xX		(bucle principal)
	return

	END
