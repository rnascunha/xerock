#include <iostream>
#include <functional>
#include <memory>

#include "util/shareable.hpp"

#define USE_SHARED_POINTER

class Test
#ifdef USE_SHARED_POINTER
		: public std::enable_shared_from_this<Test>
#endif /* USE_SHARED_POINTER */
{
	public:
		Test(const char* s, std::shared_ptr<Shareable<Test>> share)
			: s(s), share_(share){
			share_->join(this);
		}
		~Test(){
			share_->leave(this);
		}

		void print_size(){
			std::cout << share_->get_num_shares() << ": ";
		}

		void print_m(){
			std::cout << s << " ";
		}

		void print_m2(const char* what){
			std::cout << s << " " << what << " ";
		}

		void exec(std::function<void(Test*)> func){
			share_->exec_to_all(func);
		}
	protected:
		const char* s;
		std::shared_ptr<Shareable<Test>> share_;
};

void print(Test* t){
	t->print_m();
}

void print2(Test* t, const char* what){
	t->print_m2(what);
}

#ifdef USE_SHARED_POINTER
void print_shares_num(std::shared_ptr<Test> t){
	t->print_size();
	t->exec(&print);
	std::cout << "\n";
}

int main(){
	auto share = std::make_shared<Shareable<Test>>();
	auto t1 = std::make_shared<Test>("a", share);
	print_shares_num(t1);

	{
		auto t2 = std::make_shared<Test>("b", share);

		{
			auto t3 = std::make_shared<Test>("c", share);

			print_shares_num(t1);
			std::cout << "Function (without bind): ";
			t1->exec(&print);
			std::cout << "\nFunction (with bind): ";
			t2->exec(std::bind(&print2, std::placeholders::_1, "/"));
			std::cout << "\nMember function: ";
			t3->exec(std::bind(&Test::print_m, std::placeholders::_1));
			std::cout << "\nMember function (with extra param): ";
			t1->exec(std::bind(&Test::print_m2, std::placeholders::_1, "\\"));
			std::cout << "\n";
		}

		print_shares_num(t1);
	}

	print_shares_num(t1);
}

#else

void print_shares_num(Test& t){
	t.print_size();
	t.exec(&print);
	std::cout << "\n";
}

int main(){
	auto share = std::make_shared<Shareable<Test>>();
	Test t1("a", share);
	print_shares_num(t1);
	{
		Test t2("b", share);
		print_shares_num(t1);
		{
			Test t3("c", share);

			print_shares_num(t1);
			std::cout << "Function (without bind): ";
			t1.exec(&print);
			std::cout << "\nFunction (with bind): ";
			t2.exec(std::bind(&print2, std::placeholders::_1, "/"));
			std::cout << "\nMember function: ";
			t3.exec(std::bind(&Test::print_m, std::placeholders::_1));
			std::cout << "\nMember function (with extra param): ";
			t1.exec(std::bind(&Test::print_m2, std::placeholders::_1, "\\"));
			std::cout << "\n";
		}
		print_shares_num(t1);
	}
	print_shares_num(t1);
}


#endif
