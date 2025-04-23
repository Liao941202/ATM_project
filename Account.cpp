#include "Account.h" // 包含 Account 類定義

// 建構子，初始化用戶名稱、密碼和餘額
Account::Account(string uname, string pwd) {
    username = uname; // 設置用戶名稱
    password = pwd; // 設置密碼
    balance = 0.0; // 初始餘額設為 0
}

// 返回用戶名稱
string Account::getUsername() const {
    return username; // 直接返回私有成員 username
}

// 驗證密碼是否匹配
bool Account::checkPassword(string pwd) const {
    return password == pwd; // 比較輸入密碼與儲存密碼
}

// 返回當前餘額
double Account::getBalance() const {
    return balance; // 直接返回私有成員 balance
}

// 存錢，檢查金額是否有效
bool Account::deposit(double amount) {
    if (amount <= 0) { // 如果金額小於等於 0
        return false; // 返回 false 表示存錢失敗
    }
    balance += amount; // 增加餘額
    return true; // 返回 true 表示存錢成功
}

// 取錢，檢查金額是否有效且餘額足夠
bool Account::withdraw(double amount) {
    if (amount <= 0 || amount > balance) { // 如果金額無效或超過餘額
        return false; // 返回 false 表示取錢失敗
    }
    balance -= amount; // 扣除餘額
    return true; // 返回 true 表示取錢成功
}