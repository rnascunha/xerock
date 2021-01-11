#ifndef SESSION_LISTENER_TCP_CONCEPT_HPP__
#define SESSION_LISTENER_TCP_CONCEPT_HPP__

#include <vector>
#include <boost/asio.hpp>
#include "byte_array.h"

/**
 * Este conceito só funciona para o metodo write com Byte_Array como Argumento. O ideal seria generalizar para
 * qualquer tipo
 */

namespace My_Async{
namespace Listener{

class Listener_Concept{
	private:
		struct Concept{
			virtual ~Concept(){}

			virtual std::vector<boost::asio::ip::tcp::endpoint>
			clients() const noexcept = 0;

			virtual void
			close(boost::system::error_code&) noexcept = 0;

			virtual void
			close(boost::system::error_code& ec, bool close_clients) noexcept = 0;

			virtual void
			close_clients(std::vector<boost::asio::ip::tcp::endpoint> eps) = 0;

			virtual void
			write(boost::asio::ip::tcp::endpoint client, std::shared_ptr<Byte_Array const> const& data) = 0;

			virtual void
			init(boost::asio::ip::tcp::endpoint endpoint, boost::system::error_code& ec) noexcept = 0;

			virtual bool
			use_ssl() const noexcept = 0;
		};

		template<typename Listener>
		struct Model : Concept {
			Model(const std::shared_ptr<Listener>& listener) : listener_(listener){}
			virtual ~Model(){}

			std::vector<boost::asio::ip::tcp::endpoint>
			clients() const noexcept override
			{
				return listener_->clients();
			}

			void
			close(boost::system::error_code& ec) noexcept override
			{
				listener_->close(ec);
			}

			void
			close(boost::system::error_code& ec, bool close_clients) noexcept override
			{
				listener_->close(ec, close_clients);
			}

			void
			close_clients(std::vector<boost::asio::ip::tcp::endpoint> eps) override
			{
				listener_->close_clients(eps);
			}

			void
			write(boost::asio::ip::tcp::endpoint client,
					std::shared_ptr<Byte_Array const> const& data)
			{
				listener_->write(client, data);
			}

			void
			init(boost::asio::ip::tcp::endpoint endpoint,
					boost::system::error_code& ec) noexcept override
			{
				listener_->init(endpoint, ec);
			}

			bool use_ssl() const noexcept override
			{
				return listener_->use_ssl;
			}

			private:
				std::shared_ptr<Listener> listener_;
		};

		std::shared_ptr<Concept> listener_;
	public:
		template<typename Listener>
		Listener_Concept(const std::shared_ptr<Listener>& listener)
		: listener_(std::make_shared<Model<Listener>>(listener)){}

		std::vector<boost::asio::ip::tcp::endpoint>
		clients() const noexcept{
			return listener_->clients();
		}

		void
		close(boost::system::error_code& ec) noexcept{
			listener_->close(ec);
		}

		void
		close(boost::system::error_code& ec, bool close_clients) noexcept{
			listener_->close(ec, close_clients);
		}

		void
		close_clients(std::vector<boost::asio::ip::tcp::endpoint> eps){
			listener_->close_clients(eps);
		}

		template<typename OutContainer>
		void
		write(boost::asio::ip::tcp::endpoint client, std::shared_ptr<OutContainer const> const& data){
			listener_->write(client, data);
		}

		void
		init(boost::asio::ip::tcp::endpoint endpoint, boost::system::error_code& ec) noexcept{
			listener_->init(endpoint, ec);
		}

		bool use_ssl() const noexcept{
			return listener_->use_ssl();
		}
};

}//Listener
}//My_Async

#endif /* SESSION_LISTENER_TCP_CONCEPT_HPP__ */
