/* Simple linker script for MyOS */

ENTRY(_start)

SECTIONS
{
    /* Start at 1MB physical address */
    . = 1M;

    /* Multiboot header must be at the beginning */
    .multiboot_header : {
        *(.multiboot_header)
    }

    /* Code section */
    .text : {
        *(.text)
        *(.text.*)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    /* Data section */
    .data : {
        *(.data)
        *(.data.*)
    }

    /* BSS section */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /* Discard debug info */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}
