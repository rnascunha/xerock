#ifndef RAPIDJSON_HELPER_HPP__
#define RAPIDJSON_HELPER_HPP__

#include <string>
#include <rapidjson/document.h>
#include "byte_array.h"

namespace Util{

std::string json_2_string(const rapidjson::Value& d, bool pretty = true);
void print_json(const rapidjson::Value& d, bool pretty = true);

bool json_2_byte_array(const rapidjson::Value& doc, Byte_Array& data);

}//Util

#endif /* RAPIDJSON_HELPER_HPP__ */
