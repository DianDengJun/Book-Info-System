#ifndef USERTABLE_H
#define USERTABLE_H

#include "Password.h"
#include "User.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <vector>

class UserTable {
    // 符号引入
public:
    using Admin = Administrator;
    using ReaderTable = std::vector<Reader>;
    using AdminTable = std::vector<Admin>;
    using Table = std::vector<const User *>;

    // 属性
private:
    ReaderTable readers{};
    AdminTable admins{};

    // 构造
public:
    // 默认构造函数
    UserTable() = default;

    // 查找与遍历
public:
    // 提取出所有读者信息
    const ReaderTable &getAllReaders() const {
        return readers;
    }

    // 提取出所有管理员信息
    const AdminTable &getAllAdmins() const {
        return admins;
    }

    // 提取出所有用户信息
    Table getAllUsers() const {
        Table table;
        table.reserve(readers.size() + admins.size());
        for (auto &reader : readers)
            table.push_back(&reader);
        for (auto &admin : admins)
            table.push_back(&admin);
        return table;
    }

    // 查询是否有某个ID的读者
    bool hasReaderByID(int id) const {
        return std::binary_search(readers.begin(), readers.end(), Reader{id});
    }

    // 查询是否有某个ID的管理员
    bool hasAdminByID(int id) const {
        return std::binary_search(admins.begin(), admins.end(), Admin{id});
    }

    // 精准查询读者
    ReaderTable select(const Reader &reader) const {
        // 查询结果集合
        ReaderTable ans;

        // 依次查询匹配
        for (const auto &r : readers)
            if (r.isSameProperties(reader))
                ans.push_back(r);

        return ans;
    }

    // 精准查询管理员
    AdminTable select(const Admin &admin) const {
        // 查询结果集合
        AdminTable ans;

        // 依次查询匹配
        for (const auto &a : admins)
            if (a.isSameProperties(admin))
                ans.push_back(a);

        return ans;
    }

    // 模糊查询读者
    ReaderTable vagueSelect(const Reader &reader) const {
        // 查询结果集合
        ReaderTable ans;

        // 依次查询匹配
        for (const auto &r : readers)
            if (r.hasSameProperties(reader))
                ans.push_back(r);

        return ans;
    }

    // 模糊查询管理员
    AdminTable vagueSelect(const Admin &admin) const {
        // 查询结果集合
        AdminTable ans;

        // 依次查询匹配
        for (const auto &a : admins)
            if (a.hasSameProperties(admin))
                ans.push_back(a);

        return ans;
    }

    // 选定某个特定编号的读者
    Reader &selectReaderByID(int id) {
        auto reader = std::lower_bound(readers.begin(), readers.end(), Administrator{id});
        if (reader != readers.end() && reader->getId() == id)
            return *reader;
        else
            throw std::invalid_argument{"Can not select this Reader"};
    }

    // 选定某个特定编号的管理员
    Admin &selectAdminByID(int id) {
        auto admin = std::lower_bound(admins.begin(), admins.end(), Administrator{id});
        if (admin != admins.end() && admin->getId() == id)
            return *admin;
        else
            throw std::invalid_argument{"Can not locate this Admin"};
    }

    // 读者空表检测
    bool isReaderEmpty() const {
        return readers.empty();
    }

    // 管理员空表检测
    bool isAdminEmpty() const {
        return admins.empty();
    }

    // 读者最小ID
    int readerMinID() const {
        return readers.front().getId();
    }

    // 管理员最小ID
    int adminMinID() const {
        return admins.front().getId();
    }

    // 读者最大ID
    int readerMaxID() const {
        return readers.back().getId();
    }

    // 管理员最大ID
    int adminMaxID() const {
        return admins.back().getId();
    }

    // 添加器
public:
    // 添加一个新读者(重分配ID)
    // 返回系统分配的ID
    int addNewReader(const Reader &reader) {
        return addNewReader(reader.getName(), reader.getAge(), reader.getTel(), reader.getPwd(), reader.getMaxBorrowNumbers());
    }

    // 添加一个新管理员(重分配ID)
    // 返回系统分配的ID
    int addNewAdmin(const Admin &admin) {
        return addNewAdmin(admin.getName(), admin.getAge(), admin.getTel(), admin.getPwd());
    }

