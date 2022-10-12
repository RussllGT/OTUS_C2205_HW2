#include <stdbool.h>
#include "decode.h";

#define FIRST_BYTE_MSB 0b11000000
#define SECOND_BYTE_MSB 0b10000000
#define FIRST_BYTE_MASK 0b00011111
#define SECOND_BYTE_MASK 0b00111111

/// <summary>
/// Меняет местами байты в uint16_t
/// </summary>
/// <param name="value">Исходное число</param>
/// <returns>Полученное число</returns>
uint16_t reverse(uint16_t value)
{
	uint8_t* first = &value;
	uint8_t* second = first + 1;

	*first = *first ^ *second;
	*second = *second ^ *first;
	*first = *first ^ *second;

	return value;
}

/// <summary>
/// Преобразует кодовую точку unicode в код UTF-8
/// </summary>
/// <param name="point">Кодовая точка</param>
/// <param name="little">Используется ли little-endian процессор</param>
/// <returns>Ккод символа UTF-8</returns>
uint16_t unicode_to_utf8(uint16_t point, bool little)
{
	uint16_t shift = point / 64;
	uint8_t* second_ptr = (uint8_t*)&point;
	uint8_t* first_ptr = (uint8_t*)&shift;
	if (!little)
	{
		first_ptr++;
		second_ptr++;
	}

	uint8_t second = SECOND_BYTE_MASK & *second_ptr;
	uint8_t first = FIRST_BYTE_MASK & *first_ptr;

	uint16_t result = 0;
	uint8_t* ptr = &result;

	if (little)
	{
		*(ptr++) = FIRST_BYTE_MSB | first;
		*ptr = SECOND_BYTE_MSB | second;
	}
	else
	{
		*(ptr++) = SECOND_BYTE_MSB | second;
		*ptr = FIRST_BYTE_MSB | first;
	}

	return result;
}


/// <summary>
/// Метод возвращает код символа в заданной кодировке (UTF-8)
/// </summary>
/// <param name="symbol">Указатель на символ в таблице кодировок</param>
/// <param name="little">Используется ли little-endian процессор</param>
/// <returns>Код символа в заданной кодировке (UTF-8)</returns>
uint16_t get_symbol(const symbol_t* symbol, bool little)
{
	return unicode_to_utf8(symbol->to, little);
}

/// <summary>
/// Записывает перкодированные данные
/// </summary>
/// <param name="output">Указатель на начало записи</param>
/// <param name="start">Указатель на начало исходной записи</param>
/// <param name="end">Указатель на конец исходной записи</param>
/// <param name="decoder">Указатель на таблицу символов для перекодирования</param>
/// <returns>Возвращает указатель на конец записи</returns>
uint8_t* write_output(uint8_t* output, const uint8_t* start, const uint8_t* end, const symbol_t* decoder)
{
	//Проверка, является ли процессор little-endian
	uint16_t endian_test = 0x0001;
	bool little = *((uint8_t*)&endian_test);

	//Указатель на текущий символ декодирования
	symbol_t* ptr;

	while (start != end)
	{
		// Если символ начинается с 0, то он совпадает с UTF-8
		if (*start < 128) 
		{
			*output = *start;
			output++;
		}
		else
		{
			ptr = decoder;
			while (ptr->from)
			{
				if (ptr->from == *start)
				{
					*(uint16_t*)output = get_symbol(ptr, little);
					output += 2;
					break;
				}
				ptr++;
			}
		}
		start++;
	}

	return output;
}