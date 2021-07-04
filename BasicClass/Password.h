#ifndef PASSWORD_H
#define PASSWORD_H

#include <QString>

class Password {
private:
    QString code;    // 存储密文
    static char key; // 密钥
public:
    // 默认构造函数
    Password() { code = ""; }

    // 是否是空密码
    bool empty() const {
        return code.isEmpty();
    }

    // 从字符串构造
    // isCode isCode 指定了传入的是明文(false)还是密文(true)
    explicit Password(const QString &str, bool isCode = false) {
        code = isCode ? str : x_or(str);
    }

    // 从明文字符串赋值
    Password &operator=(const QString &str) {
        code = x_or(str);
        return *this;
    }

    // 做异或运算
    static QString x_or(const QString &str) {
        QString ans = str;
        // 逐位与密钥异或
        for (auto &item : ans) {
            item = item.toLatin1() ^ key;
        }
        return ans;
    }

    // 获取密文
    const QString &getCode() const {
        return code;
    }

    // 解密当前密码(获取明文)
    QString decrypt() const {
        return x_or(code);
    }

    // 判断密码是否相等
    bool operator==(const Password &rhs) const {
        return code == rhs.code;
    }

    // 判断密码是否不相等
    bool operator!=(const Password &rhs) const {
        return !(rhs == *this);
    }

    // 判断密码是否与明文字符串相等
    bool operator==(const QString &rhs) const {
        return decrypt() == rhs;
    }

    // 判断明文字符串是否与密码相等
    friend bool operator==(const QString &lhs, const Password &rhs) {
        return rhs == lhs;
    }

    // 判断密码是否与明文字符串不相等
    bool operator!=(const QString &rhs) const {
        return !(*this == rhs);
    }

    // 判断明文字符串是否与密码不相等
    friend bool operator!=(const QString &lhs, const Password &rhs) {
        return !(rhs == lhs);
    }
};

#endif // PASSWORD_H
