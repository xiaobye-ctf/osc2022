ENTRY(start) 

SECTIONS
{
    . = 0;
    __text_start = .;
    .text : 
    {
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

}
