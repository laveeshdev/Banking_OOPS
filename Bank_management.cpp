#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <ctime>
using namespace std;

// Transaction class to hold transaction details
class Transaction {
private:
    string type;
    double amount;
    time_t timestamp;

public:
    Transaction(string t, double a) : type(t), amount(a) {
        timestamp = time(0);
    }

    void displayTransaction() const {
        cout << "Type: " << type << ", Amount: $" << amount << ", Date: " << ctime(&timestamp);
    }
};

// Base class: BankAccount
class BankAccount {
protected:
    string accountNumber;
    double balance;
    string password;
    vector<Transaction> transactions;

public:
    BankAccount(string accNum, double initialBalance, string pass)
        : accountNumber(accNum), balance(initialBalance), password(pass) {}

    virtual void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            transactions.push_back(Transaction("Deposit", amount));
            cout << "Deposited: $" << amount << endl;
        } else {
            cout << "Deposit amount must be positive." << endl;
        }
    }

    virtual void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            transactions.push_back(Transaction("Withdraw", amount));
            cout << "Withdrawn: $" << amount << endl;
        } else {
            cout << "Invalid withdrawal amount." << endl;
        }
    }

    double getBalance() const {
        return balance;
    }

    string getAccountNumber() const {
        return accountNumber;
    }

    bool verifyPassword(const string& pass) const {
        return pass == password;
    }

    void changePassword(const string& oldPass, const string& newPass) {
        if (verifyPassword(oldPass)) {
            password = newPass;
            cout << "Password changed successfully." << endl;
        } else {
            cout << "Incorrect old password." << endl;
        }
    }

    virtual void displayAccountInfo() const {
        cout << "Account Number: " << accountNumber << ", Balance: $" << balance << endl;
    }

    void displayStatement() const {
        cout << "Transaction History for Account: " << accountNumber << endl;
        for (const auto& transaction : transactions) {
            transaction.displayTransaction();
        }
        cout << endl;
    }

    virtual void applyMonthlyInterest() {}
};

// Derived class: SavingsAccount
class SavingsAccount : public BankAccount {
private:
    double interestRate;

public:
    SavingsAccount(string accNum, double initialBalance, double rate, string pass)
        : BankAccount(accNum, initialBalance, pass), interestRate(rate) {}

    void applyMonthlyInterest() override {
        double interest = balance * interestRate / 12;
        balance += interest;
        transactions.push_back(Transaction("Interest", interest));
        cout << "Monthly interest applied: $" << interest << endl;
    }

    void displayAccountInfo() const override {
        cout << "Savings Account - ";
        BankAccount::displayAccountInfo();
        cout << "Interest Rate: " << interestRate * 100 << "%" << endl;
    }
};

// Derived class: CurrentAccount
class CurrentAccount : public BankAccount {
private:
    double overdraftLimit;

public:
    CurrentAccount(string accNum, double initialBalance, double overdraft, string pass)
        : BankAccount(accNum, initialBalance, pass), overdraftLimit(overdraft) {}

    void withdraw(double amount) override {
        if (amount > 0 && amount <= balance + overdraftLimit) {
            balance -= amount;
            transactions.push_back(Transaction("Withdraw", amount));
            cout << "Withdrawn: $" << amount << endl;
        } else {
            cout << "Withdrawal exceeds overdraft limit." << endl;
        }
    }

    void displayAccountInfo() const override {
        cout << "Checking Account - ";
        BankAccount::displayAccountInfo();
        cout << "Overdraft Limit: $" << overdraftLimit << endl;
    }
};

class Customer {
private:
    string name;
    vector<BankAccount*> accounts;

public:
    Customer(string customerName) : name(customerName) {}

    void addAccount(BankAccount* account) {
        accounts.push_back(account);
    }

    void transferFunds(string fromAccNum, string toAccNum, double amount, const string& password) {
        BankAccount* fromAccount = nullptr;
        BankAccount* toAccount = nullptr;

        for (auto account : accounts) {
            if (account->getAccountNumber() == fromAccNum && account->verifyPassword(password)) {
                fromAccount = account;
            }
            if (account->getAccountNumber() == toAccNum) {
                toAccount = account;
            }
        }

        if (fromAccount && toAccount && amount > 0) {
            fromAccount->withdraw(amount);
            toAccount->deposit(amount);
            cout << "Transferred $" << amount << " from " << fromAccNum << " to " << toAccNum << endl;
        } else {
            cout << "Transfer failed. Check account numbers, password, and balance." << endl;
        }
    }

    void displayAllAccounts() const {
        cout << "Customer: " << name << endl;
        for (auto account : accounts) {
            account->displayAccountInfo();
            cout << endl;
        }
    }

    void applyMonthlyInterest() {
        for (auto account : accounts) {
            account->applyMonthlyInterest();
        }
    }

