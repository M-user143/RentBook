#include "RentbookApp.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QLineEdit>
#include <QProgressBar>
#include <QStandardPaths>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QHeaderView>
#include <QIntValidator>
#include <QMenuBar>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFile>
#include <string>

using namespace std;

RentbookApp::RentbookApp(QWidget *parent) : QMainWindow(parent) {
    system = new RentbookSystem();
    setWindowTitle("RentBook Property Management");
    setMinimumSize(1000, 700);
    
    // Set the default data file path
    dataFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/rentbook_data.rbk";
    
    createMenu();
    createTabs();
    createStatusBar();
    
    connect(tabWidget, &QTabWidget::currentChanged, this, &RentbookApp::refreshCurrentTab);
    
    // Load data automatically on startup
    autoLoad();
}

RentbookApp::~RentbookApp() {
    autoSave();
    delete system;
}

void RentbookApp::refreshCurrentTab(int index) {
    switch(index) {
        case 0: refreshProperties(); break;
        case 1: refreshTenants(); break;
        case 2: refreshLeases(); break;
        case 3: refreshPayments(); break;
        case 4: refreshReports(); break;
    }
}

void RentbookApp::addProperty() {
    QDialog dialog(this);
    dialog.setWindowTitle("Add Property");
    
    QFormLayout form(&dialog);
    
    QLineEdit addressInput;
    QDoubleSpinBox rentInput;
    rentInput.setMinimum(0);
    rentInput.setMaximum(100000);
    rentInput.setPrefix("$ ");
    QTextEdit descInput;
    
    form.addRow("Address:", &addressInput);
    form.addRow("Monthly Rent:", &rentInput);
    form.addRow("Description:", &descInput);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
                              Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Input validation
        if (addressInput.text().isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Address cannot be empty!");
            return;
        }
        if (rentInput.value() <= 0) {
            QMessageBox::warning(this, "Input Error", "Rent must be positive!");
            return;
        }
        
        system->addProperty(addressInput.text().toStdString(), 
                           rentInput.value(), 
                           descInput.toPlainText().toStdString());
        refreshProperties();
        autoSave();
        statusBar()->showMessage("Property added successfully", 3000);
    }
}

void RentbookApp::addTenant() {
    if (system->properties.empty()) {
        QMessageBox::warning(this, "No Properties", "Add properties first!");
        return;
    }
    
    QDialog dialog(this);
    dialog.setWindowTitle("Add Tenant");
    
    QFormLayout form(&dialog);
    
    QLineEdit nameInput;
    QLineEdit contactInput;
    QLineEdit emailInput;
    QComboBox propertyInput;
    
    for (const auto& prop : system->properties) {
        if (!prop.occupied) {
            propertyInput.addItem(QString("%1: %2").arg(prop.id).arg(prop.address.c_str()), prop.id);
        }
    }
    
    if (propertyInput.count() == 0) {
        QMessageBox::warning(this, "No Available Properties", "All properties are occupied!");
        return;
    }
    
    form.addRow("Name:", &nameInput);
    form.addRow("Contact:", &contactInput);
    form.addRow("Email:", &emailInput);
    form.addRow("Property:", &propertyInput);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
                              Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Input validation
        if (nameInput.text().isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Name cannot be empty!");
            return;
        }
        
        int propId = propertyInput.currentData().toInt();
        system->addTenant(nameInput.text().toStdString(), 
                        contactInput.text().toStdString(), 
                        emailInput.text().toStdString(), 
                        propId);
        autoSave();
        refreshTenants();
        statusBar()->showMessage("Tenant added successfully", 3000);
    }
}

