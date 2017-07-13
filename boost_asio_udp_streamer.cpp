// It's the very simple udp streaming server
// by shhyun, xjavalov@shhyun.com
// 
// 1. just make fake 3-axis data
// 2. streaming some host/port.
// 

#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <math.h>

using boost::asio::ip::udp;

#include <iostream>
#include <ctime>
#include <chrono>

class udp_server
{
public:
	udp_server(boost::asio::io_service& io_service, const std::string& host, const std::string& port)
		: io_service_(io_service), _socket(io_service, udp::endpoint(udp::v4(), 0)) 
	{
		udp::resolver resolver(io_service_);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		_endpoint = *iter;
	}

	void start_send()
	{
		for (;;)
		{
			std::string axis_data = get_axis_data();
			char data[max_length];
			strncpy_s(data, axis_data.c_str(), sizeof(data));
			data[sizeof(data) - 1] = 0;

			_socket.send_to(boost::asio::buffer(data), _endpoint);
			Sleep(100);
		}
	}
	
private:
	boost::asio::io_service& io_service_;
	udp::socket _socket;
	udp::endpoint _endpoint;


	std::string get_axis_data()
	{
		using namespace std::chrono;
		__int64 ms = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		double degree = d2r(static_cast<double>(ms) / 100.0);

		std::string return_value = boost::str(boost::format{ "%|f|+10,%|f|+10,%|f|-10" } % sin(degree) % cos(degree) % tan(degree));
		std::cout << return_value << std::endl;

		return return_value;
	}

	double d2r(double d) {
		return (d / 180.0) * (3.141592);
	}

	enum {
		max_length = 1024,
	};
};


int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		udp_server server(io_service, "192.168.1.190","15");
		server.start_send();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

}
