; Simple calculator program
start:
	in $0			; First number
	in $1			; Second number
	in $2			; Operator (0 = +, 1 = -, 2 = *, 3 = /, 4 = factorial)
	mov $3 0		; Result
	cmp $2 0
	jeq op_add
	cmp $2 1
	jeq op_sub
	cmp $2 2
	jeq op_mul
	cmp $2 3
	jeq op_div
	cmp $2 4
	jeq op_fact
	out 0x49
	out 0x6e
	out 0x76
	out 0x61
	out 0x6c
	out 0x69
	out 0x64
	out 0x4f
	out 0x70
	out 0xA
	end

op_add:
	mov $3 $0
	add $3 $1
	jmp show_res

op_sub:
	mov $3 $0
	sub $3 $1
	jmp show_res

op_mul:
	mov $3 $0
	mul $3 $1
	jmp show_res

op_div:
	mov $3 $0
	div $3 $1
	jmp show_res

op_fact:
	cmp $0 0
	jeq ret_one
	cmp $0 1
	jeq ret_one
	mov $3 $0
	jmp fact_step
	
fact_step:
	sub $0 1
	mul $3 $0
	outn $3
	out 0xA
	cmp $0 1
	jgt fact_step
	jmp show_res

ret_one:
	mov $3 1
	jmp show_res

show_res:
	out 0x52
	out 0x65
	out 0x73
	out 0x3a
	outn $3
	out 0xA
	end
