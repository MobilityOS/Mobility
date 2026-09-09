// license:BSD-3-Clause
// copyright-holders:Ville Linde, Barry Rodewald, Carl, Philip Bennett, Samuele Zannoli
// Pentium+ specific opcodes

void i386_device::pentium_rdmsr()          // Opcode 0x0f 32
{
	uint64_t data;
	bool valid_msr = false;

	// call the model specific implementation
	data = opcode_rdmsr(valid_msr);
	if (m_CPL != 0 || valid_msr == false) // if current privilege level isn't 0 or the register isn't recognized ...
		FAULT(FAULT_GP, 0) // ... throw a general exception fault
	else
	{
		REG32(EDX) = data >> 32;
		REG32(EAX) = data & 0xffffffff;
	}

	CYCLES(CYCLES_RDMSR);
}

void i386_device::pentium_wrmsr()          // Opcode 0x0f 30
{
	uint64_t data;
	bool valid_msr = false;

	data = (uint64_t)REG32(EAX);
	data |= (uint64_t)(REG32(EDX)) << 32;

	// call the model specific implementation
	opcode_wrmsr(data, valid_msr);

	if(m_CPL != 0 || valid_msr == 0) // if current privilege level isn't 0 or the register isn't recognized
		FAULT(FAULT_GP,0) // ... throw a general exception fault

	CYCLES(1);     // TODO: correct cycle count (~30-45)
}

void i386_device::pentium_rdtsc()          // Opcode 0x0f 31
{
	uint64_t ts = m_tsc + (m_base_cycles - m_cycles);
	REG32(EAX) = (uint32_t)(ts);
	REG32(EDX) = (uint32_t)(ts >> 32);

	CYCLES(CYCLES_RDTSC);
}

void i386_device::pentium_ud2()    // Opcode 0x0f 0b
{
	i386_trap(6, 0);
}

void i386_device::pentium_rsm()
{
	if(!m_smm)
	{
		LOGMASKED(LOG_INVALID_OPCODE, "i386: Invalid RSM outside SMM at %08X\n", m_pc - 1);
		i386_trap(6, 0);
		return;
	}

	leave_smm();
	if(m_smi_latched)
	{
		enter_smm();
		return;
	}
	if(m_nmi_latched)
	{
		m_nmi_latched = false;
		i386_trap(2, 1);
	}
}

void i386_device::pentium_cmpxchg8b_m64()  // Opcode 0x0f c7
{
	uint8_t modm = FETCH();
	if( modm >= 0xc0 ) {
		report_invalid_modrm("cmpxchg8b_m64", modm);
	} else {
		uint32_t ea = GetEA(modm, 0);
		uint64_t value = READ64(ea);
		uint64_t edx_eax = (((uint64_t) REG32(EDX)) << 32) | REG32(EAX);
		uint64_t ecx_ebx = (((uint64_t) REG32(ECX)) << 32) | REG32(EBX);

		if( value == edx_eax ) {
			WRITE64(ea, ecx_ebx);
			m_ZF = 1;
			CYCLES(CYCLES_CMPXCHG_REG_MEM_T);
		} else {
			REG32(EDX) = (uint32_t) (value >> 32);
			REG32(EAX) = (uint32_t) (value >>  0);
			m_ZF = 0;
			CYCLES(CYCLES_CMPXCHG_REG_MEM_F);
		}
	}
}
