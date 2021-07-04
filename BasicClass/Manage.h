#ifndef MANAGE_H
#define MANAGE_H

#include "BookTable.h"
#include "BorrowTable.h"
#include "UserTable.h"
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

// 管理类
class Manage {
    // 符号引入
public:
    using UserType = User::Type;
    using Admin = Administrator;
    using Record = BorrowRecord;

    // 属性
private:
    UserTable users{};     // 用户列表
    BookTable books{};     // 图书列表
    BorrowTable records{}; // 借阅列表
    User *currentUser;     // 当前用户
    QString usersFile{};   // 用户信息配置文件路径
    QString booksFile{};   // 图书信息配置文件路径
    QString recordsFile{}; // 借阅记录配置文件路径

    // 构造、析构
public:
    // 默认构造函数
    Manage()
        : currentUser(nullptr) {}

    // 析构函数
    ~Manage() {
        // 保存用户信息
        saveUsersInfo();
        // 保存图书信息
        saveBooksInfo();
        // 保存记录信息
        saveRecordsFile();
    }

    // 用户管理
public:
    // 设置当前用户
    void setCurrentUser(User &user) {
        // 切换用户
        currentUser = &user;
    }

    // 获取当前用户模式
    UserType userMode() const {
        return currentUser->getType();
    }

    // 获取当前用户
    User *getCurrentUser() {
        return currentUser;
    }

    // 获取当前管理员
    // 当前用户不是管理员，则返回空指针
    Admin *getCurrentAdmin() {
        return dynamic_cast<Admin *>(currentUser);
    }

    // 获取当前读者
    // 当前用户不是读者，则返回空指针
    Reader *getCurrentReader() {
        return dynamic_cast<Reader *>(currentUser);
    }

    // 根据ID查找一个管理员用户
    // 如果没找到，抛出异常
    Admin &selectAdminByID(int id) {
        return users.selectAdminByID(id);
    }

    // 根据ID查找一个读者用户
    // 如果没找到，抛出异常
    Reader &selectReaderByID(int id) {
        return users.selectReaderByID(id);
    }

