#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include "RentbookSystem.h"

class RentbookApp : public QMainWindow {
    Q_OBJECT

public:
    RentbookApp(QWidget *parent = nullptr);
    ~RentbookApp();

private slots:
    void refreshCurrentTab(int index);
    void addProperty();
    void addTenant();
    void addLease();
    void recordPayment();
    void generateReport();
    void saveData();
    void loadData();
    void about();

private:
    void createMenu();
    void createTabs();
    void createStatusBar();
    void autoLoad();
    void autoSave();
    void refreshProperties();
    void refreshTenants();
    void refreshLeases();
    void refreshPayments();
    void refreshReports();

    RentbookSystem *system;
    QTabWidget *tabWidget;
    QTableWidget *propertyTable;
    QTableWidget *tenantTable;
    QTableWidget *leaseTable;
    QTableWidget *paymentTable;
    QTextEdit *reportText;
    QtCharts::QChartView *chartView;
    QString dataFilePath;
};