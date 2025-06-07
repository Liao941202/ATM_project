#include <iostream>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <conio.h> // Windows 用於密碼隱藏
#include <cctype>  // 檢查數字
#include <set>     // 儲存凍結帳戶
#include "Account.h"

using namespace std;

// 清除輸入緩衝區，處理無效輸入
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// 檢查密碼是否僅包含數字
bool isNumericPassword(const string& pwd) {
    if (pwd.empty()) return false;
    for (char c : pwd) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// 隱藏密碼輸入，顯示 *，支援退格鍵
string getHiddenPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') { // 按 Enter 結束輸入
        if (ch == '\b' && !password.empty()) { // 退格鍵
            password.pop_back();
            cout << "\b \b"; // 擦除最後一個 *
        } else if (ch >= 32 && ch <= 126) { // 可打印字符
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

/* Linux 替代方案（註解，因需跨平台請選擇使用）：
#include <termios.h>
#include <unistd.h>
string getHiddenPassword() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    string password;
    char ch;
    while ((ch = getchar()) != '\n') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return password;
}
*/

// 生成 6 位隨機驗證碼
string generateVerificationCode() {
    srand(time(nullptr));
    string code = "";
    for (int i = 0; i < 6; ++i) {
        code += to_string(rand() % 10);
    }
    return code;
}

// 將密碼轉為等長 * 字串
string maskPassword(const string& pwd) {
    return string(pwd.length(), '*');
}

// 記錄操作到 desktop.txt，密碼顯示為 *
void logAction(const string& username, const string& action, const string& details) {
    ofstream outFile("./desktop.txt", ios::app);
    if (outFile.is_open()) {
        time_t now = time(nullptr);
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
        string logDetails = (action == "create") ? maskPassword(details) : details;
        outFile << "[" << timeStr << "] " << username << " " << action << " " << logDetails << endl;
        outFile.close();
    } else {
        cout << "錯誤：無法寫入 desktop.txt，請檢查檔案權限或路徑。" << endl;
    }
}

// 從 desktop.txt 匯入歷史記錄
void loadHistory(vector<Account*>& accounts, set<string>& frozenAccounts) {
    ifstream inFile("./desktop.txt");
    if (!inFile.is_open()) {
        cout << "desktop.txt 不存在，將創建新檔案。" << endl;
        return;
    }

    // 第一次掃描：僅處理創建帳戶
    string line;
    int lineNumber = 0;
    vector<string> lines;
    while (getline(inFile, line)) {
        lineNumber++;
        if (line.empty()) continue;
        lines.push_back(line);
        stringstream ss(line);
        string timestamp, username, action;
        ss >> timestamp >> username >> action;

        if (action == "create") {
            string password;
            getline(ss >> ws, password);
            bool exists = false;
            for (Account* acc : accounts) {
                if (acc->getUsername() == username) {
                    exists = true;
                    break;
                }
            }
            if (!exists && frozenAccounts.find(username) == frozenAccounts.end()) {
                accounts.push_back(new Account(username, password));
            }
        }
    }
    inFile.close();

    // 第二次掃描：處理其他操作
    lineNumber = 0;
    for (const string& l : lines) {
        lineNumber++;
        stringstream ss(l);
        string timestamp, username, action, details;
        ss >> timestamp >> username >> action;
        getline(ss >> ws, details);

        Account* account = nullptr;
        for (Account* acc : accounts) {
            if (acc->getUsername() == username) {
                account = acc;
                break;
            }
        }
        if (!account && action != "create") {
            cout << "警告：第 " << lineNumber << " 行忽略，無效帳戶：" << username << endl;
            continue;
        }

        try {
            if (action == "deposit") {
                double amount = stod(details);
                if (account && !account->deposit(amount)) {
                    cout << "警告：第 " << lineNumber << " 行存款無效：" << amount << endl;
                }
            } else if (action == "withdraw") {
                double amount = stod(details);
                if (account && !account->withdraw(amount)) {
                    cout << "警告：第 " << lineNumber << " 行取出無效：" << amount << endl;
                }
            } else if (action == "transfer") {
                string targetUsername;
                double amount;
                stringstream detailStream(details);
                detailStream >> targetUsername >> amount;
                if (detailStream.fail()) {
                    cout << "警告：第 " << lineNumber << " 行轉帳格式錯誤" << endl;
                    continue;
                }
                bool targetExists = false;
                for (Account* acc : accounts) {
                    if (acc->getUsername() == targetUsername) {
                        if (account && !account->transfer(*acc, amount)) {
                            cout << "警告：第 " << lineNumber << " 行轉帳無效：" << amount << endl;
                        }
                        targetExists = true;
                        break;
                    }
                }
                if (!targetExists) {
                    cout << "警告：第 " << lineNumber << " 行轉帳目標不存在：" << targetUsername << endl;
                }
            } else if (action == "fixed_deposit") {
                double amount;
                int duration;
                time_t start;
                stringstream detailStream(details);
                detailStream >> amount >> duration >> start;
                if (detailStream.fail() || amount <= 0 || duration <= 0) {
                    cout << "警告：第 " << lineNumber << " 行定存格式錯誤" << endl;
                    continue;
                }
                if (account) {
                    account->setFixedDeposit(amount, duration, start);
                }
            } else if (action == "loan") {
                double amount;
                int duration, monthsPaid;
                stringstream detailStream(details);
                detailStream >> amount >> duration >> monthsPaid;
                if (detailStream.fail() || amount <= 0 || duration <= 0 || monthsPaid < 0) {
                    cout << "警告：第 " << lineNumber << " 行貸款格式錯誤" << endl;
                    continue;
                }
                if (account) {
                    account->setLoan(amount, duration, monthsPaid);
                }
            } else if (action == "repay") {
                double amount = stod(details);
                if (account && !account->repayLoan(amount)) {
                    cout << "警告：第 " << lineNumber << " 行還款無效：" << amount << endl;
                }
            } else if (action == "gamble") {
                double amount;
                bool won;
                stringstream detailStream(details);
                detailStream >> amount >> won;
                if (detailStream.fail() || amount <= 0) {
                    cout << "警告：第 " << lineNumber << " 行賭博格式錯誤" << endl;
                    continue;
                }
                if (account) {
                    account->setBalance(account->getBalance() - amount);
                    if (won) {
                        account->setBalance(account->getBalance() + amount * 2);
                    }
                }
            }
        } catch (const exception& e) {
            cout << "錯誤：第 " << lineNumber << " 行解析失敗：" << l << endl;
        }
    }
    cout << "歷史記錄匯入完成，載入 " << accounts.size() << " 個帳戶。" << endl;
}

int main() {
    // 清空 desktop.txt
    ofstream clearFile("./desktop.txt", ios::trunc);
    if (!clearFile.is_open()) {
        cout << "錯誤：無法清空 desktop.txt，請檢查檔案權限或路徑。" << endl;
    }
    clearFile.close();

    vector<Account*> accounts;
    set<string> frozenAccounts; // 儲存凍結帳戶名稱
    Account* current = nullptr;
    int choice;

    // 匯入歷史記錄（因清空，此步應無記錄，除非外部修改）
    loadHistory(accounts, frozenAccounts);

    // 顯示歡迎訊息
    cout << "歡迎使用 ATM 系統！" << endl;

    while (true) {
        cout << "\n--- ATM 系統 ---" << endl;
        if (current == nullptr) {
            cout << "1. 創建新用戶" << endl;
            cout << "2. 登入" << endl;
        } else {
            cout << "1. 存錢" << endl;
            cout << "2. 取錢" << endl;
            cout << "3. 轉帳" << endl;
            cout << "4. 定存" << endl;
            cout << "5. 查詢定存" << endl;
            cout << "6. 申請貸款" << endl;
            cout << "7. 還款" << endl;
            cout << "8. 查詢貸款狀態" << endl;
            cout << "9. 賭博" << endl;
            cout << "10. 登出" << endl;
        }
        cout << "請輸入您的選擇：";

        if (!(cin >> choice)) {
            cout << "無效輸入，請輸入數字。" << endl;
            clearInputBuffer();
            continue;
        }

        if (current == nullptr) {
            if (choice == 1) { // 創建新用戶
                string username, password;
                cout << "請輸入用戶名稱：";
                clearInputBuffer();
                getline(cin, username);

                // 檢查是否凍結
                if (frozenAccounts.find(username) != frozenAccounts.end()) {
                    cout << "此帳戶已被凍結，無法創建。" << endl;
                    continue;
                }

                cout << "請輸入密碼（僅限數字）：";
                password = getHiddenPassword();
                while (!isNumericPassword(password)) {
                    cout << "密碼只能包含數字，請重新輸入：";
                    password = getHiddenPassword();
                }

                bool exists = false;
                for (Account* acc : accounts) {
                    if (acc->getUsername() == username) {
                        exists = true;
                        break;
                    }
                }
                if (exists) {
                    cout << "用戶名稱已存在，請選擇其他名稱。" << endl;
                } else {
                    Account* newAccount = new Account(username, password);
                    accounts.push_back(newAccount);
                    cout << "用戶創建成功！" << endl;
                    logAction(username, "create", password);
                }
            } else if (choice == 2) { // 登入
                string username, password, inputCode;
                cout << "請輸入用戶名稱：";
                clearInputBuffer();
                getline(cin, username);

                // 檢查是否凍結
                if (frozenAccounts.find(username) != frozenAccounts.end()) {
                    cout << "帳戶已被凍結： " << username << endl;
                    continue;
                }

                bool found = false;
                Account* tempAccount = nullptr;
                for (Account* acc : accounts) {
                    if (acc->getUsername() == username) {
                        tempAccount = acc;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "用戶名稱不存在。" << endl;
                    continue;
                }

                // 最多三次嘗試
                int attempts = 0;
                bool loginSuccess = false;
                while (attempts < 3 && !loginSuccess) {
                    attempts++;
                    cout << "請輸入密碼（僅限數字，剩餘嘗試次數：" << (3 - attempts + 1) << "）：";
                    password = getHiddenPassword();
                    while (!isNumericPassword(password)) {
                        cout << "密碼只能包含數字，請重新輸入（剩餘嘗試次數：" << (3 - attempts + 1) << "）：";
                        password = getHiddenPassword();
                    }

                    if (!tempAccount->checkPassword(password)) {
                        cout << "密碼錯誤。" << endl;
                        if (attempts == 3) {
                            cout << "帳戶已被凍結： " << username << endl;
                            frozenAccounts.insert(username);
                            break;
                        }
                        continue;
                    }

                    // 模擬發送驗證碼
                    string verificationCode = generateVerificationCode();
                    cout << "已發送驗證碼至您的設備：" << verificationCode << endl; // 模擬顯示驗證碼
                    cout << "請輸入驗證碼：";
                    getline(cin, inputCode);

                    if (inputCode == verificationCode) {
                        current = tempAccount;
                        cout << "驗證成功！登入成功！歡迎，" << current->getUsername() << "！" << endl;
                        logAction(username, "login", "");
                        loginSuccess = true;
                    } else {
                        cout << "驗證碼錯誤。" << endl;
                        if (attempts == 3) {
                            cout << "帳戶已被凍結： " << username << endl;
                            frozenAccounts.insert(username);
                            break;
                        }
                    }
                }
            } else {
                cout << "無效選項，請重新選擇。" << endl;
            }
        } else {
            if (choice == 1) { // 存錢
                double amount;
                cout << "請輸入存款金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                if (current->deposit(amount)) {
                    cout << "存款成功！目前餘額：$" << current->getBalance() << endl;
                    logAction(current->getUsername(), "deposit", to_string(amount));
                } else {
                    cout << "存款失敗：金額必須為正數。" << endl;
                }
            } else if (choice == 2) { // 取錢
                double amount;
                cout << "請輸入取款金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                if (current->withdraw(amount)) {
                    cout << "取款成功！目前餘額：$" << current->getBalance() << endl;
                    logAction(current->getUsername(), "withdraw", to_string(amount));
                } else {
                    cout << "取款失敗：金額無效或餘額不足。" << endl;
                }
            } else if (choice == 3) { // 轉帳
                string targetUsername;
                double amount;
                cout << "請輸入目標用戶名稱：";
                clearInputBuffer();
                getline(cin, targetUsername);
                // 檢查目標帳戶是否凍結
            if (frozenAccounts.find(targetUsername) != frozenAccounts.end()) {
                cout << "目標帳戶已被凍結，無法轉帳。" << endl;
                continue;
            }
                cout << "請輸入轉帳金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                bool targetExists = false;
                Account* targetAccount = nullptr;
                for (Account* acc : accounts) {
                    if (acc->getUsername() == targetUsername) {
                        targetAccount = acc;
                        targetExists = true;
                        break;
                    }
                }
                if (!targetExists) {
                    cout << "目標用戶不存在。" << endl;
                } else if (targetAccount == current) {
                    cout << "不能轉帳給自己。" << endl;
                } else if (current->transfer(*targetAccount, amount)) {
                    cout << "轉帳成功！目前餘額：$" << current->getBalance() << endl;
                    logAction(current->getUsername(), "transfer", targetUsername + " " + to_string(amount));
                } else {
                    cout << "轉帳失敗：金額無效或餘額不足。" << endl;
                }
            } else if (choice == 4) { // 定存
                double amount;
                int duration;
                cout << "請輸入定存金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                cout << "請輸入定存期限（月）：";
                if (!(cin >> duration)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                if (current->startFixedDeposit(amount, duration)) {
                    cout << "定存成功！金額 $" << amount << " 已存入，期限 " << duration << " 個月。" << endl;
                    logAction(current->getUsername(), "fixed_deposit", to_string(amount) + " " + to_string(duration) + " " + to_string(time(nullptr)));
                } else {
                    cout << "定存失敗：金額無效、餘額不足或期限無效。" << endl;
                }
            } else if (choice == 5) { // 查詢定存
                double amount, totalAssets;
                int duration;
                current->checkFixedDeposit(amount, duration, totalAssets);
                if (amount == 0) {
                    cout << "目前無定存，總資產：$" << totalAssets << endl;
                } else {
                    cout << "定存金額：$" << amount << "，期限：" << duration << " 個月，總資產：$" << totalAssets << endl;
                }
            } else if (choice == 6) { // 申請貸款
                double amount;
                int duration;
                cout << "請輸入貸款金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                cout << "請輸入貸款期限（月）：";
                if (!(cin >> duration)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                if (current->applyLoan(amount, duration)) {
                    cout << "貸款申請成功！金額 $" << amount << " 已存入帳戶，期限 " << duration << " 個月。" << endl;
                    logAction(current->getUsername(), "loan", to_string(amount) + " " + to_string(duration) + " 0");
                } else {
                    cout << "貸款申請失敗：金額無效、期限無效或已有未結清貸款。" << endl;
                }
            } else if (choice == 7) { // 還款
                double amount;
                cout << "請輸入還款金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                if (current->repayLoan(amount)) {
                    cout << "還款成功！剩餘貸款金額：$" << current->getLoanStatus() << endl;
                    logAction(current->getUsername(), "repay", to_string(amount));
                } else {
                    cout << "還款失敗：金額無效、餘額不足或無貸款。" << endl;
                }
            } else if (choice == 8) { // 查詢貸款狀態
                double remainingLoan = current->getLoanStatus();
                if (remainingLoan == 0) {
                    cout << "目前無貸款。" << endl;
                } else {
                    cout << "剩餘貸款金額：$" << remainingLoan << endl;
                }
            } else if (choice == 9) { // 賭博
                double amount;
                cout << "請輸入投注金額：";
                if (!(cin >> amount)) {
                    cout << "無效輸入，請輸入數字。" << endl;
                    clearInputBuffer();
                    continue;
                }
                bool won = current->gamble(amount);
                if (won) {
                    cout << "恭喜！你贏得了雙倍金額！目前餘額：$" << current->getBalance() << endl;
                } else {
                    cout << "很遺憾，你輸了！目前餘額：$" << current->getBalance() << endl;
                }
                logAction(current->getUsername(), "gamble", to_string(amount) + " " + to_string(won));
            } else if (choice == 10) { // 登出
                cout << "感謝使用 ATM 系統，再見！" << endl;
                cout << "已登出，用戶：" << current->getUsername() << endl;
                logAction(current->getUsername(), "logout", "");
                current = nullptr;
            } else {
                cout << "無效選項，請重新選擇。" << endl;
            }
        }
    }

    // 清理動態分配的帳戶
    for (Account* acc : accounts) {
        delete acc;
    }

    return 0;
}