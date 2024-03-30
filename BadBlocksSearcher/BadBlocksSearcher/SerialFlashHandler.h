#pragma once

#include <iostream>
#include <array>

/*
	Модель Serial Flash брал из следующих соображений: 
	Тип памяти: NAND - т.к. у нас Serial - память с последовательным доступом.
	Ячейки память поделены на следующие группы: страницы (256 байт) - сектора (4096 байт - 16 страниц) - блоки (65536 байт - 16 секторов - 256 страниц).
	Страница - минимально допустимая единица для стирания-записи-чтения. Однако, даже при записи целого блока - запись всё равно будет происходить по 256 байт, 
	но в цикле, т.к. больше, чем размер страницы, память записать за 1 сеанс не может. 
	Во время стирания байты заполняются значениями 0xFF, т.к. при записи делается побитовое И для получения необходимого значения в ячейке памяти. В случае, если внутри
	записываемой области, позиция текущего записываемого байта не равно 0xFF в Flash - происходит erase ВСЕЙ страницы с предшевствующим сохранением текущей ин-ции в буфер.
	Затем эта ин-ция в буфере объединяется с новой записываемой и повторно ложится по тому же адресу начала страницы.
	Если какая-то ячейка памяти (или последовательность ячеек) является битой, то она всегда хранит значение 0xFF (как сразу после ERASE)
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

public:		// функции стирания памяти
	operation_status_t erase_page_of_memory(uint32_t start_addr);
	operation_status_t erase_sector_of_memory(uint32_t start_addr);
	operation_status_t erase_block_of_memory(uint32_t start_addr);
	operation_status_t erase_full_memory();
		
public:		// функции записи памяти
	operation_status_t write_to_flash_page(uint32_t start_addr, const std::array<uint8_t, PAGE_SIZE_BYTES>& bytes_for_writting);
	operation_status_t write_to_flash_sector(uint32_t start_addr, const std::array<uint8_t, SECTOR_SIZE_BYTES>& bytes_for_writting);
	operation_status_t write_to_flash_block(uint32_t start_addr, const std::array<uint8_t, BLOCK_SIZE_BYTES>& bytes_for_writting);

public:		// функции чтения памяти
	operation_status_t  get_page(uint32_t start_addr, std::array<uint8_t, PAGE_SIZE_BYTES>& arr_for_reading);
	operation_status_t  get_sector(uint32_t start_addr, std::array<uint8_t, SECTOR_SIZE_BYTES>& arr_for_reading);
	operation_status_t  get_block(uint32_t start_addr, std::array<uint8_t, BLOCK_SIZE_BYTES>& arr_for_reading);

private:
	HANDLER_STATUS  m_flash_handler_status;		// текущее состояние обработчика
	bool			m_check_mode;				// включён ли режим тестирования памяти
	uint32_t		m_flash_start_addr;			// начальный адрес обрабатываемой памяти
	uint32_t		m_flash_end_addr;			// конечный адрес обрабатываемой памяти

	std::array<uint8_t, PAGE_SIZE_BYTES> internal_buff;		// внутренний буфер для хранения тестируемой страницы
};

