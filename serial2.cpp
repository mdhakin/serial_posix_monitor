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

void receive_loop2();
void send_loop2();
void cmdLoop2();


std::atomic<bool> recieve_loop_running(true);
std::atomic<bool> send_loop_running(true);
std::atomic<bool> cmd_loop_running(true);

std::atomic<bool> recieve_loop_running2(true);
std::atomic<bool> send_loop_running2(true);


std::string cmd = "";
std::mutex cmd_mutex;

std::string cmd2 = "";
std::mutex cmd_mutex2;

const char* Cport;
const char* Cport2;

unsigned int baudRate = 19200;
unsigned int baudRate2 = 19200;
int main(int argc, char* argv[]) {
    
    //port = argv[1];
    std::string sPort(argv[1]);
    Cport = sPort.c_str();

    std::string sPort2(argv[3]);
    Cport2 = sPort2.c_str();

    char* myCharPtr = argv[2];
    char* myCharPtr2 = argv[4];


    char* end;
    unsigned int myUint = std::strtoul(myCharPtr, &end, 10);
    baudRate = myUint;

    unsigned int myUint2 = std::strtoul(myCharPtr2, &end, 10);
    baudRate2 = myUint2;

    std::cout << "port::" << Cport << std::endl;
    std::cout << "baud rate::" << baudRate << std::endl;
    std::cout << "port2::" << Cport2 << std::endl;
    std::cout << "baud rate2::" << baudRate2 << std::endl;



    std::thread rec_thread(receive_loop);
    std::thread snd_thread(send_loop);

    std::thread rec_thread2(receive_loop2);
    std::thread snd_thread2(send_loop2);

    std::thread cli_thread(cmdLoop);

    rec_thread.join();
    snd_thread.join();
    rec_thread2.join();
    snd_thread2.join();
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
void receive_loop2()
{
    
    // Step 1. Assuming that the client application has already
    // obtained the I/O service object, in this case represented 
    // as an object of the io_service class.
    boost::asio::io_service io_service;
    // Step 2. Create and open a serial port object.
    //boost::asio::serial_port port(io_service, "/dev/ttyUSB0");
    boost::asio::serial_port port(io_service, Cport2);

    // Step 3. Set options for the serial port.
    port.set_option(boost::asio::serial_port_base::baud_rate(baudRate2));


    while (recieve_loop_running2)
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

void send_loop2()
{
    // Step 1. Assuming that the client application has already
    // obtained the I/O service object, in this case represented 
    // as an object of the io_service class.
    boost::asio::io_service io_service;

    // Step 2. Create and open a serial port object.
    //boost::asio::serial_port port(io_service, "/dev/ttyUSB0");
    boost::asio::serial_port port(io_service, Cport2);

    // Step 3. Set options for the serial port.
    port.set_option(boost::asio::serial_port_base::baud_rate(baudRate2));
    
    // Main loop
    while (send_loop_running2)
    {
        std::lock_guard<std::mutex> lock(cmd_mutex2);
        if (cmd2 != "")
        {
            std::string output = cmd2 + '\n';
            cmd2 = "";
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
            recieve_loop_running2 = false;
            send_loop_running2 = false;
            cmd_loop_running = false;
        }else
        {
           
                std::lock_guard<std::mutex> lock(cmd_mutex);
                cmd = command;

                std::lock_guard<std::mutex> lock2(cmd_mutex2);
                cmd2 = command;
            
            
        }

    }
}