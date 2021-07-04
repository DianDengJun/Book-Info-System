#ifndef TELEPHONE_H
#define TELEPHONE_H

#include <regex>
#include <stdexcept>
#include <QString>

class Telephone {
private:
    QString number;  // 号码
public:
    // 默认构造函数
    // "18000000000" 是特殊号码，注意不要使用
    explicit Telephone(const QString &number = "18000000000") {
        if (isTel(number))
            this->number = number;
        else throw std::invalid_argument(toStdString() + " is not a valid Telephone.");
    }

    // 转换为标准字符串
    std::string toStdString() const {
        return number.toStdString();
    }

    // 检测是否为11位电话号码
    static bool isTel(const QString &number) {
        // 首先确保字符串为11位
        if (number.size() == 11) {
            // 正则表达式检测是否为11位电话号码
            std::regex r{"^1([358][0-9]|4[579]|66|7[0135678]|9[89])[0-9]{8}$"};
            return std::regex_match(number.toStdString(), r);
        } else return false;
    }

    // 从字符串赋值
    Telephone &operator=(const QString &n) {
        if (number != n) {
            if (isTel(n))
                number = n;
            else throw std::invalid_argument(n.toStdString() + " is not a valid Telephone.");
        }
        return *this;
    }

    // 判断电话号码是否相等
    bool operator==(const Telephone &rhs) const {
        return number == rhs.number;
    }

    // 判断电话号码是否不相等
    bool operator!=(const Telephone &rhs) const {
        return !(rhs == *this);
    }

    // 调用()运算符，转换为字符串(返回的是左值)
    const QString &operator()() const {
        return number;
    }

    // 强制转换为字符串(返回的是右值)
    explicit operator QString() const {
        return number;
    }

    // 转换为字符串
    const QString &toString() const {
        return number;
    }

    // 是非法电话吗？
    bool isIllegal() const {
        return number == "18000000000";
    }
};


#endif // TELEPHONE_H