    // 从JSON文件导入初始化用户信息
    void initUsers(const QString &fileName) {
        usersFile = fileName;

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::StandardButton result =
                    QMessageBox::warning(nullptr, "操作失败", "无法加载默认的 users.json, 请手动选择所在位置。", QMessageBox::Ok | QMessageBox::Cancel);
            if (result == QMessageBox::Ok) {
                usersFile = QFileDialog::getOpenFileName(nullptr, QString("打开用户信息配置文件"), "/", QString("数据交换文件(*json)"));
                file.setFileName(usersFile);
                if (!file.open(QIODevice::ReadOnly)) {
                    QMessageBox::warning(nullptr, "操作失败", "没有此JSON文件！程序退出！");
                    exit(1);
                }
            } else {
                exit(0);
            }
        }
        try {
            users.initFromJsonFile(file);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(nullptr, "操作失败", "未能成功解析users.json！程序退出！");
            exit(-1);
        }
        file.close();
    }

    // 从JSON文件批量添加用户(忽略ID)
    // json解析失败会抛出异常
    // 返回添加的新图书信息
    auto addUsers(QFile &jsonFile) {
        return users.importFromJsonFile(jsonFile);
    }

    // 用户注册
    // 返回系统分配的ID
    int registerReader(const QString &name, int age, const Telephone &tel, const Password &pwd) {
        return users.addNewReader(name, age, tel, pwd, 5);
    }

    // 获取所有读者
    auto getAllReaders() const {
        return users.getAllReaders();
    }

    // 获取所有管理员
    auto getAllAdmins() const {
        return users.getAllAdmins();
    }

    // 精确搜索读者
    auto selectReaders(const Reader &example) const {
        return users.select(example);
    }

    // 精确搜索管理员
    auto selectAdmins(const Admin &example) const {
        return users.select(example);
    }

    // 模糊搜索读者
    auto vagueSelectReaders(const Reader &example) const {
        return users.vagueSelect(example);
    }

    // 模糊搜索管理员
    auto vagueSelectAdmins(const Admin &example) const {
        return users.vagueSelect(example);
    }

    // 删除读者
    // 没找到读者抛出异常
    // 读者尚有书在借之中抛出异常
    void removeReader(int ReaderID) {
        // 非管理员不能操作
        if (userMode() != UserType::Admin)
            return;
        // 先查询该读者是否还有书在借中
        auto borrowingRecords = records.getBorrowingRecordsByReaderID(ReaderID);
        if (!borrowingRecords.empty())
            throw std::invalid_argument("该读者尚有书在借中，不能删除！");
        return users.removeReader(ReaderID);
    }

    // 删除管理员
    // 没有找到管理员抛出异常
    // 删除的管理员是自己抛出异常
    void removeAdmin(int AdminID) {
        // 非管理员不能操作
        if (userMode() != UserType::Admin)
            return;
        if (getCurrentAdmin()->getId() == AdminID)
            throw std::invalid_argument("不能在此界面中删除自己！");
        return users.removeAdmin(AdminID);
    }

    // 更新读者
    // 找不到读者抛出异常
    const Reader &updateReader(int ReaderID, const Reader &newReader) {
        return users.updateReader(ReaderID, newReader);
    }

    // 更新管理员
    // 找不到管理员抛出异常
    const Admin &updateAdmin(int AdminID, const Admin &newAdmin) {
        return users.updateAdmin(AdminID, newAdmin);
    }

    // 添加新用户
    // 返回系统分配的ID
    int addNewReader(const QString &name, int age, const Telephone &tel, const Password &pwd, int maxBorrowNumbers) {
        return users.addNewReader(name, age, tel, pwd, maxBorrowNumbers);
    }

    // 添加新管理员
    // 返回系统分配的ID
    int addNewAdmin(const QString &name, int age, const Telephone &tel, const Password &pwd) {
        return users.addNewAdmin(name, age, tel, pwd);
    }

    // 注销用户
    // 账户不存在，仍有书在借会抛出异常
    void logoutUser(User &user) {
        if (user.getType() == UserType::Admin) {
            users.removeAdmin(user.getId());
        } else {
            // 先查询该用户是否还有书在借中
            if (!getBorrowingRecords(user.getId()).empty())
                throw std::invalid_argument("你还有书在借中，不能注销账户。");
            users.removeReader(user.getId());
        }
    }

    // 导出当前用户信息
    void saveUsersInfo() {
        // 获取所有读者
        auto readers = getAllReaders();
        // 获取所有管理员
        auto admins = getAllAdmins();

        // 构建JSON数组 - readers
        QJsonArray jsonReaders;
        // 构建JSON数组 - admins
        QJsonArray jsonAdmins;

        for (const auto &reader : readers) {
            // 构建单个JSON对象 - reader
            QJsonObject jsonReader;
            jsonReader.insert("ID", reader.getId());
            jsonReader.insert("name", reader.getName());
            jsonReader.insert("telephone", reader.getTel().toString());
            jsonReader.insert("age", reader.getAge());
            jsonReader.insert("maxBorrowNumbers", reader.getMaxBorrowNumbers());
            jsonReader.insert("password", reader.getPwd().getCode());
            // 追加到readers中
            jsonReaders.append(jsonReader);
        }

        for (const auto &admin : admins) {
            // 构建单个JSON对象 - admin
            QJsonObject jsonAdmin;
            jsonAdmin.insert("ID", admin.getId());
            jsonAdmin.insert("name", admin.getName());
            jsonAdmin.insert("telephone", admin.getTel().toString());
            jsonAdmin.insert("age", admin.getAge());
            jsonAdmin.insert("password", admin.getPwd().getCode());
            // 追加到admins中
            jsonAdmins.append(jsonAdmin);
        }

        // 构建JSON对象 - 根
        QJsonObject rootObj;
        rootObj.insert("readers", jsonReaders);
        rootObj.insert("admins", jsonAdmins);

        // 构建json文档
        QJsonDocument jsonDoc;
        jsonDoc.setObject(rootObj);

        // 打开指定文件
        QFile file{usersFile};
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "System Error", "Save users information failed.");
            return;
        }
        file.write(jsonDoc.toJson());
        // 关闭文件
        file.close();
    }

    // 图书管理
