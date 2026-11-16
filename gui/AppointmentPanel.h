#pragma once

#include "../controller/AppointmentController.h"

#include <QWidget>

class QComboBox;
class QLineEdit;
class QTableWidget;

/** GUI for appointment scheduling, review, and cancellation. */
class AppointmentPanel : public QWidget {
    Q_OBJECT

public:
    explicit AppointmentPanel(Model::ClinicDatabase& database, QWidget* parent = nullptr);
    void refreshAppointments();

private slots:
    void scheduleAppointment();
    void cancelSelectedAppointment();

private:
    Model::ClinicDatabase& database;
    Controller::AppointmentController appointmentController;
    QComboBox* patientCombo;
    QLineEdit* dateInput;
    QLineEdit* timeInput;
    QLineEdit* reasonInput;
    QTableWidget* appointmentTable;

    void refreshPatientChoices();
    void showError(const QString& message);
};
