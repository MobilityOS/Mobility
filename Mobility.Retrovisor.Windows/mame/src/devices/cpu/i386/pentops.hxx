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

void i386_device::pentium_prefetch_m8()    // Opcode 0x0f 18
{
	uint8_t modrm = FETCH();
	uint32_t ea = GetEA(modrm,0);
	// TODO: manage the cache if present
	CYCLES(1+(ea & 1)); // TODO: correct cycle count
}

void i386_device::pentium_cmovo_r16_rm16()    // Opcode 0x0f 40
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_OF == 1)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_OF == 1)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovo_r32_rm32()    // Opcode 0x0f 40
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_OF == 1)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_OF == 1)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovno_r16_rm16()    // Opcode 0x0f 41
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_OF == 0)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_OF == 0)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovno_r32_rm32()    // Opcode 0x0f 41
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_OF == 0)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_OF == 0)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovb_r16_rm16()    // Opcode 0x0f 42
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_CF == 1)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_CF == 1)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovb_r32_rm32()    // Opcode 0x0f 42
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_CF == 1)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_CF == 1)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovae_r16_rm16()    // Opcode 0x0f 43
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_CF == 0)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_CF == 0)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovae_r32_rm32()    // Opcode 0x0f 43
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_CF == 0)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_CF == 0)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmove_r16_rm16()    // Opcode 0x0f 44
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_ZF == 1)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_ZF == 1)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmove_r32_rm32()    // Opcode 0x0f 44
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_ZF == 1)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_ZF == 1)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovne_r16_rm16()    // Opcode 0x0f 45
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_ZF == 0)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_ZF == 0)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovne_r32_rm32()    // Opcode 0x0f 45
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_ZF == 0)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_ZF == 0)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovbe_r16_rm16()    // Opcode 0x0f 46
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_CF == 1) || (m_ZF == 1))
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_CF == 1) || (m_ZF == 1))
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovbe_r32_rm32()    // Opcode 0x0f 46
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_CF == 1) || (m_ZF == 1))
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_CF == 1) || (m_ZF == 1))
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmova_r16_rm16()    // Opcode 0x0f 47
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_CF == 0) && (m_ZF == 0))
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_CF == 0) && (m_ZF == 0))
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmova_r32_rm32()    // Opcode 0x0f 47
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_CF == 0) && (m_ZF == 0))
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_CF == 0) && (m_ZF == 0))
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovs_r16_rm16()    // Opcode 0x0f 48
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF == 1)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF == 1)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovs_r32_rm32()    // Opcode 0x0f 48
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF == 1)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF == 1)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovns_r16_rm16()    // Opcode 0x0f 49
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF == 0)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF == 0)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovns_r32_rm32()    // Opcode 0x0f 49
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF == 0)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF == 0)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovp_r16_rm16()    // Opcode 0x0f 4a
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_PF == 1)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_PF == 1)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovp_r32_rm32()    // Opcode 0x0f 4a
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_PF == 1)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_PF == 1)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovnp_r16_rm16()    // Opcode 0x0f 4b
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_PF == 0)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_PF == 0)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovnp_r32_rm32()    // Opcode 0x0f 4b
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_PF == 0)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_PF == 0)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovl_r16_rm16()    // Opcode 0x0f 4c
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF != m_OF)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF != m_OF)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovl_r32_rm32()    // Opcode 0x0f 4c
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF != m_OF)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF != m_OF)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovge_r16_rm16()    // Opcode 0x0f 4d
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF == m_OF)
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF == m_OF)
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovge_r32_rm32()    // Opcode 0x0f 4d
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if (m_SF == m_OF)
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if (m_SF == m_OF)
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovle_r16_rm16()    // Opcode 0x0f 4e
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_ZF == 1) || (m_SF != m_OF))
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_ZF == 1) || (m_SF != m_OF))
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovle_r32_rm32()    // Opcode 0x0f 4e
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_ZF == 1) || (m_SF != m_OF))
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_ZF == 1) || (m_SF != m_OF))
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovg_r16_rm16()    // Opcode 0x0f 4f
{
	uint16_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_ZF == 0) && (m_SF == m_OF))
		{
			src = LOAD_RM16(modrm);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_ZF == 0) && (m_SF == m_OF))
		{
			src = READ16(ea);
			STORE_REG16(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_cmovg_r32_rm32()    // Opcode 0x0f 4f
{
	uint32_t src;
	uint8_t modrm = FETCH();

	if( modrm >= 0xc0 )
	{
		if ((m_ZF == 0) && (m_SF == m_OF))
		{
			src = LOAD_RM32(modrm);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
	else
	{
		uint32_t ea = GetEA(modrm,0);
		if ((m_ZF == 0) && (m_SF == m_OF))
		{
			src = READ32(ea);
			STORE_REG32(modrm, src);
		}
		CYCLES(1); // TODO: correct cycle count
	}
}

void i386_device::pentium_movnti_m16_r16() // Opcode 0f c3
{
	uint8_t modrm = FETCH();
	if( modrm >= 0xc0 ) {
		// unsupported by cpu
		CYCLES(1);     // TODO: correct cycle count
	} else {
		// TODO: manage the cache if present
		uint32_t ea = GetEA(modrm, 0);
		WRITE16(ea,LOAD_RM16(modrm));
		CYCLES(1);     // TODO: correct cycle count
	}
}

void i386_device::pentium_movnti_m32_r32() // Opcode 0f c3
{
	uint8_t modrm = FETCH();
	if( modrm >= 0xc0 ) {
		// unsupported by cpu
		CYCLES(1);     // TODO: correct cycle count
	} else {
		// TODO: manage the cache if present
		uint32_t ea = GetEA(modrm, 0);
		WRITE32(ea,LOAD_RM32(modrm));
		CYCLES(1);     // TODO: correct cycle count
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

void i386_device::pentium_popcnt_r16_rm16()    // Opcode f3 0f b8
{
	uint16_t src;
	uint8_t modrm = FETCH();
	int n,count;

	if( modrm >= 0xc0 ) {
		src = LOAD_RM16(modrm);
	} else {
		uint32_t ea = GetEA(modrm,0);
		src = READ16(ea);
	}
	count=0;
	for (n=0;n < 16;n++) {
		count=count+(src & 1);
		src=src >> 1;
	}
	STORE_REG16(modrm, count);
	CYCLES(1); // TODO: correct cycle count
}

void i386_device::pentium_popcnt_r32_rm32()    // Opcode f3 0f b8
{
	uint32_t src;
	uint8_t modrm = FETCH();
	int n,count;

	if( modrm >= 0xc0 ) {
		src = LOAD_RM32(modrm);
	} else {
		uint32_t ea = GetEA(modrm,0);
		src = READ32(ea);
	}
	count=0;
	for (n=0;n < 32;n++) {
		count=count+(src & 1);
		src=src >> 1;
	}
	STORE_REG32(modrm, count);
	CYCLES(1); // TODO: correct cycle count
}

void i386_device::pentium_tzcnt_r16_rm16()
{
	// for CPUs that don't support TZCNT, fall back to BSF
	i386_bsf_r16_rm16();
	// TODO: actually implement TZCNT
}

void i386_device::pentium_tzcnt_r32_rm32()
{
	// for CPUs that don't support TZCNT, fall back to BSF
	i386_bsf_r32_rm32();
	// TODO: actually implement TZCNT
}

static inline int8_t SaturatedSignedWordToSignedByte(int16_t word)
{
	if (word > 127)
		return 127;
	if (word < -128)
		return -128;
	return (int8_t)word;
}

static inline uint8_t SaturatedSignedWordToUnsignedByte(int16_t word)
{
	if (word > 255)
		return 255;
	if (word < 0)
		return 0;
	return (uint8_t)word;
}

static inline int16_t SaturatedSignedDwordToSignedWord(int32_t dword)
{
	if (dword > 32767)
		return 32767;
	if (dword < -32768)
		return -32768;
	return (int16_t)dword;
}

static inline uint16_t SaturatedSignedDwordToUnsignedWord(int32_t dword)
{
	if (dword > 65535)
		return 65535;
	if (dword < 0)
		return 0;
	return (uint16_t)dword;
}
