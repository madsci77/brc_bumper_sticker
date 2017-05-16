; label HISPEED => if defined, 115200Bd is used instead of 9600Bd

	include "mc9s08aw60.inc"

SCIBDH	equ SCI1BDH
SCIBDL	equ SCI1BDL
SCC1  	equ SCI1C1
SCC2		equ SCI1C2
SCC3		equ SCI1C3
SCS1		equ SCI1S1
SCS2		equ SCI1S2
SCDR		equ SCI1D
TC			equ SCI1S1_TC
RDRF		equ SCI1S1_RDRF

TXDDDR          EQU     PTEDD
TXDPORT         EQU     PTED
TXDBIT          EQU     0
RXDPORT         EQU     PTED
RXDBIT          EQU     1

LOCK    equ ICGS1_LOCK

; command codes
mBlank:     equ   $05         ;Blank Check command
mByteProg:  equ   $20         ;Byte Program command
mBurstProg: equ   $25         ;Burst Program command
mPageErase: equ   $40         ;Page Erase command
mMassErase: equ   $41         ;Mass Erase command


RCS_ENA    	EQU     1     	; READ COMMAND SUPPORTED?

RCS         EQU     $80   	; READ COMMAND SUPPORTED

VER_NUM     EQU     2     	; FC protocol version number

IDENTS      MACRO
				DC.B	'GT1'
            DC.B    0
            ENDM

ERBLK_LEN	EQU		512
WRBLK_LEN	EQU		64      

FLS_BEG		EQU		$8000		; FLASH #0 block start
FLS_END		EQU		$FE00		; FLASH #0 block end

REL_VECT		EQU 		$FDCC		; newly relocated int. vectors


INT_VECT		EQU		$FFCC		; start of table of original vectors


				XDEF		bootloader_main
;*******************************************************************************************
  
WR_DATA		EQU	    'W'
RD_DATA		EQU	    'R'
ENDPRG		EQU	    'Q'
ERASE	    	EQU	    'E'
ACK		 	EQU	    $FC
IDENT			EQU	    'I'
TURBO			EQU		 'T'

T100MS		EQU	    255

ILOP        MACRO
            DC.B    $8d             ; this is illegal operation code
            ENDM
;*******************************************************************************************
MY_ZEROPAGE:	SECTION  SHORT

ADRS:			DS.W	1
ADRR: 		DS.W	1
LEN:  		DS.B	1
STAT: 		DS.B	1
STACK:		DS.W	1

				XDEF.B	ADRR

DEFAULT_RAM:    SECTION

DATA:	DC.B	128

;*******************************************************************************************
BOOTSTRAP_ROM:	SECTION
     
ID_STRING:
			DC.W	FLS_BEG         	; START ADDRESS OF FLASH	
			DC.W  FLS_END		    	; END ADDRESS OF FLASH	
			DC.W	REL_VECT				; POINTER TO APPLICATION VECTOR TABLE
			DC.W	INT_VECT				; POINTER TO BEGINING OF FLASH INT. VECTORS
			DC.W	ERBLK_LEN			; ERASE BLCK LENGTH OF FLASH ALG.
			DC.W	WRBLK_LEN			; WRITE BLCK LENGTH OF FLASH ALG.

		IDENTS
ID_STRING_END:

;*******************************************************************************************
			XDEF    NVPROT			; externed
        
NVPROT_ROM:		SECTION
NVPROT	DC.B	$FC     			; fpopen, flash protected (from 0xFE00) 

			XDEF    NVOPT   		; externalized

NVOPT_ROM:		SECTION
NVOPT 	DC.B	%10000010		; backdoor enable, redirection enable, (un)secured flash [last 10]

BOOTSTRAP_ROM:	SECTION
;*******************************************************************************************
bootloader_main:
			BRA	PVEC0
			LDA     #4
			STA	  PTDPE
			MOV	  #%11111011, PTDDD
;			BRN		slfprg
			BRCLR	  PTDD, 2, PVEC0
			LDA     SRS                     ; fetch RESET status reg.
			TSTA                            ; check if zero (this happens if RESET pulse is too short)
			BEQ     PVEC0                   ; ignore
			AND     #%11100000              ; , pin, and COP resets
			BNE     slfprg                  ; any of these sources, go to self programming
PVEC0:  		
			LDHX	  #(REL_VECT|$00FF)&$FFFE	; there should be relocated reset vector of the new app.
			LDA		,X
			PSHA
			AND	  	1,X
			LDX	  	1,X
			PULH
			JMP     ,X					  	; jump to relocated application!

slfprg:        
			LDA	  	#%00000010
			STA	  	SOPT				      	; COP disable, BDM enable (for now)
			MOV	  	#$38,ICGC1
			MOV		#$70,ICGC2
  
