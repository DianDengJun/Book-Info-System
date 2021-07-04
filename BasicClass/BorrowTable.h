#ifndef BORROWTABLE_H
#define BORROWTABLE_H

#include "Book.h"
#include "BorrowRecord.h"
#include "User.h"
#include <vector>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class BorrowTable {
    // 符号引入
public:
    using Record = BorrowRecord;
    using Table = std::vector<Record>;
    using Status = Record::Status;

    // 属性
private:
    Table records;

    // 构造
public:
    // 默认构造函数
    BorrowTable() = default;

    // 查找与遍历
public:
    // 提取出所有记录信息
    const Table &getAllRecords() const {
        return records;
    }

    // 获取某个读者的所有记录
    Table getAllRecordsByReaderID(int id) const {
        Table ans;
        for (const auto &r : records)
            if (r.isSameProperties(Record{id, -1, QDate{0, 0, 0}, QDate{0, 0, 0}, Status::Undefined}))
                ans.push_back(r);

        return ans;
    }

    // 获取某个读者正在借阅的所有记录
    Table getBorrowingRecordsByReaderID(int id) const {
        Table ans;
        for (const auto &r : records)
            if (r.isSameProperties(Record{id, -1, QDate{0, 0, 0}, QDate{0, 0, 0}, Status::Borrowing}))
                ans.push_back(r);

        return ans;
    }

    // 获取某本书正在借阅的数量
    int getBorrowingNumbers(int BookId) const {
        int numbers = 0;
        for (const auto &r : records)
            if (r.isSameProperties(BorrowRecord{-1, BookId, QDate{0, 0, 0}, QDate{0, 0, 0}, Status::Borrowing})) {
                numbers++;
            }
        return numbers;
    }

    // 是否有记录
    bool hasRecord(int readerID, int bookID) const {
        for (const auto &record : records) {
            if (record.equals(readerID, bookID))
                return true;
        }
        return false;
    }

    // 是否有记录
    bool hasRecord(int readerID, int bookID, Status status) const {
        for (const auto &record : records) {
            if (record.equals(readerID, bookID, status))
                return true;
        }
        return false;
    }

    // 是否有记录
    bool hasRecord(const Reader &reader, const Book &book) const {
        return hasRecord(reader.getId(), book.getId());
    }

    // 是否有记录
    bool hasRecord(const BorrowRecord &record) const {
        return hasRecord(record.getReaderId(), record.getBookId(), record.getStatus());
    }

    // 是否有正在借阅中的记录
    bool hasBorrowingRecord(int readerID, int bookID) const {
        return hasRecord(readerID, bookID, Status::Borrowing);
    }

    // 精准查询
    Table select(const BorrowRecord &record) const {
        // 查询结果集合
        Table ans;

        // 依次查询匹配
        for (const auto &r : records)
            if (r.isSameProperties(record))
                ans.push_back(r);

        return ans;
    }

    // 模糊查询
    Table vagueSelect(const BorrowRecord &record) const {
        // 查询结果集合
        Table ans;

        // 依次查询匹配
        for (const auto &r : records)
            if (r.hasSameProperties(record))
                ans.push_back(r);

        return ans;
    }

    // 选定某个正在借阅中的记录
    BorrowRecord &selectBorrowingRecordByID(int ReaderID, int BookID) {
        for (auto &record : records) {
            if (record.equals(ReaderID, BookID, Status::Borrowing))
                return record;
        }
        // 找不到抛出异常
        throw std::invalid_argument("Can not locate this Borrow Record");
    }

    // 空表检测
    bool empty() const {
        return records.empty();
    }

    // 添加器
public:
    // 添加新借阅记录
    // 如果有正在借阅中的记录，则会抛出异常
    void addBorrowingRecord(int readerID, int bookID) {
        if (!hasRecord(readerID, bookID, Status::Borrowing))
            records.emplace_back(readerID, bookID, QDate::currentDate(), QDate{0, 0, 0}, Status::Borrowing);
        else
            throw std::invalid_argument("Has borrowing record.");
    }

    // 直接添加记录
    void addRecord(const Record record) {
        records.push_back(record);
    }

    // 从JSON文件初始化借阅信息
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
                if (object.contains("records") && object.value("records").isArray()) {
                    // 转换为数组
                    QJsonArray jsonRecords = object.value("records").toArray();
                    int readerID;
                    int bookID;
                    QDate borrowDate;
                    QDate returnDate;
                    BorrowRecord::Status status;
                    for (auto &&jsonRecord : jsonRecords) {
                        QJsonObject item = jsonRecord.toObject();
                        if (item.contains("reader ID")) {
                            readerID = item.value("reader ID").toInt();
                        }
                        if (item.contains("book ID")) {
                            bookID = item.value("book ID").toInt();
                        }
                        if (item.contains("borrow date")) {
                            borrowDate = QDate::fromString(item.value("borrow date").toString(), "yyyy-MM-dd");
                        }
                        if (item.contains("return date")) {
                            returnDate = QDate::fromString(item.value("return date").toString(), "yyyy-MM-dd");
                        }
                        if (item.contains("status")) {
                            status = item.value("status").toString() == "Borrowing" ? BorrowRecord::Status::Borrowing : BorrowRecord::Status::Returned;
                        }
                        // 添加记录
                        addRecord(Record{readerID, bookID, borrowDate, returnDate, status});
                    }
                }
            }
        } else
            throw std::invalid_argument("JsonParseError");
    }


    // 修改器
public:
    // 完成借阅记录(即还书)
    // 没找到记录会抛出异常
    const BorrowRecord &completeRecord(int ReaderID, int BookID) {
        // 定位记录
        auto &record = selectBorrowingRecordByID(ReaderID, BookID);

        // 完成还书
        record.completeBack();

        // 返回记录
        return record;
    }
};

#endif // BORROWTABLE_H