    // 添加一个新读者信息
    // 返回系统分配的ID
    int addNewReader(const QString &name, int age, const Telephone &tel, const Password &pwd, int maxBorrowNumbers) {
        int readerID = 1;
        // 寻找一个最大的ID
        if (!isReaderEmpty())
            readerID = readerMaxID() + 1;
        readers.emplace_back(readerID, name, age, tel, pwd, maxBorrowNumbers);
        return readerID;
    }

    // 添加一个新管理员信息
    // 返回系统分配的ID
    int addNewAdmin(const QString &name, int age, const Telephone &tel, const Password &pwd) {
        int adminID = 1;
        // 寻找一个最大的ID
        if (!isAdminEmpty())
            adminID = adminMaxID() + 1;
        admins.emplace_back(adminID, name, age, tel, pwd);
        return adminID;
    }

    // 直接导入读者
    void addReader(const Reader &reader) {
        readers.push_back(reader);
    }

    // 直接导入管理员
    void addAdmin(const Admin &admin) {
        admins.push_back(admin);
    }

    // 从JSON文件中批量初始化用户信息
    void initFromJsonFile(QFile &jsonFile) {
        // 读json数据
        QByteArray jsonData = jsonFile.readAll();

        // JSON错误指示
        QJsonParseError jsonError{};

        // 实例化JSON文档
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);

        // JSON文档解析
        if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError)) {
            // 解析为JSON对象
            if (jsonDoc.isObject()) {
                QJsonObject object = jsonDoc.object();
                // 获取 admins
                if (object.contains("admins") && object.value("admins").isArray()) {
                    // 转换为数组
                    QJsonArray jsonAdmins = object.value("admins").toArray();
                    int id;
                    int age;
                    QString name;
                    QString strPwd;
                    QString strTel;
                    for (auto &&jsonAdmin : jsonAdmins) {
                        QJsonObject item = jsonAdmin.toObject();
                        if (item.contains("ID")) {
                            id = item.value("ID").toInt();
                        }
                        if (item.contains("age")) {
                            age = item.value("age").toInt();
                        }
                        if (item.contains("name")) {
                            name = item.value("name").toString();
                        }
                        if (item.contains("password")) {
                            strPwd = item.value("password").toString();
                        }
                        if (item.contains("telephone")) {
                            strTel = item.value("telephone").toString();
                        }
                        // 载入管理员
                        addAdmin(Administrator{id, name, age, Telephone{strTel}, Password{strPwd, true}});
                    }
                }
                // 获取 readers
                if (object.contains("readers") && object.value("readers").isArray()) {
                    // 转换为数组
                    QJsonArray jsonAdmins = object.value("readers").toArray();
                    int id;
                    int age;
                    int maxBorrowNumbers;
                    QString name;
                    QString strPwd;
                    QString strTel;
                    for (auto &&jsonAdmin : jsonAdmins) {
                        QJsonObject item = jsonAdmin.toObject();
                        if (item.contains("ID")) {
                            id = item.value("ID").toInt();
                        }
                        if (item.contains("age")) {
                            age = item.value("age").toInt();
                        }
                        if (item.contains("maxBorrowNumbers")) {
                            maxBorrowNumbers = item.value("maxBorrowNumbers").toInt();
                        }
                        if (item.contains("name")) {
                            name = item.value("name").toString();
                        }
                        if (item.contains("password")) {
                            strPwd = item.value("password").toString();
                        }
                        if (item.contains("telephone")) {
                            strTel = item.value("telephone").toString();
                        }
                        // 载入读者
                        addReader(Reader{id, name, age, Telephone{strTel}, Password{strPwd, true}, maxBorrowNumbers});
                    }
                }
            }
        } else
            throw std::invalid_argument("JsonParseError");
    }

    // 从JSON文件批量导入用户信息(忽略用户ID)
    // JSON解析失败会抛出异常
    // 返回新添加的图书信息
    UserTable importFromJsonFile(QFile &jsonFile) {
        UserTable ans;
        // 读json数据
        QByteArray jsonData = jsonFile.readAll();

        // JSON错误指示
        QJsonParseError jsonError{};

        // 实例化JSON文档
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);

        // JSON文档解析
        if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError)) {
            // 解析为JSON对象
            if (jsonDoc.isObject()) {
                QJsonObject object = jsonDoc.object();
                // 获取 admins
                if (object.contains("admins") && object.value("admins").isArray()) {
                    // 转换为数组
                    QJsonArray jsonAdmins = object.value("admins").toArray();
                    int age;
                    QString name;
                    QString strPwd;
                    QString strTel;
                    for (auto &&jsonAdmin : jsonAdmins) {
                        QJsonObject item = jsonAdmin.toObject();
                        if (item.contains("age")) {
                            age = item.value("age").toInt();
                        }
                        if (item.contains("name")) {
                            name = item.value("name").toString();
                        }
                        if (item.contains("password")) {
                            strPwd = item.value("password").toString();
                        }
                        if (item.contains("telephone")) {
                            strTel = item.value("telephone").toString();
                        }
                        // 添加管理员
                        addNewAdmin(name, age, Telephone{strTel}, Password{strPwd, true});
                        ans.addAdmin(admins.back());
                    }
                }
                // 获取 readers
                if (object.contains("readers") && object.value("readers").isArray()) {
                    // 转换为数组
                    QJsonArray jsonReaders = object.value("readers").toArray();
                    int age;
                    int maxBorrowNumbers;
                    QString name;
                    QString strPwd;
                    QString strTel;
                    for (auto &&jsonReader: jsonReaders) {
                        QJsonObject item = jsonReader.toObject();
                        if (item.contains("age")) {
                            age = item.value("age").toInt();
                        }
                        if (item.contains("maxBorrowNumbers")) {
                            maxBorrowNumbers = item.value("maxBorrowNumbers").toInt();
                        }
                        if (item.contains("name")) {
                            name = item.value("name").toString();
                        }
                        if (item.contains("password")) {
                            strPwd = item.value("password").toString();
                        }
                        if (item.contains("telephone")) {
                            strTel = item.value("telephone").toString();
                        }
                        // 添加读者
                        addNewReader(name, age, Telephone{strTel}, Password{strPwd, true}, maxBorrowNumbers);
                        ans.addReader(readers.back());
                    }
                }
            }
        } else
            throw std::invalid_argument("JsonParseError");
        return ans;
    }


    // 删除器
