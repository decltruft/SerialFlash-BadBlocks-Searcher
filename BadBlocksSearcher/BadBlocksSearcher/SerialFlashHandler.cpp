#include "SerialFlashHandler.h"

SerialFlashHandler::SerialFlashHandler(uint32_t start_addr, uint32_t end_addr) : 
	m_flash_handler_status(HANDLER_STATUS::FREE),
	m_check_mode(false),
	m_flash_start_addr(start_addr),
	m_flash_end_addr(end_addr)
{

}

uint16_t SerialFlashHandler::get_page_crc16(uint32_t start_addr)
{
	std::array<uint8_t, PAGE_SIZE_BYTES> arr_for_page_reading;
	get_page(start_addr, arr_for_page_reading);
	return Calc_CRC16(arr_for_page_reading);
}

SerialFlashHandler::flash_handler_status_t SerialFlashHandler::get_handler_status() const
{
	return m_flash_handler_status;
}

uint32_t SerialFlashHandler::get_start_memory_address() const
{
	return m_flash_start_addr;
}

uint32_t SerialFlashHandler::get_end_memory_address() const
{
	return m_flash_end_addr;
}

void SerialFlashHandler::enable_check_mode()
{
	m_check_mode = true;
}

void SerialFlashHandler::disable_check_mode()
{
	m_check_mode = false;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::erase_page_of_memory(uint32_t start_addr)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::ERASE_PAGE;

	// TODO: код стирания указанной страницы

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::erase_sector_of_memory(uint32_t start_addr)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::ERASE_SECTOR;

	for (uint32_t p_index(start_addr); p_index < start_addr + SECTOR_SIZE_BYTES; p_index += PAGE_SIZE_BYTES)
	{	// стираем указанный сектор
		erase_page_of_memory(p_index);
	}

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::erase_block_of_memory(uint32_t start_addr)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::ERASE_BLOCK;

	for (uint32_t b_index(start_addr); b_index < start_addr + BLOCK_SIZE_BYTES; b_index += SECTOR_SIZE_BYTES)
	{	// стираем указанный блок
		erase_page_of_memory(b_index);
	}

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::erase_full_memory()
{
	if (HANDLER_STATUS::FREE != m_flash_handler_status)
	{
		return OPERATION_RESULT::FAIL;
	}

	for (uint32_t p_index(m_flash_start_addr); p_index < m_flash_end_addr; p_index += PAGE_SIZE_BYTES)
	{	// стираем всю память
		erase_page_of_memory(p_index);
	}

	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::write_to_flash_page(uint32_t start_addr, const std::array<uint8_t, PAGE_SIZE_BYTES>& bytes_for_writting)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::WRITE_PAGE;

	// TODO: код записи страницы в память

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::write_to_flash_sector(uint32_t start_addr, const std::array<uint8_t, SECTOR_SIZE_BYTES>& bytes_for_writting)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::WRITE_SECTOR;

	std::array<uint8_t, PAGE_SIZE_BYTES> arr_for_page_writting;
	for (uint32_t s_index(0); s_index < start_addr + SECTOR_SIZE_BYTES; s_index += PAGE_SIZE_BYTES)
	{	// запись сектора в память
		for (uint32_t p_index(0); p_index < PAGE_SIZE_BYTES; ++p_index)
		{
			arr_for_page_writting[p_index] = bytes_for_writting[p_index + s_index];
		}
		write_to_flash_page(s_index, arr_for_page_writting);
	}
	
	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t SerialFlashHandler::write_to_flash_block(uint32_t start_addr, const std::array<uint8_t, BLOCK_SIZE_BYTES>& bytes_for_writting)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::WRITE_BLOCK;

	std::array<uint8_t, SECTOR_SIZE_BYTES> arr_for_sector_writting;
	for (uint32_t b_index(0); b_index < start_addr + BLOCK_SIZE_BYTES; b_index += SECTOR_SIZE_BYTES)
	{	// запись блока в память
		for (uint32_t s_index(0); s_index < SECTOR_SIZE_BYTES; ++s_index)
		{
			arr_for_sector_writting[s_index] = bytes_for_writting[s_index + b_index];
		}
		write_to_flash_sector(b_index, arr_for_sector_writting);
	}

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t    SerialFlashHandler::get_page(uint32_t start_addr, std::array<uint8_t, PAGE_SIZE_BYTES>& arr_for_reading)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::READ_PAGE;

	// TODO: код записи страницы в память

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t  SerialFlashHandler::get_sector(uint32_t start_addr, std::array<uint8_t, SECTOR_SIZE_BYTES>& arr_for_reading)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::READ_SECTOR;

	std::array<uint8_t, PAGE_SIZE_BYTES> arr_for_page_reading;
	for (uint32_t s_index(0); s_index < start_addr + SECTOR_SIZE_BYTES; s_index += PAGE_SIZE_BYTES)
	{	// чтение сектора из памяти
		get_page(s_index, arr_for_page_reading);
		for (uint32_t index(s_index), i(0); index < s_index + PAGE_SIZE_BYTES; ++index, ++i)
		{
			arr_for_reading[index] = arr_for_page_reading[i];
		}
	}

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

SerialFlashHandler::operation_status_t   SerialFlashHandler::get_block(uint32_t start_addr, std::array<uint8_t, BLOCK_SIZE_BYTES>& arr_for_reading)
{
	if ((start_addr < m_flash_start_addr) || (start_addr > m_flash_end_addr))
	{
		return OPERATION_RESULT::FAIL;
	}
	m_flash_handler_status = HANDLER_STATUS::READ_BLOCK;

	std::array<uint8_t, SECTOR_SIZE_BYTES> arr_for_sector_reading;
	for (uint32_t b_index(0); b_index < start_addr + BLOCK_SIZE_BYTES; b_index += SECTOR_SIZE_BYTES)
	{	// чтение блока из памяти
		get_sector(b_index, arr_for_sector_reading);
		for (uint32_t index(b_index), i(0); index < b_index + SECTOR_SIZE_BYTES; ++index, ++i)
		{
			arr_for_reading[index] = arr_for_sector_reading[i];
		}
	}

	m_flash_handler_status = HANDLER_STATUS::FREE;
	return OPERATION_RESULT::OK;
}

static const uint8_t table_crc_hi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t Calc_CRC16(const std::array<uint8_t, PAGE_SIZE_BYTES>& arr)
{
	uint32_t buffer_length = PAGE_SIZE_BYTES;
	uint32_t arr_index = 0;

	uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
	uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
	unsigned int i; /* will index into CRC lookup */

	/* pass through message buffer */
	while (buffer_length--) {
		i = crc_lo ^ arr[arr_index++]; /* calculate the CRC  */
		crc_lo = crc_hi ^ table_crc_hi[i];
		crc_hi = table_crc_lo[i];
	}

	return (crc_hi << 8 | crc_lo);
}