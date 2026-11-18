#include "IDGenerator.h"
#include <atomic>
#include <iomanip>
#include <sstream>

namespace Utils {
namespace {
std::atomic<unsigned long long> sequence{0};
}

std::string IDGenerator::generate(const std::string& prefix) {
    const auto serial = sequence.fetch_add(1, std::memory_order_relaxed) + 1;

    std::ostringstream result;
    result << prefix << std::setfill('0') << std::setw(3) << serial;
    return result.str();
}

std::string IDGenerator::generatePatientID() {
    return generate("P");
}

std::string IDGenerator::generateAppointmentID() {
    return generate("A");
}

void IDGenerator::seedSequence(unsigned long long val) {
    unsigned long long current = sequence.load();
    while (val > current && !sequence.compare_exchange_weak(current, val)) {
    }
}

} // namespace Utils