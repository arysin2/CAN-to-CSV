#include <iostream>
#include <fstream>
#include <dbcppp/Network.h> // The header you just read!

struct __attribute__((packed)) canFrame {
    uint64_t timestamp; 
    uint32_t id;        
    uint8_t dlc;        
    uint8_t __pad[3];   // Add this to match the printf above
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

    // ADD THIS CHECK IMMEDIATELY:
    if (!net) {
        std::cerr << "Error: DBC failed to parse! Check syntax on line indicated above." << std::endl;
        return 1;
    }
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
    csv_out << "Timestamp,MessageName,SignalName,PhysicalValue,Unit" << std::endl;

    canFrame frame;

// Read until the end of the file
    while (log_file.read(reinterpret_cast<char*>(&frame), sizeof(canFrame))) {
    std::cout << "Read Frame ID: " << frame.id << " (looking for 500)" << std::endl;
    // Look for a message in the DBC that matches the ID we just read
        for (const auto& msg : net->Messages()) {
            if ((msg.Id() & 0x1FFFFFFF) == (frame.id & 0x1FFFFFFF)) {
            
            // We found a match! Now loop through every signal in that message
                for (const auto& sig : msg.Signals()) {
    double phys_val;

    if (sig.Name() == "Current") {
        // MANUALLY combine bytes 2 and 3 into a signed 16-bit integer
        // frame.data[2] is the Low Byte, frame.data[3] is the High Byte
        int16_t raw_signed = (int16_t)((frame.data[3] << 8) | frame.data[2]);
        
        // Apply your DBC factor (0.1) manually
        phys_val = raw_signed * 0.1; 
        
        std::cout << "Manual Current Decode: " << phys_val << " (Raw: " << raw_signed << ")" << std::endl;
    } else {
        // For Voltage (Unsigned), the library is working perfectly
        double raw_val = sig.Decode(frame.data);
        phys_val = sig.RawToPhys(raw_val);
    }

    csv_out << frame.timestamp << "," 
            << msg.Name() << "," 
            << sig.Name() << "," 
            << phys_val << "," 
            << sig.Unit() << "\n";
}
            }
        }
    }

    log_file.close();
    csv_out.close();

    return 0;

}