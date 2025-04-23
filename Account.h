#ifndef ACCOUNT_H // 防止頭檔重複包含
#define ACCOUNT_H

#include <string> // 包含 string 類以儲存用戶名稱和密碼
using namespace std; // 使用標準命名空間（僅限頭檔範圍）

class Account { // 定義 Account 類，用於管理用戶帳戶
private:
    string username;   // 用戶名稱，儲存帳戶的唯一識別
    string password;   // 密碼，用於登入驗證
    double balance;    // 餘額，儲存帳戶的當前金額

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
};

#endif // 結束頭檔保護