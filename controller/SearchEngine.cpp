#include "SearchEngine.h"
#include <algorithm>
#include <cctype>

namespace Controller {

// Helper function to convert std::string to uppercase for case-insensitive matching
static std::string toUpperStr(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

Model::PatientRecord* SearchEngine::linearSearchByID(const std::vector<Model::PatientRecord*>& patients, const std::string& targetID) {
    std::string upperTarget = toUpperStr(targetID);

    for (auto p : patients) {
        if (!p) continue;
        if (toUpperStr(p->getPatientID()) == upperTarget) return p;
    }
    return nullptr;
}

Model::PatientRecord* SearchEngine::binarySearchByID(std::vector<Model::PatientRecord*> patients, const std::string& targetID) {
    std::string upperTarget = toUpperStr(targetID);

    // Binary search must not change the user-visible ordering of the database.
    for (size_t i = 1; i < patients.size(); ++i) {
        Model::PatientRecord *key = patients[i];
        size_t j = i;
        while (j > 0 && toUpperStr(patients[j - 1]->getPatientID()) > toUpperStr(key->getPatientID())) {
            patients[j] = patients[j - 1];
            --j;
        }
        patients[j] = key;
    }

    int low = 0, high = static_cast<int>(patients.size()) - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (!patients[mid]) { low = mid + 1; continue; }

        std::string midId = toUpperStr(patients[mid]->getPatientID());

        if (midId == upperTarget) return patients[mid];
        if (midId < upperTarget) low = mid + 1;
        else high = mid - 1;
    }
    return nullptr;
}

Model::PatientRecord* SearchEngine::recursiveSearchByName(const std::vector<Model::PatientRecord*>& patients, const std::string& targetName, size_t index) {
    if (index >= patients.size()) return nullptr;
    if (!patients[index]) return recursiveSearchByName(patients, targetName, index + 1);

    // Convert both the stored name and target name to uppercase for case-insensitive comparison
    if (toUpperStr(patients[index]->getName()) == toUpperStr(targetName)) {
        return patients[index];
    }
    
    return recursiveSearchByName(patients, targetName, index + 1);
}

void SearchEngine::selectionSortByName(std::vector<Model::PatientRecord*>& patients) {
    size_t n = patients.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        size_t minIdx = i;
        for (size_t j = i + 1; j < n; ++j) {
            if (toUpperStr(patients[j]->getName()) < toUpperStr(patients[minIdx]->getName())) {
                minIdx = j;
            }
        }
        std::swap(patients[i], patients[minIdx]);
    }
}

void SearchEngine::insertionSortByAge(std::vector<Model::PatientRecord*>& patients) {
    size_t n = patients.size();
    for (size_t i = 1; i < n; ++i) {
        Model::PatientRecord* key = patients[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && patients[j]->getAge() > key->getAge()) {
            patients[j + 1] = patients[j];
            j--;
        }
        patients[j + 1] = key;
    }
}

void SearchEngine::quickSortByID(std::vector<Model::PatientRecord*>& patients, int low, int high) {
    if (low < high) {
        std::string pivot = patients[high]->getPatientID();
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (patients[j]->getPatientID() < pivot) {
                i++;
                std::swap(patients[i], patients[j]);
            }
        }
        std::swap(patients[i + 1], patients[high]);
        int pi = i + 1;

        quickSortByID(patients, low, pi - 1);
        quickSortByID(patients, pi + 1, high);
    }
}

} // namespace Controller