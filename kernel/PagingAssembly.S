.section .text
.global copy_page_physical   
copy_page_physical:
   push %ebx              # According to __cdecl, we must preserve the contents of EBX.
   pushf                 # push EFLAGS, so we can pop it and reenable interrupts
                         # later, if they were enabled anyway.
   cli                   # Disable interrupts, so we aren't interrupted.
                         # Load these in BEFORE we disable paging!
   mov 12(%esp), %ebx     # Source address
   mov 16(%esp), %ecx      # Destination address

   mov %cr0, %edx          # Get the control register...
   and 0x7fffffff, %edx    # and...
   mov %edx, %cr0          # Disable paging.

   mov 1024, %edx         # 1024*4bytes = 4096 bytes to copy

.loop:
   mov (%ebx), %eax        # Get the word at the source address
   mov %eax, (%ecx)        # Store it at the dest address
   add 4, %ebx            # Source address += sizeof(word)
   add 4, %ecx           # Dest address += sizeof(word)
   dec %edx               # One less word to do
   jnz .loop

   mov %cr0, %edx          # Get the control register again
   or  0x80000000, %edx   # and...
   mov %edx, %cr0          # Enable paging.

   popf                  # Pop EFLAGS back.
   pop %ebx               # Get the original value of EBX back.
   ret

.global perform_task_switch
perform_task_switch:
     cli
     mov 4(%esp), %ecx # EIP
     mov 8(%esp), %eax  # physical address of current directory
     mov  12(%esp), %ebp  # EBP
     mov 16(%esp), %esp  # ESP
     mov  %eax, %cr3       # set the page directory
     # mov  0x12345, %eax   # magic number to detect a task switch
     sti
     jmp *%ecx