public:
    // 从JSON文件初始化图书信息
    // 导入失败会抛出异常
    void
    initBooks(const QString &fileName) {
        booksFile = fileName;

        QFile file(booksFile);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::StandardButton result =
                    QMessageBox::warning(nullptr, "操作失败", "无法加载默认的 books.json, 请手动选择所在位置。", QMessageBox::Ok | QMessageBox::Cancel);
            if (result == QMessageBox::Ok) {
                booksFile = QFileDialog::getOpenFileName(nullptr, QString("打开图书信息配置文件"), "/", QString("数据交换文件(*json)"));
                file.setFileName(booksFile);
                if (!file.open(QIODevice::ReadOnly)) {
                    QMessageBox::warning(nullptr, "操作失败", "没有此JSON文件！程序退出！");
                    exit(1);
                }
            } else {
                exit(0);
            }
        }
        try {
            books.initFromJsonFile(file);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(nullptr, "操作失败", "未能成功解析books.json！程序退出！");
            exit(-1);
        }
        file.close();
    }

    // 从JSON文件批量添加图书(忽略ID)
    // json解析失败会抛出异常
    // 返回添加的新图书信息
    auto addBooks(QFile &jsonFile) {
        return books.importFromJsonFile(jsonFile);
    }

    // 获取所有图书信息
    auto getAllBooks() const {
        return books.getAllBooks();
    }

    // 精准查询图书
    auto selectBooks(const Book &example) const {
        return books.select(example);
    }

    // 模糊查询图书
    auto vagueSelectBooks(const Book &example) const {
        return books.vagueSelect(example);
    }

    // 删除图书
    void removeBook(int BookID) {
        // 非管理员不能操作
        if (userMode() != UserType::Admin)
            return;

        // 先查询这本书是否有正在借阅中的记录
        if (records.getBorrowingNumbers(BookID))
            throw std::invalid_argument("该书仍有正在借阅中的记录，不能删除！");

        // 删除书
        try {
            books.removeBook(BookID);
        } catch (std::invalid_argument &e) {
            throw std::invalid_argument("抱歉，没有这本书！请先重新查询！");
        }
    }

    // 通过ID选择图书
    Book &selectBookByID(int BookID) {
        return books.selectBookByID(BookID);
    }

    // 通过ID修改图书
    // 没找书会抛出异常
    void updateBookByID(int BookID, const Book &newBook) {
        // 非管理员不能操作
        if (userMode() != UserType::Admin)
            return;
        books.updateBook(BookID, newBook);
    }

    // 添加新书
    // 返回分配的ID
    int addNewBook(const QString &name, const QString &author, const QString &type, const QString &press, const QDate &pubDate, double price, int numbers) {
        // 非管理员不能操作
        if (userMode() != UserType::Admin)
            return -1;
        return books.addNewBook(name, author, type, press, pubDate, price, numbers);
    }

    // 保存图书信息
    void saveBooksInfo() {
        // 获取所有书
        auto allBooks = getAllBooks();
        // 构建JSON数组 - books
        QJsonArray jsonBooks;

        for (const auto &book : allBooks) {
            // 构建单个JSON对象 - book
            QJsonObject jsonBook;
            jsonBook.insert("ID", book.getId());
            jsonBook.insert("name", book.getName());
            jsonBook.insert("author", book.getAuthor());
            jsonBook.insert("type", book.getType());
            jsonBook.insert("press", book.getPress());
            jsonBook.insert("publication time", book.getPubDate().toString("yyyy-MM-dd"));
            jsonBook.insert("price", book.getPrice());
            jsonBook.insert("numbers", book.getNumbers());

            // 追加到books中
            jsonBooks.append(jsonBook);
        }
        // 构建json根对象
        QJsonObject jsonRoot;
        jsonRoot.insert("books", jsonBooks);

        // 构建json文档
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonRoot);

        // 打开指定文件
        QFile file{booksFile};
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "System Error", "Save books information failed.");
            return;
        }
        file.write(jsonDoc.toJson());
        // 关闭文件
        file.close();
    }

    // 借阅管理
