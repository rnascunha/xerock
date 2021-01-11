#include "builder.hpp"
#include <rapidjson/writer.h>

namespace Message{

Builder::Builder(){
	doc.SetObject();
}

std::string Builder::get_string(){
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);

	doc.Accept(writer);

	return std::string(s.GetString());
}

Builder& Builder::add(const char* key, const char* data, identity<const char*>){
	doc.AddMember(rapidjson::StringRef(key), rapidjson::Value(data,
				doc.GetAllocator()).Move(), doc.GetAllocator());

	return *this;
}

Builder& Builder::add(const char* key, std::string const list, identity<std::string>){
	return add(key, list.c_str());
}

Builder& Builder::add(const char* key, std::vector<std::string> const list, identity<std::vector<std::string>>){
	rapidjson::Value array(rapidjson::kArrayType);
	for(auto const& l : list){
		array.PushBack(
				rapidjson::Value{}.SetString(l.c_str(), l.length(), doc.GetAllocator()),
				doc.GetAllocator());
	}
	doc.AddMember(rapidjson::StringRef(key), array, doc.GetAllocator());

	return *this;
}

Builder& Builder::add(const char* key, std::vector<int> const data, identity<std::vector<int>>){
	rapidjson::Value array(rapidjson::kArrayType);
	for(auto const& d : data){
		array.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember(rapidjson::StringRef(key), array, doc.GetAllocator());

	return *this;
}


}//Message
