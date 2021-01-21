#ifndef TCP_CLIENT_CONCEPT_HPP__
#define TCP_CLIENT_CONCEPT_HPP__

#include <vector>
#include <boost/asio.hpp>
#include "byte_array.h"

using boost::asio::ip::tcp;

namespace My_Async{
namespace TCP{

class Client_Concept{
	private:
		struct Concept{
			virtual ~Concept(){}

			virtual void open(tcp::endpoint ep) noexcept = 0;

			virtual void close() noexcept = 0;

			virtual void write(std::shared_ptr<Byte_Array const> const& data) noexcept = 0;
			virtual void write(Byte_Array const data) noexcept = 0;

			virtual bool keep_alive() const noexcept = 0;
			virtual void keep_alive(bool enable) noexcept = 0;

			virtual void keep_alive(int32_t idle, int32_t count, int32_t interval) noexcept = 0;

			virtual tcp::endpoint endpoint() const = 0;
			virtual tcp::endpoint local_endpoint() const = 0;
			virtual bool use_ssl() const noexcept = 0;
		};

		template<typename Client>
		struct Model : Concept {
			Model(const std::shared_ptr<Client>& client) : client_(client){}
			virtual ~Model(){}

			void open(tcp::endpoint ep) noexcept override
			{
				client_->open(ep);
			}

			void close() noexcept override
			{
				client_->close();
			}

			void write(std::shared_ptr<Byte_Array const> const& data) noexcept override
			{
				client_->write(data);
			}

			void write(Byte_Array const data) noexcept override
			{
				client_->write(data);
			}

			bool keep_alive() const noexcept override
			{
				return client_->keep_alive();
			}

			void keep_alive(bool enable) noexcept override
			{
				client_->keep_alive(enable);
			}

			void keep_alive(int32_t idle, int32_t count, int32_t interval) noexcept override
			{
				client_->keep_alive(idle, count, interval);
			}

			tcp::endpoint endpoint() const override
			{
				return client_->endpoint();
			}

			tcp::endpoint local_endpoint() const override
			{
				return client_->local_endpoint();
			}

			bool use_ssl() const noexcept override
			{
				return client_->use_ssl;
			}

			private:
				std::shared_ptr<Client> client_;
		};

		std::shared_ptr<Concept> client_;
	public:
		template<typename Client>
		Client_Concept(const std::shared_ptr<Client>& client)
		: client_(std::make_shared<Model<Client>>(client)){}

		void open(tcp::endpoint ep) noexcept
		{
			client_->open(ep);
		}

		void close() noexcept
		{
			client_->close();
		}

		void write(std::shared_ptr<Byte_Array const> const& data) noexcept
		{
			client_->write(data);
		}

		void write(Byte_Array const data) noexcept
		{
			client_->write(data);
		}

		bool keep_alive() const noexcept
		{
			return client_->keep_alive();
		}

		void keep_alive(bool enable) noexcept
		{
			client_->keep_alive(enable);
		}

		void keep_alive(int32_t idle, int32_t count, int32_t interval) noexcept
		{
			client_->keep_alive(idle, count, interval);
		}

		tcp::endpoint endpoint() const
		{
			return client_->endpoint();
		}

		tcp::endpoint local_endpoint() const
		{
			return client_->local_endpoint();
		}

		bool use_ssl() const noexcept
		{
			return client_->use_ssl();
		}
};

}//TCP
}//My_Async

#endif /* TCP_CLIENT_CONCEPT_HPP__ */
