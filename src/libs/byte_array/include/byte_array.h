#ifndef BYTE_ARRAY_H__
#define BYTE_ARRAY_H__

#include <vector>
#include <stdint.h>
#include <string>
#include <stddef.h>
#include <sstream>

class Byte_Array : public std::vector<uint8_t>
{
	public:
		Byte_Array() = default;
		Byte_Array(std::string const& s) : std::vector<uint8_t>(s.begin(), s.end()){}
		using std::vector<uint8_t>::vector;

		enum Escape_Format{
			PRINTF = 0,
			HEX,
			OCTO
		};

		std::string to_string() const;
		std::string to_hex(bool up_case = true ,
				const char* separator = "" ,
				unsigned int byte_space  = 1) const;

		std::string to_scape_string(Escape_Format = PRINTF,
				const char* enclose_begin = "\\", const char* enclose_end = "");

		void copy(const std::string& str);
		void copy(const std::stringstream& ss);

		Byte_Array& operator=(const std::string& a);
		Byte_Array& operator=(const std::stringstream& a);

		static std::string byte_to_hex(uint8_t byte, bool up_case = true);
		static std::string byte_to_octo(uint8_t byte);
};

#endif /* BYTE_ARRAY_H__ */
