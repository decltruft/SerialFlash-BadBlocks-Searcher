#pragma once

#include <iostream>
#include <array>

/*
	������ Serial Flash ���� �� ��������� �����������: 
	��� ������: NAND - �.�. � ��� Serial - ������ � ���������������� ��������.
	������ ������ �������� �� ��������� ������: �������� (256 ����) - ������� (4096 ���� - 16 �������) - ����� (65536 ���� - 16 �������� - 256 �������).
	�������� - ���������� ���������� ������� ��� ��������-������-������. ������, ���� ��� ������ ������ ����� - ������ �� ����� ����� ����������� �� 256 ����, 
	�� � �����, �.�. ������, ��� ������ ��������, ������ �������� �� 1 ����� �� �����. 
	�� ����� �������� ����� ����������� ���������� 0xFF, �.�. ��� ������ �������� ��������� � ��� ��������� ������������ �������� � ������ ������. � ������, ���� ������
	������������ �������, ������� �������� ������������� ����� �� ����� 0xFF � Flash - ���������� erase ���� �������� � ��������������� ����������� ������� ��-��� � �����.
	����� ��� ��-��� � ������ ������������ � ����� ������������ � �������� ������� �� ���� �� ������ ������ ��������.
	���� �����-�� ������ ������ (��� ������������������ �����) �������� �����, �� ��� ������ ������ �������� 0xFF (��� ����� ����� ERASE)
*/

constexpr uint32_t PAGE_SIZE_BYTES = 256;
constexpr uint32_t SECTOR_SIZE_BYTES = 4096;
constexpr uint32_t BLOCK_SIZE_BYTES = 65536;

uint16_t Calc_CRC16(const std::array<uint8_t, PAGE_SIZE_BYTES>&);

class SerialFlashHandler
{
public:
	enum class operation_status_t : uint8_t
	{
		OK,
		FAIL
	};
	using OPERATION_RESULT = SerialFlashHandler::operation_status_t;

	enum class flash_handler_status_t : uint8_t
	{
		FREE,

		ERASE_FULL_MEMORY,
		ERASE_PAGE,
		ERASE_SECTOR,
		ERASE_BLOCK,

		WRITE_PAGE,
		WRITE_SECTOR,
		WRITE_BLOCK,

		READ_PAGE,
		READ_SECTOR,
		READ_BLOCK
	};
	using HANDLER_STATUS = SerialFlashHandler::flash_handler_status_t;

public:
	SerialFlashHandler(uint32_t start_addr = 0x0, uint32_t end_addr = 0x0);
	flash_handler_status_t get_handler_status() const;
	uint32_t			   get_start_memory_address() const;
	uint32_t			   get_end_memory_address() const;
	void				   enable_check_mode();
	void				   disable_check_mode();
	uint16_t			   get_page_crc16(uint32_t start_addr);

public:		// ������� �������� ������
	operation_status_t erase_page_of_memory(uint32_t start_addr);
	operation_status_t erase_sector_of_memory(uint32_t start_addr);
	operation_status_t erase_block_of_memory(uint32_t start_addr);
	operation_status_t erase_full_memory();
		
public:		// ������� ������ ������
	operation_status_t write_to_flash_page(uint32_t start_addr, const std::array<uint8_t, PAGE_SIZE_BYTES>& bytes_for_writting);
	operation_status_t write_to_flash_sector(uint32_t start_addr, const std::array<uint8_t, SECTOR_SIZE_BYTES>& bytes_for_writting);
	operation_status_t write_to_flash_block(uint32_t start_addr, const std::array<uint8_t, BLOCK_SIZE_BYTES>& bytes_for_writting);

public:		// ������� ������ ������
	operation_status_t  get_page(uint32_t start_addr, std::array<uint8_t, PAGE_SIZE_BYTES>& arr_for_reading);
	operation_status_t  get_sector(uint32_t start_addr, std::array<uint8_t, SECTOR_SIZE_BYTES>& arr_for_reading);
	operation_status_t  get_block(uint32_t start_addr, std::array<uint8_t, BLOCK_SIZE_BYTES>& arr_for_reading);

private:
	HANDLER_STATUS  m_flash_handler_status;		// ������� ��������� �����������
	bool			m_check_mode;				// ������� �� ����� ������������ ������
	uint32_t		m_flash_start_addr;			// ��������� ����� �������������� ������
	uint32_t		m_flash_end_addr;			// �������� ����� �������������� ������

	std::array<uint8_t, PAGE_SIZE_BYTES> internal_buff;		// ���������� ����� ��� �������� ����������� ��������
};

