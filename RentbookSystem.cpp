#include "RentbookSystem.h"
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <map>

Property::Property(int id, const std::string& address, double rent, 
                   const std::string& desc, bool occ)
    : id(id), address(address), monthlyRent(rent), 
      description(desc), occupied(occ) {}

Tenant::Tenant(int id, const std::string& name, const std::string& contact, 
               const std::string& email, int propId, double bal, 
               const std::string& lastPay)
    : id(id), name(name), contact(contact), email(email), 
      propertyId(propId), balance(bal), lastPaymentDate(lastPay) {}

Payment::Payment(int id, int tenantId, double amount, 
                 const std::string& date, const std::string& notes)
    : id(id), tenantId(tenantId), amount(amount), 
      date(date), notes(notes) {}

LeaseAgreement::LeaseAgreement(int id, int tenantId, int propertyId, 
                               const std::string& start, const std::string& end, 
                               double rent, const std::string& terms, 
                               const std::string& dueDay)
    : id(id), tenantId(tenantId), propertyId(propertyId), 
      startDate(start), endDate(end), monthlyRent(rent), 
      terms(terms), paymentDueDay(dueDay) {}

RentbookSystem::RentbookSystem() {}

void RentbookSystem::addProperty(const std::string& address, double rent, const std::string& desc) {
    properties.emplace_back(nextPropId++, address, rent, desc, false);
}

void RentbookSystem::addTenant(const std::string& name, const std::string& contact, const std::string& email, int propId) {
    tenants.emplace_back(nextTenantId++, name, contact, email, propId, 0.0, "");
    for (auto& prop : properties) {
        if (prop.id == propId) {
            prop.occupied = true;
            break;
        }
    }
}

void RentbookSystem::addLeaseAgreement(int tenantId, int propertyId, const std::string& start, 
                                      const std::string& end, double rent, const std::string& terms, 
                                      const std::string& dueDay) {
    leases.emplace_back(nextLeaseId++, tenantId, propertyId, start, end, rent, terms, dueDay);
}

void RentbookSystem::recordPayment(int tenantId, double amount, const std::string& date, const std::string& notes) {
    payments.emplace_back(nextPaymentId++, tenantId, amount, date, notes);
    for (auto& tenant : tenants) {
        if (tenant.id == tenantId) {
            tenant.balance -= amount;
            tenant.lastPaymentDate = date;
            break;
        }
    }
}

int RentbookSystem::monthsBetween(const std::string& start, const std::string& end) {
    int start_year, start_month, start_day;
    int end_year, end_month, end_day;
    
    if (std::sscanf(start.c_str(), "%d-%d-%d", &start_year, &start_month, &start_day) != 3) return 0;
    if (std::sscanf(end.c_str(), "%d-%d-%d", &end_year, &end_month, &end_day) != 3) return 0;
    
    int months = (end_year - start_year) * 12 + (end_month - start_month);
    if (end_day < start_day) months--;
    return std::max(0, months);
}

void RentbookSystem::calculateTenantBalances() {
    std::string today = getCurrentDate();
    std::map<int, double> totalPayments;

    // Calculate total payments per tenant
    for (const auto& payment : payments) {
        totalPayments[payment.tenantId] += payment.amount;
    }

    for (auto& tenant : tenants) {
        tenant.balance = 0; // Reset before recalculating
        
        for (const auto& lease : leases) {
            if (lease.tenantId == tenant.id) {
                // Calculate months from lease start to today or lease end
                std::string endDate = (lease.endDate > today) ? today : lease.endDate;
                int months = monthsBetween(lease.startDate, endDate);
                
                if (months > 0 && today >= lease.startDate) {
                    double totalRent = lease.monthlyRent * months;
                    tenant.balance = totalRent - totalPayments[tenant.id];
                }
                break;
            }
        }
    }
}

void RentbookSystem::saveData(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) return;

    outFile << "[Properties]\n";
    for (const auto& p : properties) {
        outFile << p.id << "|" << p.address << "|" << p.monthlyRent << "|" 
               << p.description << "|" << p.occupied << "\n";
    }

    outFile << "[Tenants]\n";
    for (const auto& t : tenants) {
        outFile << t.id << "|" << t.name << "|" << t.contact << "|" 
               << t.email << "|" << t.propertyId << "|" 
               << t.balance << "|" << t.lastPaymentDate << "\n";
    }

    outFile << "[Payments]\n";
    for (const auto& p : payments) {
        outFile << p.id << "|" << p.tenantId << "|" << p.amount << "|" 
               << p.date << "|" << p.notes << "\n";
    }

    outFile << "[Leases]\n";
    for (const auto& l : leases) {
        outFile << l.id << "|" << l.tenantId << "|" << l.propertyId << "|" 
               << l.startDate << "|" << l.endDate << "|" 
               << l.monthlyRent << "|" << l.terms << "|" 
               << l.paymentDueDay << "\n";
    }

    outFile.close();
}

