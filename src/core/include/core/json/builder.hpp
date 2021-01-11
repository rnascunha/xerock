#ifndef MESSAGE_BUILDER_JSON_HPP__
#define MESSAGE_BUILDER_JSON_HPP__

#include <string>
#include <vector>

#include "../types.hpp"
#include "byte_array.h"

#include <rapidjson/document.h>

template<typename T>
struct identity { typedef T type; };

namespace Message{

class Builder{
	public:
		Builder();

		template<typename T>
		Builder& add(const char* key, T data){
			return add(key, data, identity<T>());
		}

		std::string get_string();
//		rapidjson::Document* get_document(){
//			return &doc;
//		}
	protected:
		rapidjson::Document doc;

		template<typename T>
		Builder& add(const char* key, T data, identity<T>);
		Builder& add(const char* key, const char* data, identity<const char*>);
		Builder& add(const char* key, std::string const list, identity<std::string>);
		Builder& add(const char* key, std::vector<std::string> const list, identity<std::vector<std::string>>);
		Builder& add(const char* key, std::vector<int> const list, identity<std::vector<int>>);
};

template<typename T>
Builder& Builder::add(const char* key, T data, identity<T>){
	doc.AddMember(rapidjson::StringRef(key), data, doc.GetAllocator());

	return *this;
}

}//Message

//#include "apps/main/json/builder.hpp"
//#include <vector>
//#include <string>
//
//#include <iostream>
//
//int main(){
//	Message::Builder b;
//
//	b.add("t0", true);
//	b.add("t1", 1);
//	b.add("t2", "const char");
//	b.add("t3", "string");
//	b.add("t4", std::vector<int>{1,2,3,4,5});
//	b.add("t5", std::vector<std::string>{"a", "b", "c", "d"});
//
////	Message::Builder d;
////
////	d.add("x0", 3);
////	d.add("x1", "string");
////
////	b.add("t6", d.get_document());
//
//	std::cout << "json: " << b.get_string() << "\n";
//
//	return 0;
//}

#endif /* MESSAGE_BUILDER_JSON_HPP__ */
