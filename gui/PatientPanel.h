#pragma once

#include "../controller/ClinicManager.h"

#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QTableWidget;

/**
 * Patient-record CRUD interface. The panel delegates storage and waiting-queue
 * ownership to ClinicManager and keeps presentation/validation in the GUI layer.
 */
class PatientPanel : public QWidget {
    Q_OBJECT

public:
    explicit PatientPanel(Controller::ClinicManager& manager, QWidget* parent = nullptr);

    void refreshTable();

private slots:
    void addPatient();
    void updateSelectedPatient();
    void deleteSelectedPatient();
    void loadSelectedPatient();

private:
    Controller::ClinicManager& clinicManager;
    QLineEdit* idInput;
    QLineEdit* nameInput;
    QLineEdit* ageInput;
    QLineEdit* contactInput;
    QComboBox* typeCombo;
    QTableWidget* patientTable;
    QPushButton* addButton;
    QPushButton* updateButton;

    Model::PatientRecord* selectedPatient() const;
    void clearForm(bool generateNextID);
    void showError(const QString& message);
};
