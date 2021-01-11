#include "../parser.hpp"
#include "../make.hpp"
#include "defines.hpp"

namespace Apps{
namespace Serial{

static std::string make_serial_status(std::shared_ptr<Container> serials);

void serial_send_status(std::shared_ptr<Container> serials)
{
	std::string msg = make_serial_status(serials);
	Core::Propagator::write_all(Byte_Array(msg));
}

void serial_send_status(std::shared_ptr<Core::Propagator>,
		std::shared_ptr<Container> serials)
{
	std::string msg = make_serial_status(serials);
	Core::Propagator::write_all(Byte_Array(msg));
}

Byte_Array serial_make_data(std::string const& str, Byte_Array const& data)
{
	rapidjson::Document doc;

	doc.SetObject();
	doc.AddMember("port", rapidjson::Value(str.c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());

	rapidjson::Value data_arr;
	data_arr.SetArray();
	for(auto const& d : data){
		data_arr.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember("data", data_arr, doc.GetAllocator());

	return Byte_Array(std::move(
						Message::Factory::create(Message::App::serial,
											doc)));
}

static std::string make_serial_status(std::shared_ptr<Container> serials)
{
	rapidjson::Document doc;

	doc.SetObject();

	rapidjson::Value ports_array(rapidjson::kArrayType);

	auto scanned = My_Async::Serial::scan_ports();

	for(auto const& p : scanned)
	{
		rapidjson::Document d(&doc.GetAllocator());
		d.SetObject();
		d.AddMember(SERIAL_CONFIG_PORT, rapidjson::Value(p.c_str(), d.GetAllocator()).Move(), d.GetAllocator());
		d.AddMember(SERIAL_STATUS_OPENED, serials->is_open(p), d.GetAllocator());
		if(serials->is_open(p))
		{
			unsigned int br;
			boost::asio::serial_port_base::character_size ch;
			boost::asio::serial_port_base::parity parity;
			boost::asio::serial_port_base::stop_bits stopbit;
			boost::asio::serial_port_base::flow_control fc;
			boost::system::error_code ec;

			serials->get_config(p, br, ch, parity, stopbit, fc, ec);
			if(!ec)
			{
				rapidjson::Document c(&doc.GetAllocator());
				c.SetObject();
				c.AddMember(SERIAL_CONFIG_BAUDRATE, br, doc.GetAllocator());
				c.AddMember(SERIAL_CONFIG_CHAR_SIZE, ch.value(), doc.GetAllocator());
				c.AddMember(SERIAL_CONFIG_PARITY, parity.value(), doc.GetAllocator());
				c.AddMember(SERIAL_CONFIG_STOPBIT, stopbit.value(), doc.GetAllocator());
				c.AddMember(SERIAL_CONFIG_FLOWCONTROL, fc.value(), doc.GetAllocator());

				d.AddMember(SERIAL_STATUS_CONFIG, c, doc.GetAllocator());
			}
		}
		ports_array.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember("ports", ports_array, doc.GetAllocator());

	return Message::Factory::create(Message::App::serial,
									Message::Control::status,
									doc);
}

}//Serial
}//Apps