void RentbookApp::addLease() {
    if (system->tenants.empty()) {
        QMessageBox::warning(this, "No Tenants", "Add tenants first!");
        return;
    }
    
    QDialog dialog(this);
    dialog.setWindowTitle("Add Lease Agreement");
    
    QFormLayout form(&dialog);
    
    QComboBox tenantInput;
    for (const auto& tenant : system->tenants) {
        bool hasLease = false;
        for (const auto& lease : system->leases) {
            if (lease.tenantId == tenant.id) {
                hasLease = true;
                break;
            }
        }
        if (!hasLease) {
            tenantInput.addItem(QString("%1: %2").arg(tenant.id).arg(tenant.name.c_str()), tenant.id);
        }
    }
    
    if (tenantInput.count() == 0) {
        QMessageBox::warning(this, "No Available Tenants", "All tenants already have leases!");
        return;
    }
    
    QDateEdit startDate;
    startDate.setDate(QDate::currentDate());
    startDate.setCalendarPopup(true);
    
    QDateEdit endDate;
    endDate.setDate(QDate::currentDate().addYears(1));
    endDate.setCalendarPopup(true);
    
    QLineEdit dueDayInput;
    dueDayInput.setValidator(new QIntValidator(1, 31, this));
    dueDayInput.setText("5");
    
    QTextEdit termsInput;
    
    form.addRow("Tenant:", &tenantInput);
    form.addRow("Start Date:", &startDate);
    form.addRow("End Date:", &endDate);
    form.addRow("Payment Due Day:", &dueDayInput);
    form.addRow("Terms:", &termsInput);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
                              Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Validate dates
        if (startDate.date() >= endDate.date()) {
            QMessageBox::warning(this, "Date Error", "End date must be after start date!");
            return;
        }
        
        int tenantId = tenantInput.currentData().toInt();
        int propId = 0;
        double rent = 0.0;
        
        for (const auto& tenant : system->tenants) {
            if (tenant.id == tenantId) {
                propId = tenant.propertyId;
                for (const auto& prop : system->properties) {
                    if (prop.id == propId) {
                        rent = prop.monthlyRent;
                        break;
                    }
                }
                break;
            }
        }
        
        system->addLeaseAgreement(tenantId, propId,
                                 startDate.date().toString("yyyy-MM-dd").toStdString(),
                                 endDate.date().toString("yyyy-MM-dd").toStdString(),
                                 rent,
                                 termsInput.toPlainText().toStdString(),
                                 dueDayInput.text().toStdString());
        refreshLeases();
        autoSave();
        statusBar()->showMessage("Lease agreement added successfully", 3000);
    }
}

void RentbookApp::recordPayment() {
    if (system->tenants.empty()) {
        QMessageBox::warning(this, "No Tenants", "Add tenants first!");
        return;
    }
    
    QDialog dialog(this);
    dialog.setWindowTitle("Record Payment");
    
    QFormLayout form(&dialog);
    
    QComboBox tenantInput;
    for (const auto& tenant : system->tenants) {
        tenantInput.addItem(QString("%1: %2").arg(tenant.id).arg(tenant.name.c_str()), tenant.id);
    }
    
    QDoubleSpinBox amountInput;
    amountInput.setMinimum(0);
    amountInput.setMaximum(100000);
    amountInput.setPrefix("$ ");
    
    QDateEdit dateInput;
    dateInput.setDate(QDate::currentDate());
    dateInput.setCalendarPopup(true);
    
    QLineEdit notesInput;
    
    form.addRow("Tenant:", &tenantInput);
    form.addRow("Amount:", &amountInput);
    form.addRow("Date:", &dateInput);
    form.addRow("Notes:", &notesInput);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
                              Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Input validation
        if (amountInput.value() <= 0) {
            QMessageBox::warning(this, "Input Error", "Amount must be positive!");
            return;
        }
        
        int tenantId = tenantInput.currentData().toInt();
        system->recordPayment(tenantId,
                             amountInput.value(),
                             dateInput.date().toString("yyyy-MM-dd").toStdString(),
                             notesInput.text().toStdString());
        refreshPayments();
        autoSave();
        statusBar()->showMessage("Payment recorded successfully", 3000);
    }
}

