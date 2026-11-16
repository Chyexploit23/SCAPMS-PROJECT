#include "AppointmentController.h"

#include "../utils/ClinicException.h"
#include "../utils/IDGenerator.h"

#include <cctype>

namespace Controller {
namespace {
bool allDigits(const std::string& value, std::size_t begin, std::size_t end) {
    if (begin >= end || end > value.size()) return false;
    for (std::size_t i = begin; i < end; ++i) {
        if (!std::isdigit(static_cast<unsigned char>(value[i]))) return false;
    }
    return true;
}

int numberAt(const std::string& value, std::size_t begin, std::size_t length) {
    return std::stoi(value.substr(begin, length));
}
}

AppointmentController::AppointmentController(Model::ClinicDatabase& clinicDatabase)
    : database(clinicDatabase) {}

std::string AppointmentController::scheduleAppointment(const std::string& patientID,
                                                        const std::string& date,
                                                        const std::string& time,
                                                        const std::string& reason) {
    if (patientID.empty() || !database.findPatientByID(patientID)) {
        throw Utils::ClinicException("Cannot schedule an appointment: patient ID was not found.");
    }
    if (!isValidDate(date)) {
        throw Utils::ClinicException("Appointment date must be a real date in YYYY-MM-DD format.");
    }
    if (!isValidTime(time)) {
        throw Utils::ClinicException("Appointment time must be in 24-hour HH:MM format.");
    }
    if (reason.find_first_not_of(" \t\r\n") == std::string::npos) {
        throw Utils::ClinicException("Appointment reason cannot be empty.");
    }

    const std::string appointmentID = Utils::IDGenerator::generateAppointmentID();
    database.addAppointment(Model::Appointment(appointmentID, patientID, date, time, reason, "Scheduled"));
    return appointmentID;
}

void AppointmentController::cancelAppointment(const std::string& appointmentID) {
    if (appointmentID.empty() || !findAppointmentByID(appointmentID)) {
        throw Utils::ClinicException("Appointment was not found.");
    }
    database.cancelAppointment(appointmentID);
}

const std::vector<Model::Appointment>& AppointmentController::getAppointments() const {
    return database.getAppointments();
}

const Model::Appointment* AppointmentController::findAppointmentByID(const std::string& appointmentID) const {
    const auto& appointments = database.getAppointments();
    for (const Model::Appointment& appointment : appointments) {
        if (appointment.getAppointmentID() == appointmentID) return &appointment;
    }
    return nullptr;
}

bool AppointmentController::isValidDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-' ||
        !allDigits(date, 0, 4) || !allDigits(date, 5, 7) || !allDigits(date, 8, 10)) {
        return false;
    }
    const int year = numberAt(date, 0, 4);
    const int month = numberAt(date, 5, 2);
    const int day = numberAt(date, 8, 2);
    if (year < 1 || month < 1 || month > 12) return false;

    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maximumDay = daysInMonth[month - 1];
    const bool leapYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    if (month == 2 && leapYear) maximumDay = 29;
    return day >= 1 && day <= maximumDay;
}

bool AppointmentController::isValidTime(const std::string& time) {
    if (time.size() != 5 || time[2] != ':' || !allDigits(time, 0, 2) || !allDigits(time, 3, 5)) {
        return false;
    }
    const int hour = numberAt(time, 0, 2);
    const int minute = numberAt(time, 3, 2);
    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

} // namespace Controller
