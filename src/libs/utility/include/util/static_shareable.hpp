#ifndef STATIC_SHAREABLE_HPP__
#define STATIC_SHAREABLE_HPP__

#include <functional>
#include <mutex>
#include <unordered_set>

#include "../types/traits.hpp"

template<typename ShareClass,
		bool AutoJoin = true>
class Static_Shareable{
	protected:
		static std::mutex mutex_;
		static std::unordered_set<ShareClass*> share_;

		static void join(ShareClass* sh) noexcept{
			std::lock_guard<std::mutex> lock(mutex_);
			share_.insert(sh);
		}

		static void leave(ShareClass* sh) noexcept{
			std::lock_guard<std::mutex> lock(mutex_);
			share_.erase(sh);
		}

		Static_Shareable(){
			if constexpr (AutoJoin)
				join(this);
		}

		~Static_Shareable(){
			if constexpr (AutoJoin)
				leave(this);
		}
	public:
		static void exec_to_all(std::function<void(ShareClass*)> func) noexcept{
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

		static void exec_to_all(std::function<void(void)> func) noexcept{
			if constexpr(my_traits::is_enable_shared_from_this<ShareClass>::value == false){
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

		static auto get_num_shares(){
			return share_.size();
		}
};

template<typename ShareClass, bool AutoJoin>
std::mutex Static_Shareable<ShareClass, AutoJoin>::mutex_;
template<typename ShareClass, bool AutoJoin>
std::unordered_set<ShareClass*> Static_Shareable<ShareClass, AutoJoin>::share_;

#endif /* STATIC_SHAREABLE_HPP__ */
