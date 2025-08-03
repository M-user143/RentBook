#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <cctype>
#include <functional>


using namespace std;

// Helper functions
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}

// New portable date parsing function
tm parseDate(const string& dateStr) {
    tm date = {};
    stringstream ss(dateStr);
    char dash;
    ss >> date.tm_year >> dash >> date.tm_mon >> dash >> date.tm_mday;
    date.tm_year -= 1900;
    date.tm_mon -= 1;
    return date;
}

int daysBetween(const string& start, const string& end) {
    tm start_tm = parseDate(start);
    tm end_tm = parseDate(end);
    
    time_t start_time = mktime(&start_tm);
    time_t end_time = mktime(&end_tm);
    
    return difftime(end_time, start_time) / (60 * 60 * 24);
}

// Add this helper function for string case conversion
string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), 
              [](unsigned char c) { return tolower(c); });
    return lowerStr;
}


// ... [rest of the code remains the same] ...


class LeaseAgreement {
public:
    int id;
    int tenantId;
    int propertyId;
    string startDate;
    string endDate;
    double monthlyRent;
    string terms;
    string paymentDueDay; // e.g., "5" for 5th of each month

    LeaseAgreement(int id, int tenantId, int propertyId, const string& start, 
                  const string& end, double rent, const string& terms, const string& dueDay)
        : id(id), tenantId(tenantId), propertyId(propertyId), startDate(start), 
          endDate(end), monthlyRent(rent), terms(terms), paymentDueDay(dueDay) {}
};

class Property {
public:
    int id;
    string address;
    double monthlyRent;
    string description;
    bool occupied;

    Property(int id, const string& address, double rent, const string& desc)
        : id(id), address(address), monthlyRent(rent), description(desc), occupied(false) {}
};

class Tenant {
public:
    int id;
    string name;
    string contact;
    string email;
    int propertyId;
    double balance;
    string lastPaymentDate;

    Tenant(int id, const string& name, const string& contact, const string& email, int propId)
        : id(id), name(name), contact(contact), email(email), 
          propertyId(propId), balance(0.0), lastPaymentDate("") {}
};

class Payment {
public:
    int id;
    int tenantId;
    double amount;
    string date;
    string notes;

    Payment(int id, int tenantId, double amount, const string& date, const string& notes)
        : id(id), tenantId(tenantId), amount(amount), date(date), notes(notes) {}
};

class RentbookSystem {
private:
    vector<Property> properties;
    vector<Tenant> tenants;
    vector<Payment> payments;
    vector<LeaseAgreement> leases;
    int nextPropId = 1;
    int nextTenantId = 1;
    int nextPaymentId = 1;
    int nextLeaseId = 1;
    const string dataFile = "rentbook_data.txt";

    // Helper functions
    void displayProperty(const Property& prop) {
        cout << "\nID: " << prop.id
             << "\nAddress: " << prop.address
             << "\nRent: $" << fixed << setprecision(2) << prop.monthlyRent
             << "\nDescription: " << prop.description
             << "\nStatus: " << (prop.occupied ? "Occupied" : "Vacant") << "\n";
    }

    void displayTenant(const Tenant& tenant) {
        cout << "\nID: " << tenant.id
             << "\nName: " << tenant.name
             << "\nContact: " << tenant.contact
             << "\nEmail: " << tenant.email
             << "\nProperty ID: " << tenant.propertyId
             << "\nBalance: $" << fixed << setprecision(2) << tenant.balance
             << "\nLast Payment: " << (tenant.lastPaymentDate.empty() ? "Never" : tenant.lastPaymentDate) << "\n";
    }

    void displayLease(const LeaseAgreement& lease) {
        cout << "\nLease ID: " << lease.id
             << "\nTenant ID: " << lease.tenantId
             << "\nProperty ID: " << lease.propertyId
             << "\nStart Date: " << lease.startDate
             << "\nEnd Date: " << lease.endDate
             << "\nRent: $" << fixed << setprecision(2) << lease.monthlyRent
             << "\nPayment Due: " << lease.paymentDueDay << " of each month"
             << "\nTerms: " << lease.terms << "\n";
    }

