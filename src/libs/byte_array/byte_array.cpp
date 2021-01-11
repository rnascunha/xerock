#include "byte_array.h"
#include <ostream>
#include <iomanip>
#include <sstream>

std::string Byte_Array::to_hex(bool up_case /* = true */, const char* separator /* = "" */,
		unsigned int byte_space /* = 1 */) const
{
	std::ostringstream ret;

	for (std::string::size_type i = 0; i < size(); ++i){
		ret << byte_to_hex(this->at(i), up_case);

		if((i + 1) != size() && ((i+1) % byte_space) == 0){
			ret << separator;
		}
	}

	return std::move(ret.str());
}

std::string Byte_Array::to_string() const
{
	std::string s(reinterpret_cast<char const*>(data()), size());

	return s;
}

std::string Byte_Array::to_scape_string(Byte_Array::Escape_Format format /* = PRINTF */,
		const char* enclose_begin /* = "\\" */, const char* enclose_end /* = "" */)
{
	std::string str;

	for(unsigned int i = 0; i < size(); i++){
		if(isprint((int)at(i)) && (int)at(i) != '\\'){
			str.push_back(at(i));
			continue;
		}
		str += enclose_begin;
		if(format == PRINTF){
			switch(at(i)){
				case 0x5C:
					str.push_back('\\');
					break;
				case 0xA:
					str.push_back('n');
					break;
				case 0xD:
					str.push_back('n');
					break;
				case 0x9:
					str.push_back('t');
					break;
				case 0xB:
					str.push_back('v');
					break;
				default:
					str.push_back('x');
					str += byte_to_hex((int)at(i));
					break;
			}
		} else if(format == HEX){
			str.push_back('x');
			str += byte_to_hex((int)at(i));
		} else {
			str += byte_to_octo((int)at(i));
		}
		str += enclose_end;
	}

	return str;
}

std::string Byte_Array::byte_to_hex(uint8_t byte, bool up_case /* = true */)
{
	std::ostringstream ret;

	ret << std::hex
			<< std::setfill('0')
			<< std::setw(2)
			<< (up_case ? std::uppercase : std::nouppercase)
			<< (int)byte;

	return std::move(ret.str());
}

std::string Byte_Array::byte_to_octo(uint8_t byte)
{
	std::ostringstream ret;

	ret << std::oct
			<< std::setfill('0')
			<< std::setw(3)
			<< (int)byte;

	return std::move(ret.str());
}

void Byte_Array::copy(const std::string& str){
	clear();
	for(unsigned int i = 0; i < str.size(); i++)
		push_back(str[i]);
}

void Byte_Array::copy(const std::stringstream& ss)
{
	copy(ss.str());
}

Byte_Array& Byte_Array::operator=(const std::string& a)
{
	this->copy(a);

	return *this;
}

Byte_Array& Byte_Array::operator=(const std::stringstream& a)
{
	this->copy(a);

	return *this;
}
