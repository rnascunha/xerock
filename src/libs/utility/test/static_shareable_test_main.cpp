#include <iostream>
#include <functional>
#include "util/static_shareable.hpp"

#define USE_SHARED_POINTER

class Test :
#ifdef USE_SHARED_POINTER
		public std::enable_shared_from_this<Test>,
#endif
		public Static_Shareable<Test>{
	public:
		Test(const char* s) : s(s){}
		~Test(){}

		void print_m(){
			std::cout << s << " ";
		}

		void print_m2(const char* what){
			std::cout << s << " " << what << " ";
		}

	protected:
		const char* s;
};

void print(Test* t){
	t->print_m();
}

void print2(Test* t, const char* what){
	t->print_m2(what);
}

void print_shares_num(){
	std::cout << Test::get_num_shares() << ": ";
	Test::exec_to_all(&print);
	std::cout << "\n";
}

#ifdef USE_SHARED_POINTER
int main(){
	print_shares_num();
	auto t1 = std::make_shared<Test>("a");
	print_shares_num();
	{
		auto t2 = std::make_shared<Test>("b");
		print_shares_num();
		{
			auto t3 = std::make_shared<Test>("c");

			print_shares_num();
			std::cout << "Function (without bind): ";
			Test::exec_to_all(&print);
			std::cout << "\nFunction (with bind): ";
			Test::exec_to_all(std::bind(&print2, std::placeholders::_1, "/"));
			std::cout << "\nMember function: ";
			Test::exec_to_all(std::bind(&Test::print_m, std::placeholders::_1));
			std::cout << "\nMember function (with extra param): ";
			Test::exec_to_all(std::bind(&Test::print_m2, std::placeholders::_1, "\\"));
			std::cout << "\n";
		}
		print_shares_num();
	}
	print_shares_num();
}

#else

int main(){
	print_shares_num();
	Test t1("a");
	print_shares_num();
	{
		Test t2("b");
		print_shares_num();
		{
			Test t3("c");

			print_shares_num();
			std::cout << "Function (without bind): ";
			Test::exec_to_all(&print);
			std::cout << "\nFunction (with bind): ";
			Test::exec_to_all(std::bind(&print2, std::placeholders::_1, "/"));
			std::cout << "\nMember function: ";
			Test::exec_to_all(std::bind(&Test::print_m, std::placeholders::_1));
			std::cout << "\nMember function (with extra param): ";
			Test::exec_to_all(std::bind(&Test::print_m2, std::placeholders::_1, "\\"));
			std::cout << "\n";
		}
		print_shares_num();
	}
	print_shares_num();
}

#endif