void RentbookApp::generateReport() {
    system->calculateTenantBalances();
    
    double totalRent = 0.0;
    double totalCollected = 0.0;
    double totalOwed = 0.0;
    
    QMap<int, double> propRent;
    QMap<int, double> propCollected;
    QMap<int, double> propOwed;
    
    for (const auto& prop : system->properties) {
        propRent[prop.id] = 0.0;
        propCollected[prop.id] = 0.0;
        propOwed[prop.id] = 0.0;
    }
    
    for (const auto& lease : system->leases) {
        propRent[lease.propertyId] += lease.monthlyRent;
        totalRent += lease.monthlyRent;
        
        for (const auto& tenant : system->tenants) {
            if (tenant.id == lease.tenantId) {
                propOwed[lease.propertyId] += tenant.balance;
                totalOwed += tenant.balance;
                propCollected[lease.propertyId] += (lease.monthlyRent - tenant.balance);
                totalCollected += (lease.monthlyRent - tenant.balance);
            }
        }
    }
    
    reportText->clear();
    reportText->append("<h2>Financial Report</h2>");
    reportText->append("<h3>Property Summary</h3>");
    reportText->append("<table border='1' cellpadding='5' style='border-collapse:collapse;'>");
    reportText->append("<tr><th>Property ID</th><th>Address</th><th>Rent</th><th>Collected</th><th>Owed</th></tr>");
    
    for (const auto& prop : system->properties) {
        reportText->append(QString("<tr><td align='center'>%1</td><td>%2</td><td align='right'>$%3</td><td align='right'>$%4</td><td align='right'>$%5</td></tr>")
                          .arg(prop.id)
                          .arg(prop.address.c_str())
                          .arg(propRent[prop.id], 0, 'f', 2)
                          .arg(propCollected[prop.id], 0, 'f', 2)
                          .arg(propOwed[prop.id], 0, 'f', 2));
    }
    
    reportText->append("</table>");
    reportText->append("<br><h3>Totals</h3>");
    reportText->append(QString("Total Monthly Rent: <b>$%1</b><br>").arg(totalRent, 0, 'f', 2));
    reportText->append(QString("Total Collected: <b>$%1</b><br>").arg(totalCollected, 0, 'f', 2));
    reportText->append(QString("Total Owed: <b>$%1</b><br>").arg(totalOwed, 0, 'f', 2));
    
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->setTitle("Financial Overview");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    
    QtCharts::QBarSet *rentSet = new QtCharts::QBarSet("Monthly Rent");
    QtCharts::QBarSet *collectedSet = new QtCharts::QBarSet("Collected");
    QtCharts::QBarSet *owedSet = new QtCharts::QBarSet("Owed");
    
    for (const auto& prop : system->properties) {
        *rentSet << propRent[prop.id];
        *collectedSet << propCollected[prop.id];
        *owedSet << propOwed[prop.id];
    }
    
    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
    series->append(rentSet);
    series->append(collectedSet);
    series->append(owedSet);
    chart->addSeries(series);
    
    QStringList categories;
    for (const auto& prop : system->properties) {
        categories << QString("Prop %1").arg(prop.id);
    }
    
    QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setLabelFormat("$%.0f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chartView->setChart(chart);
    
    string today = RentbookSystem::getCurrentDate();
    reportText->append("<br><h3>Overdue Payments</h3>");
    reportText->append("<table border='1' cellpadding='5' style='border-collapse:collapse;'>");
    reportText->append("<tr><th>Tenant</th><th>Property</th><th>Amount Owed</th><th>Days Overdue</th></tr>");
    
    for (const auto& tenant : system->tenants) {
        if (tenant.balance > 0) {
            int daysOverdue = 0;
            if (!tenant.lastPaymentDate.empty()) {
                // Get due date from lease agreement
                std::string dueDay = "5"; // Default
                for (const auto& lease : system->leases) {
                    if (lease.tenantId == tenant.id) {
                        dueDay = lease.paymentDueDay;
                        break;
                    }
                }
                
                // Create last due date string
                std::string lastDueDate = tenant.lastPaymentDate.substr(0, 8) + dueDay;
                daysOverdue = RentbookSystem::daysBetween(lastDueDate, today);
            }
            daysOverdue = std::max(0, daysOverdue);
            
            string propAddress = "Unknown";
            for (const auto& prop : system->properties) {
                if (prop.id == tenant.propertyId) {
                    propAddress = prop.address;
                    break;
                }
            }
            
            QColor color = daysOverdue > 30 ? QColor(255, 200, 200) : Qt::white;
            reportText->append(QString("<tr bgcolor='%6'><td>%1</td><td>%2</td><td align='right'>$%3</td><td align='center'>%4</td></tr>")
                              .arg(tenant.name.c_str())
                              .arg(propAddress.c_str())
                              .arg(tenant.balance, 0, 'f', 2)
                              .arg(daysOverdue)
                              .arg(color.name()));
        }
    }
    reportText->append("</table>");
}

void RentbookApp::saveData() {
    QString filename = QFileDialog::getSaveFileName(this, "Save Data", "", "Rentbook Files (*.rbk)");
    if (!filename.isEmpty()) {
        system->saveData(filename.toStdString());
        statusBar()->showMessage("Data saved successfully", 3000);
    } else {
        QMessageBox::warning(this, "Save Error", "No file selected!");
    }
}

void RentbookApp::loadData() {
    QString filename = QFileDialog::getOpenFileName(this, "Load Data", "", "Rentbook Files (*.rbk)");
    if (!filename.isEmpty()) {
        system->loadData(filename.toStdString());
        refreshCurrentTab(tabWidget->currentIndex());
        statusBar()->showMessage("Data loaded successfully", 3000);
    } else {
        QMessageBox::warning(this, "Load Error", "No file selected!");
    }
}

void RentbookApp::about() {
    QMessageBox::about(this, "About RentBook",
                      "<b>RentBook Property Management</b><br>"
                      "Version 2.1<br>"
                      "A complete solution for tracking rental properties, tenants, "
                      "leases, and payments.<br><br>"
                      "Copyright © 2023 RentBook Inc.");
}

void RentbookApp::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    QAction *saveAction = new QAction("&Save Data", this);
    connect(saveAction, &QAction::triggered, this, &RentbookApp::saveData);
    fileMenu->addAction(saveAction);
    
    QAction *loadAction = new QAction("&Load Data", this);
    connect(loadAction, &QAction::triggered, this, &RentbookApp::loadData);
    fileMenu->addAction(loadAction);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = new QAction("E&xit", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, &RentbookApp::about);
    helpMenu->addAction(aboutAction);
}