LOOP:		BRCLR   LOCK,ICGS1,LOOP         ; wait until ICG stable       
        
    		LDA		FSTAT
    		ORA		#mFSTAT_FACCERR				                
    		STA	  	FSTAT					; clear any FACCERR flag

			LDA	  	#111
			STA		FCDIV

			BSET		TXDBIT,TXDPORT
			BSET		TXDBIT,TXDDDR           ; TxD high when SCI disabled        

			MOV		#$2C,SCC2 			 		; transmit & receive enable
			CLR	  	SCIBDH
    		MOV		#246, SCIBDL
      	CLR		SCC3

			LDHX		#$0800						; Make room on stack
			TXS

      	LDA	   SCS1
      	MOV	   #ACK,SCDR

      	LDX	   #T100MS
L2:	   CLRA
L1:	   BRSET	  RDRF,SCS1,CAUGHT
			BRN		L1
			BRN		L1
			BRN		L1
			BRN		L1
      	DBNZA 	L1
      	DBNZX   L2
ILOP:
;       timeout
        ILOP          ; generate RESET by doing illegal operation
;*******************************************************************************************
CAUGHT:			; CAUGHT IN SELF-PROGRAMMING?
			BSR     READ
			CLR     SCC2			      ; disable SCI
			MOV     #%00001100,SCC2 ; transmit & receive enable

;*******************************************************************************************
; successful return from all write routines
SUCC:
			LDA     #ACK
			BSR     WRITE

;fall thru to background
;*******************************************************************************************
; BEGIN OF BACKGROUND COMMAND LOOP
BCKGND:
			BSR     READ
			   
			CBEQA   #ERASE, ERASE_COM       ; Erase command
			CBEQA   #WR_DATA, WR_DATA_COM   ; Write (program) command
			CBEQA   #IDENT, IDENT_COM       ; Ident command
;			CBEQA   #RD_DATA, RD_DATA_COM   ; Read command
;			CBEQA	  #TURBO, TURBO_MODE		  ; Turbo command

        ; if no valid command found (including Quit)
        ; generate reset too!
        ILOP          ; generate RESET by doing illegal operation
        
;*******************************************************************************************
IDENT_COM:                      ; TRANSFER OF IDENTIFICATION STRING
        LDA     #(VER_NUM | RCS)  ; version number & "Read command supported" flag
    		BSR	  	WRITE
        MOV     #ID_STRING_END-ID_STRING, LEN
    		LDHX  	#ID_STRING
        BSR     WRITE_LOOP
                 
        BRA     BCKGND          ; finish without ACK
;*******************************************************************************************
WRITE_LOOP:             ; start address in HX, length in LEN
      	LDA	    ,X
        BSR	    WRITE
      	AIX	    #1
        DBNZ    LEN, WRITE_LOOP
        RTS
;*******************************************************************************************
      IF RCS_ENA = 1
RD_DATA_COM:

      	BSR     READ
      	STA	    ADRS
      	BSR     READ
      	STA	    ADRS+1
      	BSR     READ
      	STA	    LEN
      	LDHX	  ADRS
        
        BSR     WRITE_LOOP

        BRA     BCKGND          ; finish without ACK
      ENDIF

;*******************************************************************************************
WRITE:	
        BRCLR	  TC,SCS1,WRITE
      	STA	    SCDR
      	RTS
READ:
        BRCLR 	RDRF,SCS1,READ
      	LDA	    SCDR
      	RTS

;*******************************************************************************************
ERASE_COM:
      	BSR		READ
      	STA		ADRS
      	BSR		READ
      	STA		ADRS+1

    		lda   	#(mFSTAT_FPVIOL+mFSTAT_FACCERR) ;mask
    		sta   	FSTAT 				;abort any command and clear errors

    		mov	  	#EraseSubSize, STAT	;length of flash erase routine to STAT
    		tsx
    		sthx  	STACK
    		ldhx  	#EraseSubEnd-1 		;point at last byte to move to stack

    		bra   	DoOnStack 			;execute prog code from stack RAM
    		
;****
TURBO_MODE:
;			CLR	  	SCIBDH
;    		MOV		#41, SCIBDL
;			BRA		BCKGND
    		
;*******************************************************************************************
WR_DATA_COM:
			BSR		READ
			STA		ADRS
			BSR		READ
			STA		ADRS+1
			BSR		READ
			STA		STAT
			STA		LEN
			LDHX		#DATA
			STHX		ADRR
WR_DATA_L1:
			BSR     READ
			STA     ,X
			AIX     #1
			DBNZ    STAT,WR_DATA_L1

			LDA   	#(mFSTAT_FPVIOL+mFSTAT_FACCERR) ;mask
			STA   	FSTAT 				;abort any command and clear errors

			MOV	  	#ProgSubSize, STAT	;length of flash prog routine to STAT
			TSX
			STHX	  STACK
			LDHX 	  #ProgSubEnd-1		;point at last byte to move to stack

