#include <iostream>
#include <fstream>
#include <dbcppp/Network.h> // The header you just read!

struct canFrame {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
};

int main() {

    // 1. Open the DBC file using a standard C++ file stream
    // You'll need to put a real .dbc file in your folder later
    std::ifstream dbc_file("my_car_data.dbc");

    if (!dbc_file.is_open()) {
        std::cerr << "Error: Could not open DBC file!" << std::endl;
        return 1;
    }

    // 2. Load the network using the function you found
    // LoadDBCFromIs returns a unique_ptr to an INetwork
    auto net = dbcppp::INetwork::LoadDBCFromIs(dbc_file);

    // 3. Use the other functions you found to print info
    std::cout << "Successfully loaded DBC!" << std::endl;
    std::cout << "Protocol Version: " << net->Version() << std::endl;
    std::cout << "Number of Messages defined: " << net->Messages_Size() << std::endl;

    std::ifstream log_file("log.bin", std::ios::binary);
    std::ofstream csv_out("output.csv");

    if (!log_file.is_open()) {
        std::cerr << "Error: Could not open log.bin!" << std::endl;
        return 1;
    }

    // Write a header for your CSV
    csv_out << "MessageName,SignalName,PhysicalValue,Unit" << std::endl;

    return 0;

}