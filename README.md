# RentBook GUI-Version (Qt6)
# RentBook Property Management System  

A comprehensive desktop application for property managers and landlords to track rental properties, tenants, leases, and payments. Built with C++ and Qt for performance and cross-platform compatibility.

---

## Features ✨

- **Property Management**  
  Track addresses, rental rates, descriptions, and occupancy status  
- **Tenant Management**  
  Maintain tenant profiles with contact details and balance tracking  
- **Lease Agreements**  
  Create lease contracts with start/end dates, terms, and payment due days  
- **Payment Processing**  
  Record payments and automatically update tenant balances  
- **Financial Reporting**  
  Generate reports with charts showing rents, collections, and overdue payments  
- **Data Persistence**  
  Automatic saving to user's Documents folder with manual import/export  
- **Visual Indicators**  
  Color-coded statuses for quick property and lease status recognition  

---

## Requirements 📋

### Development Environment
- C++17 compatible compiler (GCC, Clang, or MSVC)  
- Qt 6.2 or higher  
- Qt Charts module  
- CMake 3.16+ (optional)  

### Runtime Environment
- Windows, macOS, or Linux  
- 4GB RAM minimum  
- 200MB disk space  

---

## Installation & Setup 🛠️

### Windows
1. Install [Qt 6.2+](https://www.qt.io/download) with Qt Charts module  
2. Install [MinGW-w64](https://www.mingw-w64.org/) or MSVC compiler  
3. Clone repository:
   ```bash
   git clone https://github.com/yourusername/rentbook-system.git
```

4. Open `.pro` file in Qt Creator
5. Build and run

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt install build-essential qt6-base-dev qt6-charts-dev

# Build project
git clone https://github.com/yourusername/rentbook-system.git
cd rentbook-system
qmake
make
./RentBook
```

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Qt
brew install qt

# Build project
git clone https://github.com/yourusername/rentbook-system.git
cd rentbook-system
/opt/homebrew/bin/qmake6
make
open RentBook.app
```

---

## How to Use 🖥️

### Getting Started

1. **Add Properties**: Navigate to Properties tab → "Add Property"
2. **Add Tenants**: Go to Tenants tab → "Add Tenant"
3. **Create Leases**: In Leases tab → "Add Lease Agreement"
4. **Record Payments**: In Payments tab → "Record Payment"

### Key Features

* **Automatic Saving**: Data is automatically saved to `Documents/rentbook_data.rbk`
* **Financial Reports**:

  * Generate reports showing property performance
  * Visualize data with bar charts
  * Identify overdue payments with days-overdue calculation
* **Balance Calculation**:

  * Based on lease duration, rent, and payment history
* **Status Indicators**:

  * Properties: `Green` = Occupied, `Red` = Vacant
  * Leases: `Blue` = Pending, `White` = Active, `Red` = Expired
  * Balances: `Red` = Overdue, `Green` = Paid

### File Management

* **Manual Save**: File → Save Data (exports to `.rbk` file)
* **Load Data**: File → Load Data (imports from `.rbk` file)
* **Data Format** (pipe-separated):

  ```
  [Properties]
  1|123 Main St|1500.00|2-bedroom apartment|1
  [Tenants]
  1|John Doe|555-1234|john@email.com|1|0.00|2023-06-15
  ```

---

## Building from Source 🔧

### With Qt Creator

1. Open `RentBook.pro` in Qt Creator
2. Select kit (Desktop Qt 6.x.x)
3. Click Build → Run

### Command Line

```bash
# Using qmake
qmake RentBook.pro
make

# Using CMake
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Project Structure 📂

```
RentBook/
├── src/
│   ├── RentbookApp.cpp       # Main application logic
│   ├── RentbookApp.h         # Application header
│   ├── RentbookSystem.cpp    # Core business logic
│   ├── RentbookSystem.h      # System header
│   └── main.cpp              # Entry point
├── ui/                       # UI design files (if using Qt Designer)
├── data/                     # Sample data files
├── CMakeLists.txt            # CMake build configuration
├── RentBook.pro              # Qt project file
└── README.md                 # This documentation
```

---

## Troubleshooting ⚠️

**Q: I get "module QtCharts not found" error**
A: Install Qt Charts module through Qt Maintenance Tool

**Q: Payments aren't updating balances correctly**
A: Ensure you've created a lease agreement for the tenant first

**Q: Report shows incorrect overdue days**
A: Verify tenant's last payment date format is `YYYY-MM-DD`

**Q: Application crashes on startup**
A: Delete `Documents/rentbook_data.rbk` and restart

---

## License 📄

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Contributions welcome!**
If you'd like to improve RentBook, please fork the repository and submit a pull request.

```

Let me know if you want to generate a table of contents or add badges (e.g., build status, license, platforms).
```

