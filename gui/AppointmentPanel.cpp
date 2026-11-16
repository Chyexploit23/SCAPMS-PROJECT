#include "AppointmentPanel.h"

#include "../utils/ClinicException.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QColor>

#include <exception>

AppointmentPanel::AppointmentPanel(Model::ClinicDatabase& clinicDatabase, QWidget* parent)
    : QWidget(parent), database(clinicDatabase), appointmentController(clinicDatabase),
      patientCombo(new QComboBox(this)), dateInput(new QLineEdit(this)),
      timeInput(new QLineEdit(this)), reasonInput(new QLineEdit(this)),
      appointmentTable(new QTableWidget(this)) {
    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();

    patientCombo->setToolTip(tr("Choose an existing patient for this appointment."));
    dateInput->setPlaceholderText(tr("YYYY-MM-DD"));
    dateInput->setToolTip(tr("Enter a valid calendar date, for example 2026-12-31."));
    timeInput->setPlaceholderText(tr("HH:MM (24-hour)"));
    timeInput->setToolTip(tr("Enter a time in 24-hour format, for example 09:30."));
    reasonInput->setPlaceholderText(tr("Reason for visit"));

    form->addRow(tr("Patient:"), patientCombo);
    form->addRow(tr("Date:"), dateInput);
    form->addRow(tr("Time:"), timeInput);
    form->addRow(tr("Reason:"), reasonInput);

    auto* actions = new QHBoxLayout();
    auto* scheduleButton = new QPushButton(tr("Schedule Appointment"), this);
    auto* cancelButton = new QPushButton(tr("Cancel Selected"), this);
    auto* refreshButton = new QPushButton(tr("Refresh"), this);
    scheduleButton->setToolTip(tr("Validate and schedule an appointment for the selected patient."));
    cancelButton->setToolTip(tr("Cancel the selected appointment if it is still scheduled."));
    actions->addWidget(scheduleButton);
    actions->addWidget(cancelButton);
    actions->addWidget(refreshButton);
    actions->addStretch();

    appointmentTable->setColumnCount(7);
    appointmentTable->setHorizontalHeaderLabels({tr("Appointment ID"), tr("Patient ID"), tr("Patient Name"), tr("Date"), tr("Time"), tr("Reason"), tr("Status")});
    appointmentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    appointmentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    appointmentTable->setSelectionMode(QAbstractItemView::SingleSelection);
    appointmentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    root->addWidget(new QLabel(tr("Schedule and manage clinic appointments"), this));
    root->addLayout(form);
    root->addLayout(actions);
    root->addWidget(appointmentTable, 1);

    connect(scheduleButton, &QPushButton::clicked, this, &AppointmentPanel::scheduleAppointment);
    connect(cancelButton, &QPushButton::clicked, this, &AppointmentPanel::cancelSelectedAppointment);
    connect(refreshButton, &QPushButton::clicked, this, &AppointmentPanel::refreshAppointments);

    refreshAppointments();
}

void AppointmentPanel::scheduleAppointment() {
    if (patientCombo->currentIndex() < 0) {
        showError(tr("Register a patient before scheduling an appointment."));
        return;
    }

    const std::string patientID = patientCombo->currentData().toString().toStdString();
    const std::string date = dateInput->text().trimmed().toStdString();
    const std::string time = timeInput->text().trimmed().toStdString();
    const std::string reason = reasonInput->text().trimmed().toStdString();

    try {
        const std::string appointmentID = appointmentController.scheduleAppointment(patientID, date, time, reason);
        refreshAppointments();
        dateInput->clear();
        timeInput->clear();
        reasonInput->clear();
        QMessageBox::information(this, tr("Appointment scheduled"),
                                 tr("Appointment %1 has been scheduled.").arg(QString::fromStdString(appointmentID)));
    } catch (const Utils::ClinicException& error) {
        showError(QString::fromUtf8(error.what()));
    } catch (const std::exception& error) {
        showError(tr("Could not schedule appointment: %1").arg(QString::fromUtf8(error.what())));
    }
}

void AppointmentPanel::cancelSelectedAppointment() {
    const int row = appointmentTable->currentRow();
    if (row < 0 || !appointmentTable->item(row, 0)) {
        showError(tr("Select an appointment from the table first."));
        return;
    }
    const QString appointmentID = appointmentTable->item(row, 0)->text();
    const QString status = appointmentTable->item(row, 6)->text();
    if (status.compare(QStringLiteral("Scheduled"), Qt::CaseInsensitive) != 0) {
        showError(tr("Only scheduled appointments can be cancelled."));
        return;
    }
    if (QMessageBox::question(this, tr("Cancel appointment"),
                              tr("Cancel appointment %1?").arg(appointmentID)) != QMessageBox::Yes) {
        return;
    }

    try {
        appointmentController.cancelAppointment(appointmentID.toStdString());
        refreshAppointments();
    } catch (const Utils::ClinicException& error) {
        showError(QString::fromUtf8(error.what()));
    } catch (const std::exception& error) {
        showError(tr("Could not cancel appointment: %1").arg(QString::fromUtf8(error.what())));
    }
}

void AppointmentPanel::refreshAppointments() {
    refreshPatientChoices();
    appointmentTable->setRowCount(0);
    for (const Model::Appointment& appointment : appointmentController.getAppointments()) {
        const int row = appointmentTable->rowCount();
        appointmentTable->insertRow(row);
        Model::PatientRecord* patient = database.findPatientByID(appointment.getPatientID());
        const QString values[] = {
            QString::fromStdString(appointment.getAppointmentID()),
            QString::fromStdString(appointment.getPatientID()),
            patient ? QString::fromStdString(patient->getName()) : tr("Patient record unavailable"),
            QString::fromStdString(appointment.getAppointmentDate()),
            QString::fromStdString(appointment.getAppointmentTime()),
            QString::fromStdString(appointment.getReason()),
            QString::fromStdString(appointment.getStatus())
        };
        for (int column = 0; column < 7; ++column) {
            auto* item = new QTableWidgetItem(values[column]);
            if (appointment.getStatus() == "Cancelled") {
                item->setBackground(QColor(235, 235, 235));
            } else if (appointment.getStatus() == "Scheduled") {
                item->setBackground(QColor(225, 245, 230));
            }
            appointmentTable->setItem(row, column, item);
        }
    }
}

void AppointmentPanel::refreshPatientChoices() {
    const QString previousID = patientCombo->currentData().toString();
    patientCombo->clear();
    for (const Model::PatientRecord* patient : database.getPatients()) {
        if (!patient) continue;
        const QString id = QString::fromStdString(patient->getPatientID());
        const QString label = QStringLiteral("%1 — %2").arg(id, QString::fromStdString(patient->getName()));
        patientCombo->addItem(label, id);
    }
    const int previousIndex = patientCombo->findData(previousID);
    if (previousIndex >= 0) patientCombo->setCurrentIndex(previousIndex);
}

void AppointmentPanel::showError(const QString& message) {
    QMessageBox::warning(this, tr("Appointment"), message);
}
