#ifndef BOOK_H
#define BOOK_H

#include <QDate>

class Book {
private:
    int id;         // 唯一编号
    QString name;   // 书名
    QString author; // 作者
    QString type;   // 类别
    QString press;  // 出版社
    QDate pubDate;  // 出版时间
    double price;   // 价格
    int numbers;    // 库存
public:
    // 默认构造函数
    explicit Book(int id = 0,
                  const QString &name = "",
                  const QString &author = "",
                  const QString &type = "",
                  const QString &press = "",
                  const QDate &pubDate = QDate{0, 0, 0},
                  double price = 0.0, int numbers = 0)
        : id(id), name(name), author(author),
          type(type), press(press),
          pubDate(pubDate), price(price),
          numbers(numbers) {}

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

    const QString &getAuthor() const {
        return author;
    }

    void setAuthor(const QString &author) {
        this->author = author;
    }

    const QString &getType() const {
        return type;
    }

    void setType(const QString &type) {
        this->type = type;
    }

    const QString &getPress() const {
        return press;
    }

    void setPress(const QString &press) {
        this->press = press;
    }

    const QDate &getPubDate() const {
        return pubDate;
    }

    void setPubDate(const QDate &pubDate) {
        this->pubDate = pubDate;
    }

    double getPrice() const {
        return price;
    }

    void setPrice(double price) {
        this->price = price;
    }

    int getNumbers() const {
        return numbers;
    }

    void setNumbers(int numbers) {
        this->numbers = numbers;
    }

    // 增加库存
    void addNumbers(int number) {
        numbers += number;
    }

    // 减少库存
    // 减少数量大于当前库存，将抛出异常
    void reduceNumbers(int number) {
        if (number <= numbers)
            numbers -= number;
        else
            throw std::invalid_argument("To reduce number is larger than current number");
    }

    // 判断是否是相同的书(依据ID)
    bool operator==(const Book &rhs) const {
        return this->id == rhs.id;
    }

    // ID比较
    bool operator<(const Book &rhs) const {
        return id < rhs.id;
    }

    // 修改书本信息
    // newBook 忽略ID属性, 即不会修改ID, 其余字段如果为空或者非法值，忽略修改
    void update(const Book &newBook) {
        if (!newBook.name.isEmpty())
            name = newBook.name;
        if (!newBook.author.isEmpty())
            author = newBook.author;
        if (!newBook.type.isEmpty())
            type = newBook.type;
        if (newBook.getPubDate().isValid())
            pubDate = newBook.getPubDate();
        if (newBook.price >= 0)
            price = newBook.price;
        if (newBook.numbers >= 0)
            numbers = newBook.numbers;
    }

    // 和本书是否是相同属性
    // 多字段比较，忽略空字段或非法字段
    bool isSameProperties(const Book &book) const {
        // 字段全空，另外判断
        if (book.name.isEmpty() && book.author.isEmpty() && book.type.isEmpty() && !book.pubDate.isValid() && book.price < 0)
            return false;

        bool result = true;
        if (!book.name.isEmpty())
            result = result && name == book.name;
        if (!book.author.isEmpty())
            result = result && author == book.author;
        if (!book.type.isEmpty())
            result = result && type == book.type;
        if (book.pubDate.isValid())
            result = result && pubDate == book.pubDate;
        if (book.price >= 0)
            result = result && price == book.price;
        return result;
    }

    // 和本书是否是相同属性
    // 只要有一个字段符合即可
    bool hasSameProperties(const Book &book) const {
        bool result = false;
        if (!book.name.isEmpty())
            result = result || name == book.name;
        if (!book.author.isEmpty())
            result = result || author == book.author;
        if (!book.type.isEmpty())
            result = result || type == book.type;
        if (!book.pubDate.isValid())
            result = result || pubDate == book.pubDate;
        if (book.price >= 0)
            result = result || price == book.price;
        return result;
    }

    // 转换为字符串
    QString toString() const {
        return QString{"Book{" + QString::number(id) + "," + name + "," + author + "," + type + "," + press + "," + pubDate.toString("yyyy-MM-dd") + "," + QString::number(price) + "," + QString::number(numbers) + "}"};
    }
};

#endif // BOOK_H
