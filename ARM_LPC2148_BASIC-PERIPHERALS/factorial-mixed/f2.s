

  area aa,code

 	 export fact

fact
  mov r1,#01
loop  mul r2,r1,r0

  mov r1,r2
  sub r0,r0,#1
  cmp r0,#1
  bne loop
  mov r0,r2	; to update c variable(return)
  mov pc,lr
  end