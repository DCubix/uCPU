;; [ Basic sprite rendering ]
;; Data block
;; This section is stored into memory at compile time
;; The program doesn't see this.
;; Data storage starts at 0x4000 and ends at 0x5FFF
% 4 4 3 3 3 3 4 4
% 4 3 0 1 1 1 3 4
% 3 0 0 0 1 1 1 3
% 3 1 0 1 1 1 2 3
% 3 1 1 1 1 2 1 3
% 3 1 1 1 2 1 2 3
% 4 3 1 2 1 2 3 4
% 4 4 3 3 3 3 4 4
% 'Hello World!'

init:
	mov $0 0		; x
	mov $1 0		; y
	mov $2 0		; dir x
	mov $3 0		; dir y
	mov $4 1		; speed
	jmp start

start:
	sys 0x0001		; Perform system call 0x0001 (clear screen)
	psh $1
	psh $0
	vspr [0x4000]
	sys 0x0002		; Perform system call 0x0002 (flip screen)
	call move_x
	cmp $0 112
	jge swap_x
	cmp $0 0
	jle swap_x
	call move_y
	cmp $1 88
	jge swap_y
	cmp $1 0
	jle swap_y
	jmp start
	; Game Loop

move_x:
	cmp $2 0
	jeq add_x
	cmp $2 1
	jeq sub_x
	ret

move_y:
	cmp $3 0
	jeq add_y
	cmp $3 1
	jeq sub_y
	ret

add_x:
	add $0 $4
	jmp return

add_y:
	add $1 $4
	jmp return

sub_x:
	sub $0 $4
	jmp return

sub_y:
	sub $1 $4
	jmp return

swap_x:
	xor $2 1
	jmp start

swap_y:
	xor $3 1
	jmp start

return:
	ret