void RentbookApp::createTabs() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    
    // Properties Tab
    QWidget *propertiesTab = new QWidget;
    QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesTab);
    propertyTable = new QTableWidget;
    propertyTable->setColumnCount(5);
    propertyTable->setHorizontalHeaderLabels({"ID", "Address", "Rent", "Description", "Status"});
    propertyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    propertyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    propertyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QPushButton *addPropertyBtn = new QPushButton("Add Property");
    connect(addPropertyBtn, &QPushButton::clicked, this, &RentbookApp::addProperty);
    propertiesLayout->addWidget(propertyTable);
    propertiesLayout->addWidget(addPropertyBtn);
    
    // Tenants Tab
    QWidget *tenantsTab = new QWidget;
    QVBoxLayout *tenantsLayout = new QVBoxLayout(tenantsTab);
    tenantTable = new QTableWidget;
    tenantTable->setColumnCount(6);
    tenantTable->setHorizontalHeaderLabels({"ID", "Name", "Contact", "Email", "Property ID", "Balance"});
    tenantTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tenantTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    tenantTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QPushButton *addTenantBtn = new QPushButton("Add Tenant");
    connect(addTenantBtn, &QPushButton::clicked, this, &RentbookApp::addTenant);
    tenantsLayout->addWidget(tenantTable);
    tenantsLayout->addWidget(addTenantBtn);
    
    // Leases Tab
    QWidget *leasesTab = new QWidget;
    QVBoxLayout *leasesLayout = new QVBoxLayout(leasesTab);
    leaseTable = new QTableWidget;
    leaseTable->setColumnCount(8);
    leaseTable->setHorizontalHeaderLabels({"ID", "Tenant", "Property", "Start", "End", "Rent", "Due Day", "Status"});
    leaseTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    leaseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    leaseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QPushButton *addLeaseBtn = new QPushButton("Add Lease Agreement");
    connect(addLeaseBtn, &QPushButton::clicked, this, &RentbookApp::addLease);
    leasesLayout->addWidget(leaseTable);
    leasesLayout->addWidget(addLeaseBtn);
    
    // Payments Tab
    QWidget *paymentsTab = new QWidget;
    QVBoxLayout *paymentsLayout = new QVBoxLayout(paymentsTab);
    paymentTable = new QTableWidget;
    paymentTable->setColumnCount(6);
    paymentTable->setHorizontalHeaderLabels({"ID", "Date", "Tenant", "Property", "Amount", "Notes"});
    paymentTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    paymentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    paymentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QPushButton *addPaymentBtn = new QPushButton("Record Payment");
    connect(addPaymentBtn, &QPushButton::clicked, this, &RentbookApp::recordPayment);
    paymentsLayout->addWidget(paymentTable);
    paymentsLayout->addWidget(addPaymentBtn);
    
    // Reports Tab
    QWidget *reportsTab = new QWidget;
    QVBoxLayout *reportsLayout = new QVBoxLayout(reportsTab);
    reportText = new QTextEdit;
    reportText->setReadOnly(true);
    chartView = new QtCharts::QChartView;
    chartView->setRenderHint(QPainter::Antialiasing);
    QPushButton *generateReportBtn = new QPushButton("Generate Report");
    connect(generateReportBtn, &QPushButton::clicked, this, &RentbookApp::generateReport);
    reportsLayout->addWidget(generateReportBtn);
    reportsLayout->addWidget(reportText, 2);
    reportsLayout->addWidget(chartView, 3);
    
    tabWidget->addTab(propertiesTab, "Properties");
    tabWidget->addTab(tenantsTab, "Tenants");
    tabWidget->addTab(leasesTab, "Leases");
    tabWidget->addTab(paymentsTab, "Payments");
    tabWidget->addTab(reportsTab, "Reports");
}

