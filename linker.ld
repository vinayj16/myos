/* Higher half kernel linker script */

ENTRY(_start)

/* Define the entry point */
SECTIONS
{
    /* Start at 1MB physical, 3GB + 1MB virtual */
    . = 0xFFFFFFFF80100000;
    _kernel_start = .;

    /* Multiboot header must be in the first 8KB */
    .multiboot : AT(ADDR(.multiboot) - 0xC0000000) {
        *(.multiboot_header)
    }

    /* Executable code */
    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
        *(.text*)
        *(.gnu.linkonce.t*)
    }

    /* Read-only data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata*)
        *(.gnu.linkonce.r*)
    }

    /* Read-write data (initialized) */
    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
        *(.data*)
        *(.gnu.linkonce.d*)
    }

    /* Read-write data (uninitialized) and stack */
    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
        *(COMMON)
        *(.bss*)
        *(.gnu.linkonce.b*)
        . = ALIGN(16);
        _kernel_end = .;
    }

    /* Kernel stack */
    .stack ALIGN(4K) : AT(ADDR(.stack) - 0xC0000000) {
        _stack_bottom = .;
        . += 64K;  /* 64KB kernel stack */
        _stack_top = .;
    }

    /* Symbols for memory management */
    _kernel_virtual_start = 0xFFFFFFFF80000000;
    _kernel_physical_start = 0x00100000;
    _kernel_size = _kernel_end - _kernel_start;

    /* Discard sections we don't need */
    /DISCARD/ : {
        *(.comment*)
        *(.eh_frame*)
        *(.note*)
    }
}
