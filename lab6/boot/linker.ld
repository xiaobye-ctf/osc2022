ENTRY(_start) /* set _start in boot.S as entry point */ 

SECTIONS
{
    . = 0x60000; /* bootloader (start.elf) will loader kernel8.img to 0x80000*/
    __bootloader_start = .;
    __text_start = .;
    .text : 
    {
		KEEP(*(.text.boot))
        *(.text)
    }
    
   . = ALIGN(0x1000);
    __text_end = .;
    __rodata_start = .;
    .rodata : 
    {
        *(.rodata)
    }
    
    . = ALIGN(0x1000);
    __rodata_end = .;
    __data_start = .;
    .data : 
    {
        *(.data)
    }
    
    . = ALIGN(0x1000);
    __data_end = .;
    __bss_start = .;
    .bss : 
    { 
        *(.bss) 
    }

    . = ALIGN(0x1000);
    __bss_end = .;
    __heap_start = .;
    __bootloader_end = .;
    __bootloader_size = (__bootloader_end - __bootloader_start);
}
