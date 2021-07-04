#ifndef BOOKTABLE_H
#define BOOKTABLE_H

#include "Book.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <vector>

class BookTable {
    // 符号引入
public:
    using Table = std::vector<Book>;

    // 属性
private:
    Table books;

    // 构造
public:
    // 默认构造函数
    BookTable() = default;

    // 查找与遍历
public:
    // 提取出所有图书信息
    const Table &getAllBooks() const {
        return books;
    }

    // 查询是否有某个ID的图书
    bool hasBookByID(int id) const {
        return std::binary_search(books.begin(), books.end(), Book{id});
    }

    // 精准查询
    Table select(const Book &book) const {
        // 查询结果集合
        Table ans;

        // 依次查询匹配
        for (const auto &b : books)
            if (b.isSameProperties(book))
                ans.push_back(b);

        return ans;
    }

    // 模糊查询
    Table vagueSelect(const Book &book) const {
        // 查询结果集合
        Table ans;

        // 依次查询匹配
        for (const auto &b : books)
            if (b.hasSameProperties(book))
                ans.push_back(b);

        return ans;
    }

    // 选定某个特定编号的图书
    Book &selectBookByID(int id) {
        auto book = std::lower_bound(books.begin(), books.end(), Book{id});
        if (book != books.end() && book->getId() == id)
            return *book;
        else
            throw std::invalid_argument{"Can not select this Book"};
    }

    // 空表检测
    bool empty() const {
        return books.empty();
    }

    // 最小ID
    int minID() const {
        return books.front().getId();
    }

    // 最大ID
    int maxID() const {
        return books.back().getId();
    }

    // 添加器
public:
    // 添加一个新图书(重新分配ID)
    // 返回系统分配的ID
    int addNewBook(const Book &book) {
        return addNewBook(book.getName(), book.getAuthor(), book.getType(), book.getPress() ,book.getPubDate(), book.getPrice(), book.getNumbers());
    }

    // 添加一个新图书信息
    // 返回系统分配的ID
    int addNewBook(const QString &name, const QString &author, const QString &type, const QString &press, const QDate &pubDate, double price, int numbers) {
        int bookID = 1;
        // 寻找一个最大的ID
        if (!empty())
            bookID = maxID() + 1;
        books.emplace_back(bookID, name, author, type, press, pubDate, price, numbers);
        return bookID;
    }

    // 直接导入图书
    void addBook(const Book &book) {
        books.push_back(book);
    }

    // 从JSON文件中批量初始化图书信息
    void initFromJsonFile(QFile &jsonFile) {
        // 读json数据
        QByteArray jsonData = jsonFile.readAll();

        // JSON错误指示
        QJsonParseError jsonError{};

        // 实例化JSON文档
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);

        // JSON文档解析
        if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError)) { // 解析未发生错误
            // 解析为JSON对象
            if (jsonDoc.isObject()) {
                QJsonObject object = jsonDoc.object();
                // 获取 books
                if (object.contains("books") && object.value("books").isArray()) {
                    // 转换为数组
                    QJsonArray jsonBooks = object.value("books").toArray();
                    int id;
                    QString name;
                    QString author;
                    QString type;
                    QString press;
                    QDate pubTime;
                    double price;
                    int numbers;
                    for (auto &&jsonBook : jsonBooks) {
                        QJsonObject item = jsonBook.toObject();
                        if (item.contains("ID")) {
                            id = item.value("ID").toInt();
                        }
                        if (item.contains("name")) {
                            name = item.value("name").toString();
                        }
                        if (item.contains("author")) {
                            author = item.value("author").toString();
                        }
                        if (item.contains("type")) {
                            type = item.value("type").toString();
                        }
                        if (item.contains("press")) {
                            press = item.value("press").toString();
                        }
                        if (item.contains("publication time")) {
                            pubTime = QDate::fromString(item.value("publication time").toString(), "yyyy-MM-dd");
                        }
                        if (item.contains("price")) {
                            price = item.value("price").toDouble();
                        }
                        if (item.contains("numbers")) {
                            numbers = item.value("numbers").toInt();
                        }
                        // 载入图书
                        addBook(Book{id, name, author, type, press, pubTime, price, numbers});
                    }
                }
            }
        } else
            throw std::invalid_argument("JsonParseError");
    }

    // 从JSON文件批量导入图书信息(忽略图书ID)
    // JSON解析失败会抛出异常
    // 返回新添加的图书信息
    Table importFromJsonFile(QFile &jsonFile) {
        Table ans;

        // 读json数据
        QByteArray jsonData = jsonFile.readAll();

        // JSON错误指示
        QJsonParseError jsonError{};

        // 实例化JSON文档
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);

        // JSON文档解析
        if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError)) { // 解析未发生错误
            // 解析为JSON对象
            if (jsonDoc.isObject()) {
                QJsonObject object = jsonDoc.object();
                // 获取 books
                if (object.contains("books") && object.value("books").isArray()) {
                    // 转换为数组
                    QJsonArray jsonBooks = object.value("books").toArray();
                    QString name;
                    QString author;
                    QString type;
                    QString press;
                    QDate pubTime;
                    double price;
                    int numbers;
                    for (auto &&jsonBook : jsonBooks) {
                        QJsonObject item = jsonBook.toObject();
                        if (item.contains("name")) {
                            name = item.value("name").toString();
                        }
                        if (item.contains("author")) {
                            author = item.value("author").toString();
                        }
                        if (item.contains("type")) {
                            type = item.value("type").toString();
                        }
                        if (item.contains("press")) {
                            press = item.value("press").toString();
                        }
                        if (item.contains("publication time")) {
                            pubTime = QDate::fromString(item.value("publication time").toString(), "yyyy-MM-dd");
                        }
                        if (item.contains("price")) {
                            price = item.value("price").toDouble();
                        }
                        if (item.contains("numbers")) {
                            numbers = item.value("numbers").toInt();
                        }
                        // 添加图书
                        addNewBook(name, author, type, press, pubTime, price, numbers);
                        ans.push_back(books.back());
                    }
                }
            }
        } else
            throw std::invalid_argument("JsonParseError");
        return ans;
    }
    // 删除器
public:
    // 删除某个图书(依据ID)
    // 没找到图书时抛出异常
    void removeBook(int id) {
        // 查找第一个ID大于或等于它的书，返回迭代器
        auto pToRemove = std::lower_bound(books.begin(), books.end(), Book{id});
        if (pToRemove == books.end() || pToRemove->getId() != id)
            // 没找到书时抛出异常
            throw std::invalid_argument{"Can not remove invalid book"};
        else
            books.erase(pToRemove);
    }

    // 修改器
public:
    // 修改某本书(依据ID)的信息
    // newBook 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    // 修改成功, 返回修改后的书本信息
    const Book &updateBook(int id, const Book &newBook) {
        // 查找第一个ID大于或等于它的书，返回迭代器
        auto pToUpdate = std::lower_bound(books.begin(), books.end(), Book{id});
        if (pToUpdate == books.end() || pToUpdate->getId() != id)
            // 没找到书时抛出异常
            throw std::invalid_argument{"Can not update invalid book"};
        else {
            pToUpdate->update(newBook);
            return *pToUpdate;
        }
    }

    // 借书
    // 失败会抛出异常(没有此书，书库存不够)
    void borrow(int BookID) {
        auto &book = selectBookByID(BookID);
        book.reduceNumbers(1);
    }

    // 还书
    // 失败会抛出异常(没有此书)
    void back(int BookID) {
        auto &book = selectBookByID(BookID);
        book.addNumbers(1);
    }
};
#endif //BOOKTABLE_H
