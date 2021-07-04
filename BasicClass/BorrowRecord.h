#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <QDate>
#include <utility>

class BorrowRecord {
public:
    // 借阅状态枚举
    enum class Status {
        Undefined,  // 未定义
        Borrowing,  // 借阅中
        Returned    // 已返还
    };
    using IDPair = std::pair<int, int>;
private:
    IDPair recordID;    // 记录ID组合
    QDate borrowDate;   // 借阅日期
    QDate returnDate;   // 返还日期
    Status status;      // 借阅状态
public:
    // 默认构造函数
    BorrowRecord() = default;
    // 有参构造函数
    explicit BorrowRecord(int readerId,
                          int bookId,
                          const QDate &borrowDate,
                          const QDate &returnDate,
                          Status status)
            : recordID{readerId, bookId},
              borrowDate(borrowDate), returnDate(returnDate),
              status(status) {}

    // 简化的构造函数, 用于借阅时构造
    BorrowRecord(int readerId, int bookId)
        :BorrowRecord{readerId, bookId, QDate::currentDate(), QDate{0,0,0}, BorrowRecord::Status::Borrowing} {}

    int getReaderId() const {
        return recordID.first;
    }

    void setReaderId(int readerId) {
        recordID.first = readerId;
    }

    int getBookId() const {
        return recordID.second;
    }

    void setBookId(int bookId) {
        recordID.second = bookId;
    }

    const QDate &getBorrowDate() const {
        return borrowDate;
    }

    void setBorrowDate(const QDate &borrowDate) {
        this->borrowDate = borrowDate;
    }

    const QDate &getReturnDate() const {
        return returnDate;
    }

    void setReturnDate(const QDate &returnDate) {
        this->returnDate = returnDate;
    }

    Status getStatus() const {
        return status;
    }

    void setStatus(Status status) {
        this->status = status;
    }

    bool operator==(const BorrowRecord& record) const {
        return recordID == record.recordID && status == record.status;
    }

    bool operator<(const BorrowRecord& record) const {
        return recordID < record.recordID;
    }

    bool equals(int ReaderID, int BookID) const {
        return recordID.first == ReaderID && recordID.second == BookID;
    }

    bool equals(int ReaderID, int BookID, Status status) const {
        return this->equals(ReaderID, BookID) && this->status == status;
    }

    bool isSameProperties(const BorrowRecord& record) const {
        // 字段全空(无效)，另做判断
        if (record.recordID.first < 0 && record.recordID.second < 0
        && !record.borrowDate.isValid() && !record.returnDate.isValid()
        && record.status == BorrowRecord::Status::Undefined)
            return false;

        bool result = true;
        if (record.recordID.first >= 0)
            result = result && recordID.first == record.recordID.first;
        if (record.recordID.second >= 0)
            result = result && recordID.second == record.recordID.second;
        if (record.borrowDate.isValid())
            result = result && borrowDate == record.borrowDate;
        if (record.returnDate.isValid())
            result = result && returnDate == record.returnDate;
        if (record.status != BorrowRecord::Status::Undefined)
            result = result && status == record.status;
        return result;
    }

    bool hasSameProperties(const BorrowRecord& record) const {
        bool result = false;
        if (record.recordID.first >= 0)
            result = result || recordID.first == record.recordID.first;
        if (record.recordID.second >= 0)
            result = result || recordID.second == record.recordID.second;
        if (record.borrowDate.isValid())
            result = result || borrowDate == record.borrowDate;
        if (record.returnDate.isValid())
            result = result || returnDate == record.returnDate;
        if (record.status != BorrowRecord::Status::Undefined)
            result = result || status == record.status;
        return result;
    }

    // 修改记录
    void update(const BorrowRecord& newRecord) {
        if (newRecord.borrowDate.isValid())
             borrowDate = newRecord.borrowDate;
        if (newRecord.returnDate.isValid())
             returnDate = newRecord.returnDate;
        if (newRecord.status != BorrowRecord::Status::Undefined)
             status = newRecord.status;
    }

    // 完成还书
    void completeBack() {
        returnDate = QDate::currentDate();
        status = Status::Returned;
    }
};


#endif // BORROWRECORD_H
