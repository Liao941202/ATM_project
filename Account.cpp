#include "Account.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

// 建構子，初始化用戶名稱、密碼和餘額
Account::Account(string uname, string pwd) {
    username = uname;
    password = pwd;
    balance = 0.0;
    fixedDeposit = 0.0;
    fixedDepositStart = 0;
    fixedDepositDuration = 0;
    loanAmount = 0.0;
    loanInterestRate = 0.0;
    loanDuration = 0;
    loanMonthsPaid = 0;
}

// 返回用戶名稱
string Account::getUsername() const {
    return username;
}

// 驗證密碼是否匹配
bool Account::checkPassword(string pwd) const {
    return password == pwd;
}

// 返回當前餘額
double Account::getBalance() const {
    return balance;
}

// 存錢，檢查金額是否有效
bool Account::deposit(double amount) {
    if (amount <= 0) {
        return false;
    }
    balance += amount;
    return true;
}

// 取錢，檢查金額是否有效且餘額足夠
bool Account::withdraw(double amount) {
    if (amount <= 0 || amount > balance) {
        return false;
    }
    balance -= amount;
    return true;
}

// 轉帳，檢查金額和餘額
bool Account::transfer(Account& target, double amount) {
    if (amount <= 0 || amount > balance) {
        return false;
    }
    balance -= amount;
    target.deposit(amount);
    return true;
}

// 開始定存，檢查金額和期限
bool Account::startFixedDeposit(double amount, int duration) {
    if (amount <= 0 || amount > balance || duration <= 0) {
        return false;
    }
    balance -= amount;
    fixedDeposit = amount;
    fixedDepositStart = time(nullptr);
    fixedDepositDuration = duration;
    return true;
}

// 檢查定存狀態，返回存入金額、期限和總資產
void Account::checkFixedDeposit(double& amount, int& duration, double& totalAssets) const {
    amount = fixedDeposit;
    duration = fixedDepositDuration;
    totalAssets = balance;
    if (fixedDeposit == 0) {
        return;
    }
    time_t currentTime = time(nullptr);
    double monthsPassed = difftime(currentTime, fixedDepositStart) / (60 * 60 * 24 * 30.0);
    if (monthsPassed >= fixedDepositDuration) {
        double interest = fixedDeposit * 0.012 * fixedDepositDuration / 12.0; // 單利，年利率 1.2%
        totalAssets += fixedDeposit + interest;
    } else {
        totalAssets += fixedDeposit;
    }
}

// 申請貸款，設定貸款金額和期限
bool Account::applyLoan(double amount, int duration) {
    if (amount <= 0 || duration <= 0 || loanAmount > 0) {
        return false;
    }
    loanAmount = amount;
    loanInterestRate = 0.20; // 年利率 20%
    loanDuration = duration;
    loanMonthsPaid = 0;
    balance += amount;
    return true;
}

// 還款，檢查金額，剩餘金額無條件進位
bool Account::repayLoan(double amount) {
    if (amount <= 0 || amount > balance || loanAmount == 0) {
        return false;
    }
    double totalLoanWithInterest = loanAmount * pow(1 + loanInterestRate / 12.0, loanDuration); // 複利計算
    double remainingLoan = totalLoanWithInterest - (loanMonthsPaid * totalLoanWithInterest / loanDuration);
    if (amount > remainingLoan) {
        amount = remainingLoan;
    }
    balance -= amount;
    loanMonthsPaid += ceil(amount / (totalLoanWithInterest / loanDuration)); // 無條件進位
    if (loanMonthsPaid >= loanDuration) {
        loanAmount = 0;
        loanInterestRate = 0;
        loanDuration = 0;
        loanMonthsPaid = 0;
    }
    return true;
}

// 查詢貸款狀態
double Account::getLoanStatus() const {
    if (loanAmount == 0) {
        return 0.0;
    }
    double totalLoanWithInterest = loanAmount * pow(1 + loanInterestRate / 12.0, loanDuration); // 複利計算
    double remainingLoan = totalLoanWithInterest - (loanMonthsPaid * totalLoanWithInterest / loanDuration);
    return ceil(remainingLoan); // 無條件進位
}

// 賭博功能，隨機決定輸贏
bool Account::gamble(double amount) {
    if (amount <= 0 || amount > balance) {
        return false;
    }
    balance -= amount;
    srand(time(nullptr));
    if (rand() % 2 == 0) { // 50% 機率贏
        balance += amount * 2;
        return true;
    }
    return false;
}

// 設置帳戶餘額（用於歷史紀錄）
void Account::setBalance(double amount) {
    balance = amount;
}

// 設置定存狀態（用於歷史紀錄）
void Account::setFixedDeposit(double amount, int duration, time_t start) {
    fixedDeposit = amount;
    fixedDepositDuration = duration;
    fixedDepositStart = start;
}

// 設置貸款狀態（用於歷史紀錄）
void Account::setLoan(double amount, int duration, int monthsPaid) {
    loanAmount = amount;
    loanInterestRate = 0.20;
    loanDuration = duration;
    loanMonthsPaid = monthsPaid;
}