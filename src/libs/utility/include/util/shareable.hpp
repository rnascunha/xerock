#ifndef UTIL_SHAREABLE_HPP__
#define UTIL_SHAREABLE_HPP__

#include <functional>
#include <mutex>
#include <unordered_set>

#include "../types/traits.hpp"

struct empty{};

template<typename ShareClass, typename Data_Share = empty>
class Shareable
{
	public:
		Shareable() = default;

		void join(ShareClass* sh) noexcept{
			std::lock_guard<std::mutex> lock(mutex_);
			share_.insert(sh);
		}

		void leave(ShareClass* sh) noexcept{
			std::lock_guard<std::mutex> lock(mutex_);
			share_.erase(sh);
		}

		void exec_to_all(std::function<void(ShareClass*)> func) noexcept{
			if constexpr(my_traits::is_enable_shared_from_this<ShareClass>::value == false){
				std::lock_guard<std::mutex> lock(mutex_);
				for(auto s : share_){
					func(s);
				}
			} else {
				std::vector<std::weak_ptr<ShareClass>> v;
				{
					std::lock_guard<std::mutex> lock(mutex_);
					v.reserve(share_.size());
					for(auto p : share_)
						v.emplace_back(p->weak_from_this());
				}

				for(auto const& wp : v){
					if(auto sp = wp.lock()){
						func(sp.get());
					}
				}
			}
		}

		void exec_to_all(std::function<void(void)> func) noexcept{
			if constexpr(my_traits::is_enable_shared_from_this<ShareClass>::value == false){
				std::lock_guard<std::mutex> lock(mutex_);
				for(auto s : share_){
					s->func();
				}
			} else {
				std::vector<std::weak_ptr<ShareClass>> v;
				{
					std::lock_guard<std::mutex> lock(mutex_);
					v.reserve(share_.size());
					for(auto p : share_)
						v.emplace_back(p->weak_from_this());
				}

				for(auto const& wp : v){
					if(auto sp = wp.lock()){
						sp->func();
					}
				}
			}
		}

		unsigned int get_num_shares()
		{
			return share_.size();
		}

		Data_Share data;
	protected:

		std::mutex mutex_;
		std::unordered_set<ShareClass*> share_;
};

#endif /* UTIL_SHAREABLE_HPP__ */