    void updatePropertyOccupancy(int propId, bool occupied) {
        for (auto& prop : properties) {
            if (prop.id == propId) {
                prop.occupied = occupied;
                break;
            }
        }
    }

    void updateTenantBalance(int tenantId, double amount) {
        for (auto& tenant : tenants) {
            if (tenant.id == tenantId) {
                tenant.balance += amount;
                break;
            }
        }
    }

    void updateTenantLastPayment(int tenantId, const string& date) {
        for (auto& tenant : tenants) {
            if (tenant.id == tenantId) {
                tenant.lastPaymentDate = date;
                break;
            }
        }
    }

public:
    RentbookSystem() {
        loadData();
    }

    ~RentbookSystem() {
        saveData();
    }

    void addProperty() {
        cin.ignore();
        string address, desc;
        double rent;

        cout << "\nEnter property address: ";
        getline(cin, address);
        cout << "Enter monthly rent: $";
        cin >> rent;
        cin.ignore();
        cout << "Enter description: ";
        getline(cin, desc);

        properties.emplace_back(nextPropId++, address, rent, desc);
        cout << "Property added successfully! ID: " << nextPropId - 1 << "\n";
    }

    void addTenant() {
        if (properties.empty()) {
            cout << "No properties available. Add properties first.\n";
            return;
        }

        cin.ignore();
        string name, contact, email;
        int propId;

        cout << "\nAvailable Properties:\n";
        for (const auto& p : properties) {
            if (!p.occupied) {
                cout << p.id << ". " << p.address << "\n";
            }
        }

        cout << "Select property ID: ";
        cin >> propId;

        auto propIt = find_if(properties.begin(), properties.end(), 
                            [propId](const Property& p) { return p.id == propId; });
        if (propIt == properties.end()) {
            cout << "Invalid property ID!\n";
            return;
        }

        if (propIt->occupied) {
            cout << "Property is already occupied!\n";
            return;
        }

        cin.ignore();
        cout << "Enter tenant name: ";
        getline(cin, name);
        cout << "Enter contact info: ";
        getline(cin, contact);
        cout << "Enter email: ";
        getline(cin, email);

        tenants.emplace_back(nextTenantId++, name, contact, email, propId);
        updatePropertyOccupancy(propId, true);
        cout << "Tenant added successfully! ID: " << nextTenantId - 1 << "\n";
    }

    void addLeaseAgreement() {
        if (tenants.empty()) {
            cout << "No tenants available. Add tenants first.\n";
            return;
        }

        int tenantId;
        string startDate, endDate, terms, dueDay;

        cout << "\nAvailable Tenants:\n";
        for (const auto& t : tenants) {
            bool hasLease = any_of(leases.begin(), leases.end(), 
                                [t](const LeaseAgreement& l) { return l.tenantId == t.id; });
            if (!hasLease) {
                cout << t.id << ". " << t.name << " (Property: " << t.propertyId << ")\n";
            }
        }

        cout << "Select tenant ID: ";
        cin >> tenantId;

        auto tenantIt = find_if(tenants.begin(), tenants.end(),
                            [tenantId](const Tenant& t) { return t.id == tenantId; });
        if (tenantIt == tenants.end()) {
            cout << "Invalid tenant ID!\n";
            return;
        }

        cin.ignore();
        cout << "Enter lease start date (YYYY-MM-DD): ";
        getline(cin, startDate);
        cout << "Enter lease end date (YYYY-MM-DD): ";
        getline(cin, endDate);
        cout << "Enter payment due day (e.g., 5 for 5th of month): ";
        getline(cin, dueDay);
        cout << "Enter lease terms: ";
        getline(cin, terms);

        auto propIt = find_if(properties.begin(), properties.end(),
                            [tenantIt](const Property& p) { return p.id == tenantIt->propertyId; });
        
        if (propIt == properties.end()) {
            cout << "Property not found!\n";
            return;
        }

        leases.emplace_back(nextLeaseId++, tenantId, tenantIt->propertyId, startDate, 
                         endDate, propIt->monthlyRent, terms, dueDay);
        cout << "Lease agreement added successfully! ID: " << nextLeaseId - 1 << "\n";
    }