;		bra 	DoOnStack 			;execute prog code from stack RAM
		; fallthru to this routine
;*******************************************************************************************
DoOnStack: 
    		lda 	  ,x 				;read from flash
    		psha 					;move onto stack
    		aix 	  #-1 			;next byte to move
    		dbnz	  STAT, DoOnStack
    		tsx 					;point to sub on stack
    		jmp 	  ,x 				;execute the sub on the stack (will return on it's own)		
;*******************************************************************************************
EraseSub: 	
    		ldhx  	ADRS		 	;get flash address
    		sta   	0,x 			;write to flash; latch addr and data
    		lda   	#mPageErase 	;get flash command
    		sta   	FCMD 			;write the flash command
    		lda   	#mFSTAT_FCBEF 		;mask to initiate command
    		sta   	FSTAT 			;[pwpp] register command
    		nop 					;[p] want min 4~ from w cycle to r
ChkDoneErase: 
    		lda   	FSTAT 			;[prpp] so FCCF is valid
    		lsla 					;FCCF now in MSB
    		bpl   	ChkDoneErase 	;loop if FCCF = 0

    		ldhx	  STACK
    		txs
        jmp     SUCC		    ;refer status back to PC
EraseSubEnd: 
EraseSubSize: equ (*-EraseSub)
;*******************************************************************************************
ProgSub: 	
    		lda   	FSTAT 			;check FCBEF
    		and   	#mFSTAT_FCBEF		;mask it
    		beq	  	ProgSub			;loop if not empty
    		
    		ldhx	  ADRR
    		lda	  	0,x
    		aix	  	#1
    		sthx  	ADRR
    		
    		ldhx 	  ADRS		 	;get flash address
    		sta   	0,x 			;write to flash; latch addr and data
    		aix	  	#1
    		sthx	  ADRS
    		
    		lda     #mBurstProg 	;get flash command
    		sta   	FCMD 			;write the flash command
    		lda   	#mFSTAT_FCBEF 		;mask to initiate command
    		sta   	FSTAT 			;[pwpp] register command
    		dbnz  	LEN,ProgSub		;all bytes in a row?
ChkDoneProg: 
    		lda   	FSTAT 			;[prpp] so FCCF is valid
    		lsla 					;FCCF now in MSB
    		bpl   	ChkDoneProg 	;loop if FCCF = 0
    		
    		ldhx	  STACK
    		txs
        jmp     SUCC	    	;refer status back to PC
ProgSubEnd: 
ProgSubSize: equ (*-ProgSub)


; Writes up to one erase page, origin address in ADRR
			XDEF		Bootstrap

Bootstrap:
			LDHX		#$FA00
			STHX		ADRS
			LDA	  	#111					; Set flash clock divider
			STA		FCDIV
	
    		LDA   	#(mFSTAT_FPVIOL+mFSTAT_FACCERR)
    		STA   	FSTAT 				; Abort commands and clear errors

StartWait:
    		LDA   	FSTAT 				; Check FCBEF (command buffer empty)
    		AND   	#mFSTAT_FCBEF
    		BEQ	  	StartWait

			STA		0,X					; Need to do a page erase - latch the address
			LDA		#$40					; Page erase command
			STA		FCMD					; Write command
			LDA		#mFSTAT_FCBEF		; Inititiate
			STA		FSTAT
			EOR		#0						; Wait a bit
EraWait:
			STA		$1800					; Kick the watchdog timer
			LDA		FSTAT
			BIT		#mFSTAT_FCCF		; Wait for completion
			BEQ		EraWait
			    		
BsProgSub: 	   		
			
			STA		$1800					; Kick the watchdog timer

			LDHX		ADRR					; Get next byte from RAM
			LDA		0,x
			AIX		#1
			STHX		ADRR
			
    		LDHX		ADRS
    		STA   	0,x 					; Write to flash (latch address)
    		AIX	  	#1						; Increment write address
    		STHX	  	ADRS					; Save new address
    		   		    		
    		LDA     	#$20		 			; Byte program command
    		STA   	FCMD 					; Write command
    		LDA   	#mFSTAT_FCBEF 		; Initiate
    		STA   	FSTAT
    		EOR		#0
    		EOR		#0

WaitDone:
    		LDA   	FSTAT 				; Loop until programming is done
    		BIT		#mFSTAT_FCCF
    		BEQ		WaitDone

    		LDA		ADRS					; Check to see if we're done
    		CMP		#$FC					; Only write through $FC00
    		BNE		BsProgSub			; Loop and do the next byte in row    		
    		
			RTS

END            