public:
    // 删除某个读者(依据ID)
    // 没找到读者时抛出异常
    void removeReader(int id) {
        // 查找第一个ID大于或等于它的书，返回迭代器
        auto pToRemove = std::lower_bound(readers.begin(), readers.end(), Reader{id});
        if (pToRemove == readers.end() || pToRemove->getId() != id)
            // 没找到书时抛出异常
            throw std::invalid_argument{"Can not remove invalid reader"};
        else
            readers.erase(pToRemove);
    }

    // 删除某个管理员(依据ID)
    // 没找到管理员时抛出异常
    void removeAdmin(int id) {
        // 查找第一个ID大于或等于它的管理员，返回迭代器
        auto pToRemove = std::lower_bound(admins.begin(), admins.end(), Administrator{id});
        if (pToRemove == admins.end() || pToRemove->getId() != id)
            // 没找到管理员时抛出异常
            throw std::invalid_argument{"Can not remove invalid admin"};
        else
            admins.erase(pToRemove);
    }

    // 修改器
public:
    // 修改某个读者(依据ID)的信息
    // newReader 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    // 修改成功, 返回修改后的读者信息
    const Reader &updateReader(int id, const Reader &newReader) {
        // 查找第一个ID大于或等于它的书，返回迭代器
        auto pToUpdate = std::lower_bound(readers.begin(), readers.end(), Reader{id});
        if (pToUpdate == readers.end() || pToUpdate->getId() != id)
            // 没找到读者时抛出异常
            throw std::invalid_argument{"Can not update invalid reader by ID " + std::to_string(id)};
        else {
            pToUpdate->update(newReader);
            return *pToUpdate;
        }
    }

    // 修改某个管理员(依据ID)的信息
    // newAdmin 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    // 修改成功, 返回修改后的管理员信息
    const Administrator &updateAdmin(int id, const Administrator &newAdmin) {
        // 查找第一个ID大于或等于它的书，返回迭代器
        auto pToUpdate = std::lower_bound(admins.begin(), admins.end(), Administrator{id});
        if (pToUpdate == admins.end() || pToUpdate->getId() != id)
            // 没找到管理员时抛出异常
            throw std::invalid_argument{"Can not update invalid admin by ID " + std::to_string(id)};
        else {
            pToUpdate->update(newAdmin);
            return *pToUpdate;
        }
    }
};


#endif // USERTABLE_H
