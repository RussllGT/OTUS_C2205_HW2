#include <stdbool.h>
#include "decode.h";

#define FIRST_BYTE_MSB 0b11000000
#define SECOND_BYTE_MSB 0b10000000
#define FIRST_BYTE_MASK 0b00011111
#define SECOND_BYTE_MASK 0b00111111

/// <summary>
/// ������ ������� ����� � uint16_t
/// </summary>
/// <param name="value">�������� �����</param>
/// <returns>���������� �����</returns>
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
/// ����������� ������� ����� unicode � ��� UTF-8
/// </summary>
/// <param name="point">������� �����</param>
/// <param name="little">������������ �� little-endian ���������</param>
/// <returns>���� ������� UTF-8</returns>
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
/// ����� ���������� ��� ������� � �������� ��������� (UTF-8)
/// </summary>
/// <param name="symbol">��������� �� ������ � ������� ���������</param>
/// <param name="little">������������ �� little-endian ���������</param>
/// <returns>��� ������� � �������� ��������� (UTF-8)</returns>
uint16_t get_symbol(const symbol_t* symbol, bool little)
{
	return unicode_to_utf8(symbol->to, little);
}

/// <summary>
/// ���������� ��������������� ������
/// </summary>
/// <param name="output">��������� �� ������ ������</param>
/// <param name="start">��������� �� ������ �������� ������</param>
/// <param name="end">��������� �� ����� �������� ������</param>
/// <param name="decoder">��������� �� ������� �������� ��� ���������������</param>
/// <returns>���������� ��������� �� ����� ������</returns>
uint8_t* write_output(uint8_t* output, const uint8_t* start, const uint8_t* end, const symbol_t* decoder)
{
	//��������, �������� �� ��������� little-endian
	uint16_t endian_test = 0x0001;
	bool little = *((uint8_t*)&endian_test);

	//��������� �� ������� ������ �������������
	symbol_t* ptr;

	while (start != end)
	{
		// ���� ������ ���������� � 0, �� �� ��������� � UTF-8
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