void RentbookApp::createStatusBar() {
    statusBar()->showMessage("Ready");
    
    QProgressBar *progressBar = new QProgressBar;
    progressBar->setRange(0, 100);
    progressBar->setValue(100);
    progressBar->setTextVisible(false);
    progressBar->setMaximumWidth(100);
    statusBar()->addPermanentWidget(progressBar);
}

void RentbookApp::autoLoad() {
    if (QFile::exists(dataFilePath)) {
        system->loadData(dataFilePath.toStdString());
        refreshCurrentTab(tabWidget->currentIndex());
        statusBar()->showMessage("Data loaded automatically", 3000);
    }
}

void RentbookApp::autoSave() {
    system->saveData(dataFilePath.toStdString());
}

void RentbookApp::refreshProperties() {
    propertyTable->setRowCount(0);
    for (const auto& prop : system->properties) {
        int row = propertyTable->rowCount();
        propertyTable->insertRow(row);
        propertyTable->setItem(row, 0, new QTableWidgetItem(QString::number(prop.id)));
        propertyTable->setItem(row, 1, new QTableWidgetItem(prop.address.c_str()));
        propertyTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(prop.monthlyRent, 0, 'f', 2)));
        propertyTable->setItem(row, 3, new QTableWidgetItem(prop.description.c_str()));
        propertyTable->setItem(row, 4, new QTableWidgetItem(prop.occupied ? "Occupied" : "Vacant"));
        
        QColor statusColor = prop.occupied ? QColor(200, 255, 200) : QColor(255, 230, 230);
        propertyTable->item(row, 4)->setBackground(QBrush(statusColor));
    }
}

