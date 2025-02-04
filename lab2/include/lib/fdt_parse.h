#ifndef _FDT_PARSE_H_
#define _FDT_PARSE_H_
#include "types.h"

#define FDT_BEGIN_NODE  bswap32(0x00000001)
#define FDT_END_NODE    bswap32(0x00000002)
#define FDT_PROP        bswap32(0x00000003)
#define FDT_NOP         bswap32(0x00000004)
#define FDT_END         bswap32(0x00000009)
typedef struct{
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
}fdt_header;

typedef struct{
    uint64_t address;
    uint64_t size;
} fdt_reserve_entry;

typedef struct{
    char *name;
} fdt_node;

typedef struct{
    char* name;
    uint8_t* value;
    uint32_t value_len;
} fdt_property;

typedef void* (*fdt_callback)(uint32_t, fdt_node*, fdt_property*, int32_t);
extern void fdt_parser(uint8_t*, fdt_callback);
extern void* fdt_print_callback(uint32_t, fdt_node*, fdt_property*, int32_t);
#endif
