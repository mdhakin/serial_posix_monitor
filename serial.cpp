#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <cstdlib>


void receive_loop();
void send_loop();
void cmdLoop();

std::atomic<bool> recieve_loop_running(true);
std::atomic<bool> send_loop_running(true);
std::atomic<bool> cmd_loop_running(true);

std::string cmd = "";
std::mutex cmd_mutex;

const char* Cport;
unsigned int baudRate = 19200;
int main(int argc, char* argv[]) {
    
    //port = argv[1];
    std::string sPort(argv[1]);
    Cport = sPort.c_str();


    char* myCharPtr = argv[2];
    char* end;
    unsigned int myUint = std::strtoul(myCharPtr, &end, 10);
    baudRate = myUint;

    std::cout << "port::" << Cport << std::endl;
    std::cout << "baud rate::" << baudRate << std::endl;


    std::thread rec_thread(receive_loop);
    std::thread snd_thread(send_loop);
    std::thread cli_thread(cmdLoop);

    rec_thread.join();
    snd_thread.join();
    cli_thread.join();

    
    return 0;
}


void receive_loop()
{
    
    // Step 1. Assuming that the client application has already
    // obtained the I/O service object, in this case represented 
    // as an object of the io_service class.
    boost::asio::io_service io_service;
    // Step 2. Create and open a serial port object.
    //boost::asio::serial_port port(io_service, "/dev/ttyUSB0");
    boost::asio::serial_port port(io_service, Cport);

    // Step 3. Set options for the serial port.
    port.set_option(boost::asio::serial_port_base::baud_rate(baudRate));


    while (recieve_loop_running)
    {
        boost::asio::streambuf buffer;
        boost::asio::read_until(port, buffer, '\n');
        // Step 6. Extract the data from the streambuf to a std::string.
        std::string input = boost::asio::buffer_cast<const char*>(buffer.data());
        std::cout << input;// << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // wait for 1 second before sending next message
    }
}

void send_loop()
{
    // Step 1. Assuming that the client application has already
    // obtained the I/O service object, in this case represented 
    // as an object of the io_service class.
    boost::asio::io_service io_service;

    // Step 2. Create and open a serial port object.
    //boost::asio::serial_port port(io_service, "/dev/ttyUSB0");
    boost::asio::serial_port port(io_service, Cport);

    // Step 3. Set options for the serial port.
    port.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
    
    // Main loop
    while (send_loop_running)
    {
        std::lock_guard<std::mutex> lock(cmd_mutex);
        if (cmd != "")
        {
            std::string output = cmd + '\n';
            cmd = "";
            boost::asio::write(port, boost::asio::buffer(output));
            std::this_thread::sleep_for(std::chrono::milliseconds(300)); // wait for 1 second before sending next message
        }
    }
}



void cmdLoop()
{
    std::string command;

    while (cmd_loop_running)
    {
        std::getline(std::cin, command);
        //std::cout << command << std::endl;

        if (command == "quit")
        {
            std::cout << "quit" << std::endl;
            std::lock_guard<std::mutex> lock(cmd_mutex);
            cmd = command;
            recieve_loop_running = false;
            send_loop_running = false;
            cmd_loop_running = false;
        }else
        {
            std::lock_guard<std::mutex> lock(cmd_mutex);
            cmd = command;
        }

    }
}