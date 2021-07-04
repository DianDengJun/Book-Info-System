#ifndef USER_H
#define USER_H

#include "Password.h"
#include "Telephone.h"
#include <QDebug>
#include <string>

class User {
public:
    // 用户类别枚举
    enum class Type {
        Undefined, // 未定义
        Reader,    // 读者
        Admin,     // 管理员
    };

protected:
    int id;        // 唯一编号
    QString name;  // 姓名
    int age;       // 年龄
    Telephone tel; // 电话号码
    Password pwd;  // 密码
    Type type;     // 用户类别
public:
    // 默认构造函数
    explicit User(int id = 0,
                  const QString &name = "",
                  int age = 0,
                  const Telephone &tel = Telephone{},
                  const Password &pwd = Password{},
                  Type type = Type::Undefined)
        : id(id), name(name), age(age), tel(tel), pwd(pwd), type(type) {}

    // 是否为管理员
    bool isAdmin() const {
        return type == Type::Admin;
    }

    int getId() const {
        return id;
    }

    void setId(int id) {
        this->id = id;
    }

    const QString &getName() const {
        return name;
    }

    void setName(const QString &name) {
        this->name = name;
    }

    int getAge() const {
        return age;
    }

    void setAge(int age) {
        this->age = age;
    }

    const Telephone &getTel() const {
        return tel;
    }

    void setTel(const Telephone &tel) {
        this->tel = tel;
    }

    const Password &getPwd() const {
        return pwd;
    }

    void setPwd(const Password &pwd) {
        this->pwd = pwd;
    }

    Type getType() const {
        return type;
    }

    void setType(Type type) {
        this->type = type;
    }

    // 是否是同一用户
    bool operator==(const User &rhs) const {
        return id == rhs.id;
    }

    // 是否不为同一用户
    bool operator!=(const User &rhs) const {
        return !(*this == rhs);
    }

    // 对用户按ID排序
    bool operator<(const User &rhs) const {
        return id < rhs.id;
    }

    // 修改用户信息
    // newUser 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    void update(const User &newUser) {
        if (!newUser.name.isEmpty())
            name = newUser.name;
        if (newUser.age > 0)
            age = newUser.age;
        if (!newUser.tel.isIllegal())
            tel = newUser.tel;
        if (newUser.type != Type::Undefined)
            type = newUser.type;
        if (!newUser.pwd.empty())
            pwd = newUser.pwd;
    }

    // 和本用户是否为相同属性
    // 多字段比较，忽略空字段或非法字段
    bool isSameProperties(const User &user) const {
        // 字段全空，另外判断
        if (user.id < 0 && user.name.isEmpty() && user.age < 0 && user.tel.isIllegal() && user.type == Type::Undefined)
            return false;

        bool result = true;
        if (user.id >= 0)
            result = result && id == user.id;
        if (!user.name.isEmpty())
            result = result && name == user.name;
        if (user.age >= 0)
            result = result && age == user.age;
        if (!user.tel.isIllegal())
            result = result && tel == user.tel;
        if (user.type != Type::Undefined)
            result = result && type == user.type;
        return result;
    }

    // 和本读者是否有相同属性
    // 只要有一个字段符合即可
    bool hasSameProperties(const User &user) const {
        bool result = false;
        if (user.id >= 0)
            result = result || id == user.id;
        if (!user.name.isEmpty())
            result = result || name == user.name;
        if (user.age >= 0)
            result = result || age == user.age;
        if (!user.tel.isIllegal())
            result = result || tel == user.tel;
        if (user.type != Type::Undefined)
            result = result || type == user.type;
        return result;
    }

    // 转换为字符串
    virtual QString toString() const {
        return QString{"User{" + QString::number(id) + "," + name + "," + QString::number(age) + "," + tel.toString() + "}"};
    }
};

class Reader : public User {
private:
    int maxBorrowNumbers; // 最大借书量
public:
    // 默认构造函数
    explicit Reader(int id = 0,
                    const QString &name = "",
                    int age = 0,
                    const Telephone &tel = Telephone{},
                    const Password &pwd = Password{},
                    int maxBorrowNumbers = 0)
        : User(id, name, age, tel, pwd, User::Type::Reader),
          maxBorrowNumbers(maxBorrowNumbers) {}

    int getMaxBorrowNumbers() const {
        return maxBorrowNumbers;
    }