public:
    // 从JSON文件初始化借阅记录
    void initRecords(const QString &fileName) {
        recordsFile = fileName;

        QFile file(recordsFile);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::StandardButton result =
                    QMessageBox::warning(nullptr, "操作失败", "无法加载默认的 records.json, 请手动选择所在位置。", QMessageBox::Ok | QMessageBox::Cancel);
            if (result == QMessageBox::Ok) {
                recordsFile = QFileDialog::getOpenFileName(nullptr, QString("打开借阅记录配置文件"), "/", QString("数据交换文件(*json)"));
                file.setFileName(fileName);
                if (!file.open(QIODevice::ReadOnly)) {
                    QMessageBox::warning(nullptr, "操作失败", "没有此JSON文件！程序退出！");
                    exit(1);
                }
            } else {
                exit(0);
            }
        }
        try {
            records.initFromJsonFile(file);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(nullptr, "操作失败", "未能成功解析records.json！程序退出！");
            exit(-1);
        }
        file.close();
    }

    // 获取某书正在借阅的数量
    int getBorrowingNumbers(int BookID) const {
        return records.getBorrowingNumbers(BookID);
    }

    // 借书
    void borrowBook(int BookID) {
        // 只有读者才可以借书
        if (userMode() != UserType::Reader)
            return;

        auto borrowingRecords = records.getBorrowingRecordsByReaderID(currentUser->getId());

        // 读者是否有超期未还的书
        for (const auto &record : borrowingRecords) {
            if (abs(QDate::currentDate().daysTo(record.getBorrowDate())) > 30)
                throw std::invalid_argument("你有书超期未还！请先还书！");
        }

        // 读者是否达到最大借阅数量
        if (borrowingRecords.size() >= getCurrentReader()->getMaxBorrowNumbers())
            throw std::invalid_argument("达到最大借阅数量!");

        // 是否借阅过这本书
        if (records.hasBorrowingRecord(getCurrentReader()->getId(), BookID))
            throw std::invalid_argument("已经借过这本书了！");

        // 借书
        try {
            books.borrow(BookID);
            // 写借阅记录
            records.addBorrowingRecord(currentUser->getId(), BookID);
        } catch (std::invalid_argument &e) {
            if (!strcmp(e.what(), "Has borrowing record.")) {
                books.back(BookID);
                throw std::invalid_argument("已经借过这本书了！");
            } else if (!strcmp(e.what(), "To reduce number is larger than current number"))
                throw std::invalid_argument("该书库存为0！");
            else if (!strcmp(e.what(), "Can not locate this Book"))
                throw std::invalid_argument("没有该书！");
        }
    }

    // 还书
    // 返还超期费用
    // 找不到记录抛出异常
    double backBook(int BookID) {
        double money = 0.0;
        // 只有读者才可以还书
        if (userMode() != UserType::Reader)
            return 0.0;

        // 先找到记录
        auto &record = records.selectBorrowingRecordByID(currentUser->getId(), BookID);

        // 计算超期费用
        int days = int(abs(QDate::currentDate().daysTo(record.getBorrowDate())));
        if (days > 30)
            money = days * 0.5;

        // 修改记录
        record.completeBack();

        // 完成还书
        books.back(BookID);

        // 返回超期费用
        return money;
    }

    // 获取某个用户当前正在借阅的记录
    BorrowTable::Table getBorrowingRecords(int ReaderID) const {
        return records.getBorrowingRecordsByReaderID(ReaderID);
    }

    // 获取当前读者正在借阅的记录
    auto getBorrowingRecords() const {
        return records.getBorrowingRecordsByReaderID(currentUser->getId());
    }

    // 获取某个用户所有的借书记录
    auto getAllRecordsByReaderID(int ReaderID) const {
        return records.getAllRecordsByReaderID(ReaderID);
    }

    // 获取当前读者所有的借书记录
    auto getAllRecordsByCurrentReader() const {
        return getAllRecordsByReaderID(currentUser->getId());
    }

    // 获取所有借阅记录
    auto getAllRecords() const {
        return records.getAllRecords();
    }

    // 精准搜索记录
    auto selectRecords(const Record &example) const {
        return records.select(example);
    }

    // 模糊搜索记录
    auto vagueSelectRecords(const Record &example) const {
        return records.vagueSelect(example);
    }

    // 保存当前借阅记录
    void saveRecordsFile() {
        // 获取所有借阅记录
        auto allRecords = getAllRecords();
        // 构建JSON数组 - records
        QJsonArray jsonRecords;

        for (const auto &record : allRecords) {
            // 构建单个JSON对象 - record
            QJsonObject jsonRecord;
            jsonRecord.insert("reader ID", record.getReaderId());
            jsonRecord.insert("book ID", record.getBookId());
            jsonRecord.insert("borrow date", record.getBorrowDate().toString("yyyy-MM-dd"));
            jsonRecord.insert("return date", record.getReturnDate().toString("yyyy-MM-dd"));
            if (record.getStatus() == Record::Status::Borrowing)
                jsonRecord.insert("status", "Borrowing");
            else
                jsonRecord.insert("status", "Returned");
            // 追加到books中
            jsonRecords.append(jsonRecord);
        }

        // 构建json根对象
        QJsonObject jsonRoot;
        jsonRoot.insert("records", jsonRecords);

        // 构建json文档
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonRoot);

        // 打开指定文件
        QFile file{recordsFile};
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "System Error", "Save records information failed.");
            return;
        }
        file.write(jsonDoc.toJson());
        // 关闭文件
        file.close();
    }
};


#endif //MANAGE_H
