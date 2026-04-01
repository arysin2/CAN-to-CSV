#include <iostream>
#include <fstream>
#include <dbcppp/Network.h> 

struct __attribute__((packed)) canFrame { // 24 Bytes
    uint64_t timestamp; 
    uint32_t id;        
    uint8_t dlc;        
    uint8_t __pad[3];   
    uint8_t data[8];    
};

int main() {

    // Open DBC file
    std::ifstream dbc_file("car_data.dbc");

    if (!dbc_file.is_open()) {
        std::cerr << "Error: Could not open DBC file!" << std::endl;
        return 1;
    }

    // Load the network from library and LoadDBCFromIs returns a unique_ptr to an INetwork
    auto net = dbcppp::INetwork::LoadDBCFromIs(dbc_file);

    if (!net) {
        std::cerr << "Error: DBC failed to parse! Check syntax on line indicated above." << std::endl;
        return 1;
    }

    // Print information to Terminal
    std::cout << "Successfully loaded DBC!" << std::endl;
    std::cout << "Protocol Version: " << net->Version() << std::endl;
    std::cout << "Number of Messages defined: " << net->Messages_Size() << std::endl;

    std::ifstream log_file("log.bin", std::ios::binary);
    std::ofstream csv_out("output.csv");

    if (!log_file.is_open()) {
        std::cerr << "Error: Could not open log.bin!" << std::endl;
        return 1;
    }

    // CSV Header
    csv_out << "Timestamp,MessageName,SignalName,PhysicalValue,Unit" << std::endl;

    canFrame frame;

    // Read 24 Bytes and casts to treat bytes as struct data type 
    while (log_file.read(reinterpret_cast<char*>(&frame), sizeof(canFrame))) {
    std::cout << "Read Frame ID: " << frame.id << " (looking for 500)" << std::endl;
    // Go through each message in the DBC file
        for (const auto& msg : net->Messages()) {
            // Match CAN ID's
            if ((msg.Id() & 0x1FFFFFFF) == (frame.id & 0x1FFFFFFF)) {
            
            // Loop through the signals that match with the CAN ID
                for (const auto& sig : msg.Signals()) {
                    double phys_val;
                    // Hardcoded to determine whether positive or negativce
                    if (sig.Name() == "Current") {
                        int16_t raw_signed = (int16_t)((frame.data[3] << 8) | frame.data[2]);
                        // Integer to Physical Value
                        phys_val = raw_signed * 0.1; 
                    } else {
                        // Uses dbcpp library built in functions to calculate
                        double raw_val = sig.Decode(frame.data);
                        phys_val = sig.RawToPhys(raw_val);
                    }
                        // Print data to CSV
                        csv_out << frame.timestamp << "," 
                                << msg.Name() << "," 
                                << sig.Name() << "," 
                                << phys_val << "," 
                                << sig.Unit() << "\n";
                }
            }
        }
    }
    
    // End and close files
    log_file.close();
    csv_out.close();

    return 0;
}