    void setMaxBorrowNumbers(int maxBorrowNumbers) {
        this->maxBorrowNumbers = maxBorrowNumbers;
    }

    // 修改读者信息
    // newReader 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    void update(const Reader &newReader) {
        if (!newReader.name.isEmpty())
            name = newReader.name;
        if (newReader.age > 0)
            age = newReader.age;
        if (!newReader.tel.isIllegal())
            tel = newReader.tel;
        if (!newReader.pwd.empty())
            pwd = newReader.pwd;
        if (newReader.maxBorrowNumbers > 0)
            maxBorrowNumbers = newReader.maxBorrowNumbers;
    }

    // 和本读者是否是相同属性
    // 多字段比较，忽略空字段或非法字段
    bool isSameProperties(const Reader &reader) const {
        // 字段全空，另外判断
        if (reader.id < 0 && reader.name.isEmpty() && reader.age < 0 && reader.tel.isIllegal() && reader.maxBorrowNumbers < 0)
            return false;

        bool result = true;
        if (reader.id >= 0)
            result = result && id == reader.id;
        if (!reader.name.isEmpty())
            result = result && name == reader.name;
        if (reader.age >= 0)
            result = result && age == reader.age;
        if (!reader.tel.isIllegal())
            result = result && tel == reader.tel;
        if (reader.maxBorrowNumbers >= 0)
            result = result && maxBorrowNumbers == reader.maxBorrowNumbers;

        return result;
    }

    // 和本读者是否有相同属性
    // 只要有一个字段符合即可
    bool hasSameProperties(const Reader &reader) const {
        bool result = false;
        if (reader.id >= 0)
            result = result || id == reader.id;
        if (!reader.name.isEmpty())
            result = result || name == reader.name;
        if (reader.age >= 0)
            result = result || age == reader.age;
        if (!reader.tel.isIllegal())
            result = result || tel == reader.tel;
        if (reader.maxBorrowNumbers >= 0)
            result = result || maxBorrowNumbers == reader.maxBorrowNumbers;
        return result;
    }

    // 转换为字符串
    QString toString() const override {
        return QString{"Reader{" + QString::number(id) + "," + name + "," + QString::number(age) + "," + tel.toString() + "," + QString::number(maxBorrowNumbers) + "}"};
    }
};

class Administrator : public User {
public:
    explicit Administrator(int id = 0,
                           const QString &name = "",
                           int age = 0,
                           const Telephone &tel = Telephone{},
                           const Password &pwd = Password{})
        : User(id, name, age, tel, pwd, User::Type::Admin) {}

    // 修改管理员信息
    // newAdmin 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    void update(const Administrator &newAdmin) {
        if (!newAdmin.name.isEmpty())
            name = newAdmin.name;
        if (newAdmin.age > 0)
            age = newAdmin.age;
        if (!newAdmin.tel.isIllegal())
            tel = newAdmin.tel;
        if (!newAdmin.pwd.empty())
            pwd = newAdmin.pwd;
    }

    // 和本管理员是否为相同属性
    // 多字段比较，忽略空字段或非法字段
    bool isSameProperties(const Administrator &admin) const {
        // 字段全空，另外判断
        if (admin.id < 0 && admin.name.isEmpty() && admin.age < 0 && admin.tel.isIllegal())
            return false;

        bool result = true;

        if (admin.id >= 0)
            result = result && id == admin.id;
        if (!admin.name.isEmpty())
            result = result && name == admin.name;
        if (admin.age >= 0)
            result = result && age == admin.age;
        if (!admin.tel.isIllegal())
            result = result && tel == admin.tel;

        return result;
    }

    // 和本管理员是否有相同属性
    // 只要有一个字段符合即可
    bool hasSameProperties(const Administrator &admin) const {
        bool result = false;
        if (admin.id >= 0)
            result = result || id == admin.id;
        if (!admin.name.isEmpty())
            result = result || name == admin.name;
        if (admin.age >= 0)
            result = result || age == admin.age;
        if (!admin.tel.isIllegal())
            result = result || tel == admin.tel;
        return result;
    }

    // 转换为字符串
    QString toString() const override {
        return QString{"Admin{" + QString::number(id) + "," + name + "," + QString::number(age) + "," + tel.toString() + "}"};
    }
};

#endif // USER_H
