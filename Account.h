#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <ctime>
using namespace std;

class Account {
private:
    string username;   // 用戶名稱，儲存帳戶的唯一識別
    string password;   // 密碼，用於登入驗證
    double balance;    // 餘額，儲存帳戶的當前金額
    double fixedDeposit; // 定存金額
    time_t fixedDepositStart; // 定存開始時間
    int fixedDepositDuration; // 定存期限（月）
    double loanAmount; // 貸款金額
    double loanInterestRate; // 貸款利率
    int loanDuration; // 貸款期限（月）
    int loanMonthsPaid; // 已還款的月份數

public:
    // 建構子，初始化用戶名稱和密碼
    Account(string uname, string pwd);
    
    // 取得用戶名稱，const 確保不修改物件
    string getUsername() const;
    
    // 驗證密碼是否正確
    bool checkPassword(string pwd) const;
    
    // 取得當前餘額
    double getBalance() const;
    
    // 存錢，檢查金額有效性
    bool deposit(double amount);
    
    // 取錢，檢查金額和餘額
    bool withdraw(double amount);
    
    // 轉帳，檢查金額和餘額
    bool transfer(Account& target, double amount);
    
    // 開始定存，檢查金額和期限
    bool startFixedDeposit(double amount, int duration);
    
    // 檢查定存狀態，返回詳細資訊和總資產
    void checkFixedDeposit(double& amount, int& duration, double& totalAssets) const;
    
    // 申請貸款，設定貸款金額和期限
    bool applyLoan(double amount, int duration);
    
    // 還款，檢查金額
    bool repayLoan(double amount);
    
    // 查詢貸款狀態
    double getLoanStatus() const;
    
    // 賭博功能，隨機決定輸贏
    bool gamble(double amount);
    
    // 設置帳戶狀態（用於歷史紀錄匯入）
    void setBalance(double amount);
    void setFixedDeposit(double amount, int duration, time_t start);
    void setLoan(double amount, int duration, int monthsPaid);
};

#endif