void RentbookApp::refreshTenants() {
    tenantTable->setRowCount(0);
    for (const auto& tenant : system->tenants) {
        int row = tenantTable->rowCount();
        tenantTable->insertRow(row);
        tenantTable->setItem(row, 0, new QTableWidgetItem(QString::number(tenant.id)));
        tenantTable->setItem(row, 1, new QTableWidgetItem(tenant.name.c_str()));
        tenantTable->setItem(row, 2, new QTableWidgetItem(tenant.contact.c_str()));
        tenantTable->setItem(row, 3, new QTableWidgetItem(tenant.email.c_str()));
        tenantTable->setItem(row, 4, new QTableWidgetItem(QString::number(tenant.propertyId)));
        tenantTable->setItem(row, 5, new QTableWidgetItem(QString("$%1").arg(tenant.balance, 0, 'f', 2)));
        
        if (tenant.balance > 0) {
            tenantTable->item(row, 5)->setForeground(QBrush(Qt::red));
            tenantTable->item(row, 5)->setToolTip("Balance overdue");
        } else {
            tenantTable->item(row, 5)->setForeground(Qt::darkGreen);
        }
    }
}

void RentbookApp::refreshLeases() {
    leaseTable->setRowCount(0);
    std::string today = RentbookSystem::getCurrentDate();
    
    for (const auto& lease : system->leases) {
        int row = leaseTable->rowCount();
        leaseTable->insertRow(row);
        
        string tenantName = "Unknown";
        for (const auto& tenant : system->tenants) {
            if (tenant.id == lease.tenantId) {
                tenantName = tenant.name;
                break;
            }
        }
        
        string propAddress = "Unknown";
        for (const auto& prop : system->properties) {
            if (prop.id == lease.propertyId) {
                propAddress = prop.address;
                break;
            }
        }
        
        leaseTable->setItem(row, 0, new QTableWidgetItem(QString::number(lease.id)));
        leaseTable->setItem(row, 1, new QTableWidgetItem(tenantName.c_str()));
        leaseTable->setItem(row, 2, new QTableWidgetItem(propAddress.c_str()));
        leaseTable->setItem(row, 3, new QTableWidgetItem(lease.startDate.c_str()));
        leaseTable->setItem(row, 4, new QTableWidgetItem(lease.endDate.c_str()));
        leaseTable->setItem(row, 5, new QTableWidgetItem(QString("$%1").arg(lease.monthlyRent, 0, 'f', 2)));
        leaseTable->setItem(row, 6, new QTableWidgetItem(lease.paymentDueDay.c_str()));
        
        // Add lease status
        std::string status = "Active";
        QColor color = Qt::white;
        if (today > lease.endDate) {
            status = "Expired";
            color = QColor(255, 200, 200);
        } else if (lease.startDate > today) {
            status = "Pending";
            color = QColor(200, 200, 255);
        }
        leaseTable->setItem(row, 7, new QTableWidgetItem(status.c_str()));
        leaseTable->item(row, 7)->setBackground(QBrush(color));
    }
}

void RentbookApp::refreshPayments() {
    paymentTable->setRowCount(0);
    for (const auto& payment : system->payments) {
        int row = paymentTable->rowCount();
        paymentTable->insertRow(row);
        
        string tenantName = "Unknown";
        string propAddress = "Unknown";
        for (const auto& tenant : system->tenants) {
            if (tenant.id == payment.tenantId) {
                tenantName = tenant.name;
                
                for (const auto& prop : system->properties) {
                    if (prop.id == tenant.propertyId) {
                        propAddress = prop.address;
                        break;
                    }
                }
                break;
            }
        }
        
        paymentTable->setItem(row, 0, new QTableWidgetItem(QString::number(payment.id)));
        paymentTable->setItem(row, 1, new QTableWidgetItem(payment.date.c_str()));
        paymentTable->setItem(row, 2, new QTableWidgetItem(tenantName.c_str()));
        paymentTable->setItem(row, 3, new QTableWidgetItem(propAddress.c_str()));
        paymentTable->setItem(row, 4, new QTableWidgetItem(QString("$%1").arg(payment.amount, 0, 'f', 2)));
        paymentTable->setItem(row, 5, new QTableWidgetItem(payment.notes.c_str()));
    }
}

void RentbookApp::refreshReports() {
    generateReport();
}