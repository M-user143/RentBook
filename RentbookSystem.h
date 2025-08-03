#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

struct Property {
    int id;
    std::string address;
    double monthlyRent;
    std::string description;
    bool occupied;
    Property(int id, const std::string& address, double rent, 
             const std::string& desc, bool occ);
};

struct Tenant {
    int id;
    std::string name;
    std::string contact;
    std::string email;
    int propertyId;
    double balance;
    std::string lastPaymentDate;
    Tenant(int id, const std::string& name, const std::string& contact, 
           const std::string& email, int propId, double bal, 
           const std::string& lastPay);
};

struct Payment {
    int id;
    int tenantId;
    double amount;
    std::string date;
    std::string notes;
    Payment(int id, int tenantId, double amount, 
            const std::string& date, const std::string& notes);
};

struct LeaseAgreement {
    int id;
    int tenantId;
    int propertyId;
    std::string startDate;
    std::string endDate;
    double monthlyRent;
    std::string terms;
    std::string paymentDueDay;
    LeaseAgreement(int id, int tenantId, int propertyId, 
                   const std::string& start, const std::string& end, 
                   double rent, const std::string& terms, 
                   const std::string& dueDay);
};

class RentbookSystem {
public:
    RentbookSystem();
    
    // Core operations
    void addProperty(const std::string& address, double rent, const std::string& desc);
    void addTenant(const std::string& name, const std::string& contact, 
                   const std::string& email, int propId);
    void addLeaseAgreement(int tenantId, int propertyId, const std::string& start, 
                           const std::string& end, double rent, const std::string& terms, 
                           const std::string& dueDay);
    void recordPayment(int tenantId, double amount, const std::string& date, 
                       const std::string& notes);
    void calculateTenantBalances();
    
    // Data persistence
    void saveData(const std::string& filename);
    void loadData(const std::string& filename);
    
    // Utility
    static std::string getCurrentDate();
    static int daysBetween(const std::string& start, const std::string& end);
    static int monthsBetween(const std::string& start, const std::string& end);
    
    // Data access
    std::vector<Property> properties;
    std::vector<Tenant> tenants;
    std::vector<Payment> payments;
    std::vector<LeaseAgreement> leases;
    
private:
    int nextPropId = 1;
    int nextTenantId = 1;
    int nextPaymentId = 1;
    int nextLeaseId = 1;
};