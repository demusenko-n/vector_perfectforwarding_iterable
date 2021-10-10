#pragma once
#include <exception>
#include <string>
namespace my_vector
{
	class my_vector_exception : public std::exception
	{
	public:
		my_vector_exception() = default;
		explicit my_vector_exception(std::string message) : exception() {}
	};
}