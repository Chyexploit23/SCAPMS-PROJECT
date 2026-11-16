#pragma once

#include "../model/Appointment.h"
#include "../model/ClinicDatabase.h"

#include <string>

namespace Controller {

/** Business operations for creating, listing, and cancelling appointments. */
class AppointmentController {
public:
    explicit AppointmentController(Model::ClinicDatabase& database);

    // Creates a scheduled appointment and returns its generated appointment ID.
    std::string scheduleAppointment(const std::string& patientID,
                                    const std::string& date,
                                    const std::string& time,
                                    const std::string& reason);
    void cancelAppointment(const std::string& appointmentID);
    const std::vector<Model::Appointment>& getAppointments() const;
    const Model::Appointment* findAppointmentByID(const std::string& appointmentID) const;

private:
    Model::ClinicDatabase& database;

    static bool isValidDate(const std::string& date);
    static bool isValidTime(const std::string& time);
};

} // namespace Controller
