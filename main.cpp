#include <iostream> // 包含輸入輸出功能
#include <vector> // 包含 vector 類以儲存多個帳戶
#include <limits> // 包含 limits 用於清除輸入緩衝區
#include "Account.h" // 包含 Account 類定義

using namespace std; // 使用標準命名空間

// 清除輸入緩衝區，處理無效輸入
void clearInputBuffer() {
    cin.clear(); // 清除 cin 的錯誤狀態
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩餘輸入直到換行
}

int main() {
    vector<Account> accounts; // 儲存所有帳戶物件的向量
    Account* current = nullptr; // 指向當前登入帳戶的指標，未登入時為 nullptr
    int choice; // 儲存用戶的選單選擇

    while (true) { // 主迴圈，持續顯示選單直到退出
        cout << "\n--- ATM 系統 ---" << endl; // 顯示標題
        if (current == nullptr) { // 如果未登入
            cout << "1. 創建新用戶" << endl; // 選項：創建用戶
            cout << "2. 登入" << endl; // 選項：登入
            cout << "3. 退出" << endl; // 選項：退出程式
        } else { // 如果已登入
            cout << "1. 存錢" << endl; // 選項：存錢
            cout << "2. 取錢" << endl; // 選項：取錢
            cout << "3. 查詢餘額" << endl; // 選項：查詢餘額
            cout << "4. 登出" << endl; // 選項：登出
        }
        cout << "請輸入您的選擇："; // 提示用戶輸入選項

        if (!(cin >> choice)) { // 檢查輸入是否為數字
            cout << "無效輸入，請輸入數字。" << endl; // 提示錯誤
            clearInputBuffer(); // 清除無效輸入
            continue; // 返回迴圈開頭
        }

        if (current == nullptr) { // 未登入狀態的選項處理
            if (choice == 1) { // 創建新用戶
                string username, password; // 儲存用戶輸入的名稱和密碼
                cout << "請輸入用戶名稱："; // 提示輸入用戶名稱
                clearInputBuffer(); // 清除緩衝區以接受完整字串
                getline(cin, username); // 讀取用戶名稱（支援空格）
                cout << "請輸入密碼："; // 提示輸入密碼
                getline(cin, password); // 讀取密碼

                bool exists = false; // 標記用戶名稱是否已存在
                for (const Account& acc : accounts) { // 遍歷所有帳戶
                    if (acc.getUsername() == username) { // 檢查用戶名稱是否重複
                        exists = true; // 標記為已存在
                        break; // 退出迴圈
                    }
                }
                if (exists) { // 如果用戶名稱已存在
                    cout << "用戶名稱已存在，請選擇其他名稱。" << endl; // 顯示錯誤
                } else { // 如果用戶名稱可用
                    accounts.emplace_back(username, password); // 創建新帳戶並加入向量
                    cout << "用戶創建成功！" << endl; // 顯示成功訊息
                }
            } else if (choice == 2) { // 登入
                string username, password; // 儲存用戶輸入的名稱和密碼
                cout << "請輸入用戶名稱："; // 提示輸入用戶名稱
                clearInputBuffer(); // 清除緩衝區
                getline(cin, username); // 讀取用戶名稱
                cout << "請輸入密碼："; // 提示輸入密碼
                getline(cin, password); // 讀取密碼

                bool loggedIn = false; // 標記是否登入成功
                for (int i = 0; i < accounts.size(); ++i) { // 遍歷所有帳戶
                    if (accounts[i].getUsername() == username && accounts[i].checkPassword(password)) { // 檢查名稱和密碼
                        current = &accounts[i]; // 設置當前帳戶指標
                        loggedIn = true; // 標記登入成功
                        break; // 退出迴圈
                    }
                }
                if (loggedIn) { // 如果登入成功
                    cout << "登入成功！歡迎，" << current->getUsername() << "！" << endl; // 顯示歡迎訊息
                } else { // 如果登入失敗
                    cout << "登入失敗：用戶名稱或密碼錯誤。" << endl; // 顯示錯誤
                }
            } else if (choice == 3) { // 退出程式
                cout << "感謝使用 ATM 系統，再見！" << endl; // 顯示結束訊息
                break; // 退出主迴圈，結束程式
            } else { // 無效選項
                cout << "無效選項，請重新選擇。" << endl; // 提示重新輸入
            }
        } else { // 已登入狀態的選項處理
            if (choice == 1) { // 存錢
                double amount; // 儲存存款金額
                cout << "請輸入存款金額："; // 提示輸入金額
                if (!(cin >> amount)) { // 檢查輸入是否為數字
                    cout << "無效輸入，請輸入數字。" << endl; // 顯示錯誤
                    clearInputBuffer(); // 清除無效輸入
                    continue; // 返回迴圈開頭
                }
                if (current->deposit(amount)) { // 調用 deposit 函式存錢
                    cout << "存款成功！目前餘額：$" << current->getBalance() << endl; // 顯示成功和餘額
                } else { // 如果存錢失敗
                    cout << "存款失敗：金額必須為正數。" << endl; // 顯示錯誤
                }
            } else if (choice == 2) { // 取錢
                double amount; // 儲存取款金額
                cout << "請輸入取款金額："; // 提示輸入金額
                if (!(cin >> amount)) { // 檢查輸入是否為數字
                    cout << "無效輸入，請輸入數字。" << endl; // 顯示錯誤
                    clearInputBuffer(); // 清除無效輸入
                    continue; // 返回迴圈開頭
                }
                if (current->withdraw(amount)) { // 調用 withdraw 函式取錢
                    cout << "取款成功！目前餘額：$" << current->getBalance() << endl; // 顯示成功和餘額
                } else { // 如果取錢失敗
                    cout << "取款失敗：金額無效或餘額不足。" << endl; // 顯示錯誤
                }
            } else if (choice == 3) { // 查詢餘額
                cout << "目前餘額：$" << current->getBalance() << endl; // 顯示當前餘額
            } else if (choice == 4) { // 登出
                cout << "已登出，用戶：" << current->getUsername() << endl; // 顯示登出訊息
                current = nullptr; // 清除當前帳戶指標
            } else { // 無效選項
                cout << "無效選項，請重新選擇。" << endl; // 提示重新輸入
            }
        }
    }

    return 0; // 程式正常結束
}