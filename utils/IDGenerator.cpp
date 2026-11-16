#include "IDGenerator.h"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Utils {
namespace {
std::atomic<unsigned long long> sequence{0};
}

std::string IDGenerator::generate(const std::string& prefix) {
    const auto now = std::chrono::system_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    const auto serial = sequence.fetch_add(1, std::memory_order_relaxed) + 1;

    std::ostringstream result;
    result << prefix << '-' << milliseconds << '-' << std::setfill('0') << std::setw(4) << serial;
    return result.str();
}

std::string IDGenerator::generatePatientID() {
    return generate("PAT");
}

std::string IDGenerator::generateAppointmentID() {
    return generate("APT");
}

} // namespace Utils