    void displayStatements() const {
        for (auto account : accounts) {
            account->displayStatement();
        }
    }

    const vector<BankAccount*>& getAccounts() const {
        return accounts;
    }

    vector<BankAccount*>& getAccounts() {
        return accounts;
    }

    ~Customer() {
        for (auto account : accounts) {
            delete account;
        }
    }
};
// Class: Admin
class Admin {
private:
    string adminName;
    string adminPassword;

public:
    Admin(string name, string password)
        : adminName(name), adminPassword(password) {}

    void closeAccount(Customer& customer, string accNum, const string& password) {
        if (adminPassword == password) {
            auto& accounts = customer.getAccounts();
            for (auto it = accounts.begin(); it != accounts.end(); ++it) {
                if ((*it)->getAccountNumber() == accNum) {
                    delete *it;
                    accounts.erase(it);
                    cout << "Account " << accNum << " closed successfully." << endl;
                    return;
                }
            }
            cout << "Account not found." << endl;
        } else {
            cout << "Admin password incorrect." << endl;
        }
    }

    void viewAllAccounts(const Customer& customer) const {
        customer.displayAllAccounts();
    }
};
#include <iostream>
#include <limits>


using namespace std;

void displayMenu() {
    cout << "Bank Account Management System" << endl;
    cout << "1. Create Savings Account" << endl;
    cout << "2. Create Checking Account" << endl;
    cout << "3. Deposit" << endl;
    cout << "4. Withdraw" << endl;
    cout << "5. Transfer Funds" << endl;
    cout << "6. Apply Monthly Interest" << endl;
    cout << "7. Display All Accounts" << endl;
    cout << "8. Display Account Statements" << endl;
    cout << "9. Close Account (Admin)" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter your choice: ";
}

int main() {
    Customer customer("Laveesh Sanadhya");
    Admin admin("BankAdmin", "admin_pass");

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1: {
                string accNum, password;
                double initialBalance, interestRate;
                cout << "Enter Savings Account Number: ";
                cin >> accNum;
                cout << "Enter Initial Balance: ";
                cin >> initialBalance;
                cout << "Enter Interest Rate (as a decimal): ";
                cin >> interestRate;
                cout << "Enter Password: ";
                cin >> password;
                BankAccount* savings = new SavingsAccount(accNum, initialBalance, interestRate, password);
                customer.addAccount(savings);
                break;
            }
            case 2: {
                string accNum, password;
                double initialBalance, overdraftLimit;
                cout << "Enter Checking Account Number: ";
                cin >> accNum;
                cout << "Enter Initial Balance: ";
                cin >> initialBalance;
                cout << "Enter Overdraft Limit: ";
                cin >> overdraftLimit;
                cout << "Enter Password: ";
                cin >> password;
                BankAccount* checking = new CurrentAccount(accNum, initialBalance, overdraftLimit, password);
                customer.addAccount(checking);
                break;
            }
            case 3: {
                string accNum;
                double amount;
                cout << "Enter Account Number: ";
                cin >> accNum;
                cout << "Enter Deposit Amount: ";
                cin >> amount;
                for (auto account : customer.getAccounts()) {
                    if (account->getAccountNumber() == accNum) {
                        account->deposit(amount);
                        break;
                    }
                }
                break;
            }
            case 4: {
                string accNum;
                double amount;
                cout << "Enter Account Number: ";
                cin >> accNum;
                cout << "Enter Withdrawal Amount: ";
                cin >> amount;
                for (auto account : customer.getAccounts()) {
                    if (account->getAccountNumber() == accNum) {
                        account->withdraw(amount);
                        break;
                    }
                }
                break;
            }
            case 5: {
                string fromAccNum, toAccNum, password;
                double amount;
                cout << "Enter From Account Number: ";
                cin >> fromAccNum;
                cout << "Enter To Account Number: ";
                cin >> toAccNum;
                cout << "Enter Transfer Amount: ";
                cin >> amount;
                cout << "Enter Password: ";
                cin >> password;
                customer.transferFunds(fromAccNum, toAccNum, amount, password);
                break;
            }
            case 6: {
                customer.applyMonthlyInterest();
                break;
            }
            case 7: {
                customer.displayAllAccounts();
                break;
            }
            case 8: {
                customer.displayStatements();
                break;
            }
            case 9: {
                string accNum, password;
                cout << "Enter Account Number to Close: ";
                cin >> accNum;
                cout << "Enter Admin Password: ";
                cin >> password;
                admin.closeAccount(customer, accNum, password);
                break;
            }
            case 0: {
                cout << "Exiting..." << endl;
                return 0;
            }
            default: {
                cout << "Invalid choice. Please try again." << endl;
                break;
            }
        }
    }

    return 0;
}