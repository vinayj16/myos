; debug.asm - Simple debug output for early boot
section .text
global debug_print

; void debug_print(const char* message, uint32_t length)
debug_print:
    pusha
    mov edx, [esp + 36]    ; length
    mov ecx, [esp + 32]    ; message
    mov ebx, 1             ; file descriptor (stdout)
    mov eax, 4             ; sys_write
    int 0x80
    popa
    ret