    void recordPayment() {
        if (tenants.empty()) {
            cout << "No tenants available. Add tenants first.\n";
            return;
        }

        int tenantId;
        double amount;
        string date, notes;

        cout << "\nAvailable Tenants:\n";
        for (const auto& t : tenants) 
            cout << t.id << ". " << t.name << " (Property: " << t.propertyId << ")\n";

        cout << "Select tenant ID: ";
        cin >> tenantId;

        auto tenantIt = find_if(tenants.begin(), tenants.end(),
                            [tenantId](const Tenant& t) { return t.id == tenantId; });
        if (tenantIt == tenants.end()) {
            cout << "Invalid tenant ID!\n";
            return;
        }

        auto leaseIt = find_if(leases.begin(), leases.end(),
                            [tenantId](const LeaseAgreement& l) { return l.tenantId == tenantId; });
        if (leaseIt == leases.end()) {
            cout << "No lease agreement found for this tenant!\n";
            return;
        }

        cout << "Enter payment amount: $";
        cin >> amount;
        cin.ignore();
        cout << "Enter date (YYYY-MM-DD) [Enter for today]: ";
        getline(cin, date);
        if (date.empty()) date = getCurrentDate();
        cout << "Enter notes: ";
        getline(cin, notes);

        payments.emplace_back(nextPaymentId++, tenantId, amount, date, notes);
        updateTenantBalance(tenantId, -amount);
        updateTenantLastPayment(tenantId, date);
        cout << "Payment recorded successfully!\n";
    }

    void displayAllProperties() {
        if (properties.empty()) {
            cout << "No properties found.\n";
            return;
        }
        cout << "\n=== ALL PROPERTIES ===\n";
        for (const auto& p : properties) 
            displayProperty(p);
    }

    void displayAllTenants() {
        if (tenants.empty()) {
            cout << "No tenants found.\n";
            return;
        }
        cout << "\n=== ALL TENANTS ===\n";
        for (const auto& t : tenants) 
            displayTenant(t);
    }

    void displayAllLeases() {
        if (leases.empty()) {
            cout << "No lease agreements found.\n";
            return;
        }
        cout << "\n=== ALL LEASE AGREEMENTS ===\n";
        for (const auto& l : leases) 
            displayLease(l);
    }

    void displayPaymentHistory() {
        if (payments.empty()) {
            cout << "No payments recorded.\n";
            return;
        }

        cout << "\n=== PAYMENT HISTORY ===\n";
        for (const auto& p : payments) {
            auto tenantIt = find_if(tenants.begin(), tenants.end(),
                                [p](const Tenant& t) { return t.id == p.tenantId; });
            
            cout << "Payment ID: " << p.id
                 << "\nDate: " << p.date
                 << "\nTenant: " << (tenantIt != tenants.end() ? tenantIt->name : "Unknown")
                 << "\nAmount: $" << fixed << setprecision(2) << p.amount
                 << "\nNotes: " << p.notes << "\n\n";
        }
    }

