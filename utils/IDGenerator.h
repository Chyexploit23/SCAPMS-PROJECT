#pragma once

#include <string>

namespace Utils {

/** Thread-safe, process-unique identifier generation for SCAMPS records. */
class IDGenerator {
public:
    static std::string generate(const std::string& prefix);
    static std::string generatePatientID();
    static std::string generateAppointmentID();

    static void seedSequence(unsigned long long val);
};

} // namespace Utils
