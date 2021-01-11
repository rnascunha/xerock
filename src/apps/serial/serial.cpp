#include "serial.hpp"
#include "parser.hpp"
#include "make.hpp"

#include "core/message.hpp"
#include "core/helpers.hpp"
#include "core/error.hpp"

#include "my_async/serial/serial_util.hpp"

namespace Apps{
namespace Serial{

static void serial_message(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv& doc,
		std::shared_ptr<Container> serials);
static void serial_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv& doc,
		std::shared_ptr<Container> serials);
static void serial_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv& doc,
						std::shared_ptr<Container> serials);

void serial_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data& main, Message::Type_Recv& doc,
		std::shared_ptr<Container> serials)
{
	if(main.type == Message::Type::control){
		//control
		switch(main.control){
			case Message::Control::open:
				//Open command
				serial_open(ws, doc, serials);
				break;
			case Message::Control::close:
				//Close command
				serial_close(ws, doc, serials);
				break;
			case Message::Control::error:
				return;
			case Message::Control::status:
				break;
			default:
				return;
		}
		serial_send_status(ws, serials);
	} else {
		//data
		serial_message(ws, doc, serials);
	}
}

/**
 *
 *
 *
 *
 *
 */

static void serial_message(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv& doc,
		std::shared_ptr<Container> serials)
{
	std::string port;
	Byte_Array data;
	if(!parser_serial_data(doc, port, data))
	{
		Core::fail_message(ws, Message::App::serial, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	boost::system::error_code ec;
	serials->write(port, std::move(data), ec);
	if(ec)
		Core::fail_message(ws,  Message::App::serial, ec);
}

static void serial_config(std::shared_ptr<Core::Propagator> ws,
		std::shared_ptr<Container> serials,
		Serial_Config& config)
{
	boost::system::error_code ec;
	serials->config(
			config.port,
			config.baud,
			config.char_size,
			boost::asio::serial_port_base::parity{config.parity},
			boost::asio::serial_port_base::stop_bits{config.stopbits},
			boost::asio::serial_port_base::flow_control{config.fc},
			ec);

	if(ec){
		Core::fail_message(ws, Message::App::serial, ec);
		return;
	}
}

static void serial_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv& doc,
		std::shared_ptr<Container> serials)
{
	Serial_Config config;
	if(!parser_serial_config(doc, config))
		return Core::fail_message(ws, Message::App::serial,
				make_error_code(Message_Errc::ILL_FORMED));

	if(serials->is_open(config.port))
		return Core::fail_message(ws,  Message::App::serial,
				make_error_code(boost::system::errc::already_connected));

	boost::system::error_code ec;
	serials->open(config.port, config.baud, ec, [serials](boost::asio::io_context& ioc){
		auto serial = My_Async::Serial::make_serial<Serial>(ioc);
		serial->set_close_callback([serials, serial]() {
			boost::system::error_code ec;
			serials->close(serial->get_port_name(), ec);
			serial_send_status(serials);
		});

		return serial;
	});
	if(ec)
		return Core::fail_message(ws, Message::App::serial, ec);


	serial_config(ws, serials, config);
}

static void serial_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv& doc,
						std::shared_ptr<Container> serials)
{
	std::string port;
	if(!parser_serial_close(doc, port))
		return Core::fail_message(ws, Message::App::serial,
				make_error_code(Message_Errc::ILL_FORMED));

	boost::system::error_code ec;
	serials->close(port, ec);
	if(ec)
		return Core::fail_message(ws, Message::App::serial,
				make_error_code(std::errc::no_such_device_or_address));
}

}//Serial
}//Apps