    void checkPaymentReminders() {
        if (tenants.empty()) return;

        string today = getCurrentDate();
        cout << "\n=== PAYMENT REMINDERS (" << today << ") ===\n";

        bool found = false;
        for (const auto& tenant : tenants) {
            auto leaseIt = find_if(leases.begin(), leases.end(),
                                [tenant](const LeaseAgreement& l) { return l.tenantId == tenant.id; });
            
            if (leaseIt == leases.end()) continue;

            // Calculate days since last payment
            int daysSinceLast = 0;
            if (!tenant.lastPaymentDate.empty()) {
                daysSinceLast = daysBetween(tenant.lastPaymentDate, today);
            }

            // Check if payment is overdue
            if (daysSinceLast > 30) {
                cout << "URGENT: Tenant " << tenant.name << " (ID: " << tenant.id 
                     << ") is " << daysSinceLast - 30 << " days overdue!\n";
                found = true;
            }
            // Check if payment due soon
            else if (daysSinceLast < 25) {
                cout << "REMINDER: Tenant " << tenant.name << " (ID: " << tenant.id 
                     << ") payment due in " << 30 - daysSinceLast << " days\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No payment reminders at this time.\n";
        }
    }

    void generateFinancialReport() {
        if (properties.empty()) {
            cout << "No properties available.\n";
            return;
        }

        double totalRent = 0.0;
        double totalCollected = 0.0;
        double totalOwed = 0.0;

        cout << "\n=== FINANCIAL REPORT ===\n";

        // Property-wise report
        for (const auto& prop : properties) {
            double propRent = 0.0;
            double propCollected = 0.0;
            double propOwed = 0.0;

            for (const auto& lease : leases) {
                if (lease.propertyId == prop.id) {
                    propRent += lease.monthlyRent;

                    auto tenantIt = find_if(tenants.begin(), tenants.end(),
                                        [lease](const Tenant& t) { return t.id == lease.tenantId; });
                    
                    if (tenantIt != tenants.end()) {
                        propOwed += tenantIt->balance;
                        propCollected += (lease.monthlyRent - tenantIt->balance);
                    }
                }
            }

            cout << "\nProperty " << prop.id << " (" << prop.address << "):"
                 << "\n  Monthly Rent: $" << fixed << setprecision(2) << propRent
                 << "\n  Collected: $" << propCollected
                 << "\n  Owed: $" << propOwed << "\n";

            totalRent += propRent;
            totalCollected += propCollected;
            totalOwed += propOwed;
        }

        cout << "\n=== TOTALS ==="
             << "\nTotal Monthly Rent: $" << fixed << setprecision(2) << totalRent
             << "\nTotal Collected: $" << totalCollected
             << "\nTotal Owed: $" << totalOwed
             << "\n";
    }

    void searchRecords() {
        if (properties.empty() && tenants.empty() && payments.empty()) {
            cout << "No records available.\n";
            return;
        }

        cin.ignore();
        string query;
        cout << "\nEnter search query: ";
        getline(cin, query);
        string lowerQuery = toLower(query);

        cout << "\n=== SEARCH RESULTS ===\n";

        // Search properties
        for (const auto& prop : properties) {
            string lowerAddr = toLower(prop.address);
            string lowerDesc = toLower(prop.description);
            
            if (lowerAddr.find(lowerQuery) != string::npos || 
                lowerDesc.find(lowerQuery) != string::npos) {
                displayProperty(prop);
            }
        }

        // Search tenants
        for (const auto& tenant : tenants) {
            string lowerName = toLower(tenant.name);
            string lowerContact = toLower(tenant.contact);
            string lowerEmail = toLower(tenant.email);
            
            if (lowerName.find(lowerQuery) != string::npos || 
                lowerContact.find(lowerQuery) != string::npos || 
                lowerEmail.find(lowerQuery) != string::npos) {
                displayTenant(tenant);
            }
        }

        // Search payments
        for (const auto& payment : payments) {
            string lowerNotes = toLower(payment.notes);
            
            if (lowerNotes.find(lowerQuery) != string::npos) {
                auto tenantIt = find_if(tenants.begin(), tenants.end(),
                                    [payment](const Tenant& t) { return t.id == payment.tenantId; });
                
                cout << "Payment ID: " << payment.id
                     << "\nDate: " << payment.date
                     << "\nTenant: " << (tenantIt != tenants.end() ? tenantIt->name : "Unknown")
                     << "\nAmount: $" << fixed << setprecision(2) << payment.amount
                     << "\nNotes: " << payment.notes << "\n\n";
            }
        }
    }

    void trackOverduePayments() {
        if (tenants.empty()) {
            cout << "No tenants available.\n";
            return;
        }

        string today = getCurrentDate();
        cout << "\n=== OVERDUE PAYMENTS (" << today << ") ===\n";

        bool found = false;
        for (const auto& tenant : tenants) {
            if (tenant.balance > 0) {
                auto leaseIt = find_if(leases.begin(), leases.end(),
                                    [tenant](const LeaseAgreement& l) { return l.tenantId == tenant.id; });
                
                int daysOverdue = 0;
                if (!tenant.lastPaymentDate.empty()) {
                    daysOverdue = daysBetween(tenant.lastPaymentDate, today) - 30;
                }

                cout << "Tenant: " << tenant.name
                     << "\nProperty ID: " << tenant.propertyId
                     << "\nAmount Owed: $" << fixed << setprecision(2) << tenant.balance
                     << "\nDays Overdue: " << max(0, daysOverdue) << "\n\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No overdue payments at this time.\n";
        }
    }

    void calculateTenantBalances() {
        if (tenants.empty()) {
            cout << "No tenants available.\n";
            return;
        }

        string today = getCurrentDate();
        cout << "\n=== TENANT BALANCES ===\n";

        for (auto& tenant : tenants) {
            auto leaseIt = find_if(leases.begin(), leases.end(),
                                [tenant](const LeaseAgreement& l) { return l.tenantId == tenant.id; });
            
            if (leaseIt != leases.end()) {
                // Calculate months since last payment
                int months = 1;
                if (!tenant.lastPaymentDate.empty()) {
                    months = daysBetween(tenant.lastPaymentDate, today) / 30;
                    if (months < 1) months = 1;
                }
                
                // Update balance
                double rentDue = leaseIt->monthlyRent * months;
                tenant.balance = rentDue;
                
                cout << "Tenant: " << tenant.name
                     << "\nProperty ID: " << tenant.propertyId
                     << "\nRent Due: $" << fixed << setprecision(2) << rentDue
                     << "\nBalance: $" << tenant.balance << "\n\n";
            }
        }
    }

    void saveData() {
        ofstream outFile(dataFile);
        if (!outFile) {
            cerr << "Error saving data!\n";
            return;
        }

        // Save properties
        outFile << "[Properties]\n";
        for (const auto& p : properties) {
            outFile << p.id << "|" << p.address << "|" << p.monthlyRent << "|" 
                   << p.description << "|" << p.occupied << "\n";
        }

        // Save tenants
        outFile << "[Tenants]\n";
        for (const auto& t : tenants) {
            outFile << t.id << "|" << t.name << "|" << t.contact << "|" 
                   << t.email << "|" << t.propertyId << "|" 
                   << t.balance << "|" << t.lastPaymentDate << "\n";
        }

        // Save payments
        outFile << "[Payments]\n";
        for (const auto& p : payments) {
            outFile << p.id << "|" << p.tenantId << "|" << p.amount << "|" 
                   << p.date << "|" << p.notes << "\n";
        }

        // Save leases
        outFile << "[Leases]\n";
        for (const auto& l : leases) {
            outFile << l.id << "|" << l.tenantId << "|" << l.propertyId << "|" 
                   << l.startDate << "|" << l.endDate << "|" 
                   << l.monthlyRent << "|" << l.terms << "|" 
                   << l.paymentDueDay << "\n";
        }

        outFile.close();
    }

    void loadData() {
        ifstream inFile(dataFile);
        if (!inFile) {
            cout << "No saved data found. Starting fresh.\n";
            return;
        }

        string line;
        string section = "";

        while (getline(inFile, line)) {
            if (line == "[Properties]") {
                section = "properties";
                continue;
            } else if (line == "[Tenants]") {
                section = "tenants";
                continue;
            } else if (line == "[Payments]") {
                section = "payments";
                continue;
            } else if (line == "[Leases]") {
                section = "leases";
                continue;
            }

            if (line.empty()) continue;

            if (section == "properties") {
                stringstream ss(line);
                string part;
                vector<string> parts;
                
                while (getline(ss, part, '|')) {
                    parts.push_back(part);
                }
                
                if (parts.size() >= 5) {
                    int id = stoi(parts[0]);
                    double rent = stod(parts[2]);
                    bool occupied = (parts[4] == "1");
                    properties.emplace_back(id, parts[1], rent, parts[3]);
                    properties.back().occupied = occupied;
                    nextPropId = max(nextPropId, id + 1);
                }
            }
            else if (section == "tenants") {
                stringstream ss(line);
                string part;
                vector<string> parts;
                
                while (getline(ss, part, '|')) {
                    parts.push_back(part);
                }
                
                if (parts.size() >= 7) {
                    int id = stoi(parts[0]);
                    int propId = stoi(parts[4]);
                    double balance = stod(parts[5]);
                    tenants.emplace_back(id, parts[1], parts[2], parts[3], propId);
                    tenants.back().balance = balance;
                    tenants.back().lastPaymentDate = parts[6];
                    nextTenantId = max(nextTenantId, id + 1);
                }
            }
            else if (section == "payments") {
                stringstream ss(line);
                string part;
                vector<string> parts;
                
                while (getline(ss, part, '|')) {
                    parts.push_back(part);
                }
                
                if (parts.size() >= 5) {
                    int id = stoi(parts[0]);
                    int tenantId = stoi(parts[1]);
                    double amount = stod(parts[2]);
                    payments.emplace_back(id, tenantId, amount, parts[3], parts[4]);
                    nextPaymentId = max(nextPaymentId, id + 1);
                }
            }
            else if (section == "leases") {
                stringstream ss(line);
                string part;
                vector<string> parts;
                
                while (getline(ss, part, '|')) {
                    parts.push_back(part);
                }
                
                if (parts.size() >= 8) {
                    int id = stoi(parts[0]);
                    int tenantId = stoi(parts[1]);
                    int propId = stoi(parts[2]);
                    double rent = stod(parts[5]);
                    leases.emplace_back(id, tenantId, propId, parts[3], parts[4], rent, parts[6], parts[7]);
                    nextLeaseId = max(nextLeaseId, id + 1);
                }
            }
        }

        inFile.close();
        cout << "Data loaded successfully!\n";
    }
};

int main() {
    RentbookSystem system;
    int choice;

    do {
        cout << "\n===== RENTBOOK SYSTEM =====\n"
             << "1. Add Property\n"
             << "2. Add Tenant\n"
             << "3. Add Lease Agreement\n"
             << "4. Record Payment\n"
             << "5. View All Properties\n"
             << "6. View All Tenants\n"
             << "7. View All Lease Agreements\n"
             << "8. View Payment History\n"
             << "9. Check Payment Reminders\n"
             << "10. Generate Financial Report\n"
             << "11. Search Records\n"
             << "12. Track Overdue Payments\n"
             << "13. Calculate Tenant Balances\n"
             << "14. Save Data\n"
             << "15. Exit\n"
             << "Enter your choice: ";
        
        cin >> choice;

        switch (choice) {
            case 1: system.addProperty(); break;
            case 2: system.addTenant(); break;
            case 3: system.addLeaseAgreement(); break;
            case 4: system.recordPayment(); break;
            case 5: system.displayAllProperties(); break;
            case 6: system.displayAllTenants(); break;
            case 7: system.displayAllLeases(); break;
            case 8: system.displayPaymentHistory(); break;
            case 9: system.checkPaymentReminders(); break;
            case 10: system.generateFinancialReport(); break;
            case 11: system.searchRecords(); break;
            case 12: system.trackOverduePayments(); break;
            case 13: system.calculateTenantBalances(); break;
            case 14: system.saveData(); break;
            case 15: cout << "Exiting system...\n"; break;
            default:
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 15);

    return 0;
}