void RentbookSystem::loadData(const std::string& filename) {
    // Clear existing data
    properties.clear();
    tenants.clear();
    payments.clear();
    leases.clear();
    
    // Reset IDs
    nextPropId = 1;
    nextTenantId = 1;
    nextPaymentId = 1;
    nextLeaseId = 1;

    std::ifstream inFile(filename);
    if (!inFile) return;

    std::string line;
    std::string section = "";

    while (std::getline(inFile, line)) {
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
            std::stringstream ss(line);
            std::string part;
            std::vector<std::string> parts;
            
            while (std::getline(ss, part, '|')) {
                parts.push_back(part);
            }
            
            if (parts.size() >= 5) {
                int id = std::stoi(parts[0]);
                double rent = std::stod(parts[2]);
                bool occupied = (parts[4] == "1");
                properties.emplace_back(id, parts[1], rent, parts[3], occupied);
                nextPropId = std::max(nextPropId, id + 1);
            }
        }
        else if (section == "tenants") {
            std::stringstream ss(line);
            std::string part;
            std::vector<std::string> parts;
            
            while (std::getline(ss, part, '|')) {
                parts.push_back(part);
            }
            
            if (parts.size() >= 7) {
                int id = std::stoi(parts[0]);
                int propId = std::stoi(parts[4]);
                double balance = std::stod(parts[5]);
                tenants.emplace_back(id, parts[1], parts[2], parts[3], propId, balance, parts[6]);
                nextTenantId = std::max(nextTenantId, id + 1);
            }
        }
        else if (section == "payments") {
            std::stringstream ss(line);
            std::string part;
            std::vector<std::string> parts;
            
            while (std::getline(ss, part, '|')) {
                parts.push_back(part);
            }
            
            if (parts.size() >= 5) {
                int id = std::stoi(parts[0]);
                int tenantId = std::stoi(parts[1]);
                double amount = std::stod(parts[2]);
                payments.emplace_back(id, tenantId, amount, parts[3], parts[4]);
                nextPaymentId = std::max(nextPaymentId, id + 1);
            }
        }
        else if (section == "leases") {
            std::stringstream ss(line);
            std::string part;
            std::vector<std::string> parts;
            
            while (std::getline(ss, part, '|')) {
                parts.push_back(part);
            }
            
            if (parts.size() >= 8) {
                int id = std::stoi(parts[0]);
                int tenantId = std::stoi(parts[1]);
                int propId = std::stoi(parts[2]);
                double rent = std::stod(parts[5]);
                leases.emplace_back(id, tenantId, propId, parts[3], parts[4], rent, parts[6], parts[7]);
                nextLeaseId = std::max(nextLeaseId, id + 1);
            }
        }
    }

    inFile.close();
    calculateTenantBalances();
}

std::string RentbookSystem::getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return std::string(buffer);
}

int RentbookSystem::daysBetween(const std::string& start, const std::string& end) {
    tm start_tm = {};
    tm end_tm = {};
    
    // Parse start date
    if (std::sscanf(start.c_str(), "%d-%d-%d", &start_tm.tm_year, &start_tm.tm_mon, &start_tm.tm_mday) != 3)
        return 0;
    start_tm.tm_year -= 1900;
    start_tm.tm_mon -= 1;
    start_tm.tm_isdst = -1;  // Important for DST handling
    
    // Parse end date
    if (std::sscanf(end.c_str(), "%d-%d-%d", &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday) != 3)
        return 0;
    end_tm.tm_year -= 1900;
    end_tm.tm_mon -= 1;
    end_tm.tm_isdst = -1;
    
    time_t start_time = mktime(&start_tm);
    time_t end_time = mktime(&end_tm);
    
    if (start_time == -1 || end_time == -1) return 0;
    return static_cast<int>(difftime(end_time, start_time) / (60 * 60 * 24));
}