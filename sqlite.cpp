#include "sqlite.h"
#include <QDebug>

/* 构造函数设置数据库的连接参数 */
Sqlite::Sqlite()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        // 建立和SQlite数据库的连接
        db = QSqlDatabase::addDatabase("QSQLITE");
        // 设置数据库文件的名字
        db.setDatabaseName("E-Commerce-Platform.db");
    }
}

/* 打开数据库 */
bool Sqlite::openDb()
{
    if (!db.open())
    {
        qDebug() << "Error: Failed to connect database." << db.lastError();
    }
    else
    {
        QString tableName = "productItem";
        if (!isTableExist(tableName))
        {
            createTable();
        }
    }
    return true;
}

/* 判断数据库中某个数据表是否存在 */
bool Sqlite::isTableExist(QString &tableName) const
{
    if (db.tables().contains(tableName))
    {
        return true;
    }
    return false;
}

/* 创建数据表 */
void Sqlite::createTable() const
{
    // 用于执行sql语句的对象
    QSqlQuery sqlQuery;
    // 构建创建数据库的sql语句字符串
    QString createSql = QString("CREATE TABLE `productItem` (`id` INTEGER  PRIMARY KEY,`name` TEXT NOT NULL,`description` TEXT,`price` DOUBLE(32,2) NOT NULL,`remaining` INTEGER  NOT NULL,`mainPhoto` INTEGER ,`type` INTEGER  NOT NULL, `deleted` BOOLEAN DEFAULT false, `seller` INTEGER  NOT NULL);");
    sqlQuery.prepare(createSql);
    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    // 构建创建数据库的sql语句字符串
    createSql = QString("CREATE TABLE `productPhoto` (`id` INTEGER PRIMARY KEY, `productId` INTEGER NOT NULL, `photo` BLOB NOT NULL);");
    sqlQuery.prepare(createSql);
    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    createSql = QString("CREATE TABLE `discount` (`fooddiscount` DOUBLE(32,2) NOT NULL,`clothesdiscount` DOUBLE(32,2) NOT NULL,`bookdiscount` DOUBLE(32,2) NOT NULL, `seller` INTEGER PRIMARY KEY NOT NULL);");
    sqlQuery.prepare(createSql);
    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    createSql = QString("CREATE TABLE `cart` (`id` INTEGER PRIMARY KEY, `userId` INTEGER NOT NULL,`productId` INTEGER NOT NULL, `number` INTEGER NOT NULL, `checked` BOOLEAN DEFAULT true);");
    sqlQuery.prepare(createSql);
    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    createSql = QString("CREATE TABLE `order` (`id` INTEGER PRIMARY KEY, `userId` INTEGER NOT NULL,`price` INTEGER NOT NULL, `time` INTEGER NOT NULL, `paied` BOOLEAN DEFAULT false);");
    sqlQuery.prepare(createSql);
    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    createSql = QString("CREATE TABLE `orderItem` (`id` INTEGER PRIMARY KEY, `orderId` INTEGER NOT NULL,`productId` INTEGER NOT NULL, `price` BOOLEAN DEFAULT false, `number` INTEGER NOT NULL);");
    sqlQuery.prepare(createSql);
    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
}

/* 插入单条数据 */
void Sqlite::singleInsertData(productItem item) const
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO `productItem` (`name`,`description`,`price`,`remaining`,`mainPhoto`,`type`, `seller`) VALUES (:name, :description, :price, :remaining, :mainPhoto, :type, :seller)");
    sqlQuery.bindValue(":name", item.name.c_str());
    sqlQuery.bindValue(":description", item.description.c_str());
    sqlQuery.bindValue(":price", item.price);
    sqlQuery.bindValue(":remaining", item.remaining);
    sqlQuery.bindValue(":mainPhoto", item.mainPhoto);
    sqlQuery.bindValue(":type", item.type);
    sqlQuery.bindValue(":seller", item.seller);

    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to insert. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Insert successed!" << sqlQuery.lastInsertId().toInt();
    }
    int productId = sqlQuery.lastInsertId().toInt();
    for (int i = 0; i < int(item.photo.size()); i++)
    {
        sqlQuery.prepare("INSERT INTO `productPhoto` (`productId`,`photo`) VALUES (:productId, :photo)");
        sqlQuery.bindValue(":productId", productId);
        sqlQuery.bindValue(":photo", item.photo[i]);
        if (!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to insert. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Insert successed!" << sqlQuery.lastInsertId().toInt();
        }
    }
}

/* 加购物车 */
void Sqlite::modifyItemInCart(int productId, int userId, int number, bool checked)
{
    QSqlQuery sqlQuery;
    bool exist = false;
    sqlQuery.prepare("SELECT * FROM `cart` WHERE `userId`==:userId AND `productId`==:productId ");
    sqlQuery.bindValue(":userId", userId);
    sqlQuery.bindValue(":productId", productId);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while (sqlQuery.next())
        {
            if (number == -1)
            {
                number = sqlQuery.value(3).toInt() + 1;
            }
            QSqlQuery sqlQuery2;
            sqlQuery2.prepare("UPDATE `cart` SET "
                             "`userId`=:userId,"
                             "`productId`=:productId,"
                             "`number`=:number,"
                             "`checked`=:checked "
                             "WHERE `id`==:id;");
            sqlQuery2.bindValue(":userId", userId);
            sqlQuery2.bindValue(":productId", productId);
            sqlQuery2.bindValue(":number", number);
            sqlQuery2.bindValue(":checked", checked);
            sqlQuery2.bindValue(":id", sqlQuery.value(0).toInt());
            if (!sqlQuery2.exec())
            {
                qDebug() << "Error: Fail to update. " << sqlQuery2.lastError();
            }
            else
            {
                qDebug() << "Update successed!";
            }
            exist = true;
        }
    }
    if (!exist)
    {
        if (number == -1)
        {
            number = 1;
        }
        sqlQuery.prepare("INSERT INTO `cart` (`userId`,`productId`, `number`) VALUES (:userId, :productId, :number)");
        sqlQuery.bindValue(":userId", userId);
        sqlQuery.bindValue(":productId", productId);
        sqlQuery.bindValue(":number", number);

        // 执行sql语句
        if (!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to insert. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Insert successed!" << sqlQuery.lastInsertId().toInt();
        }
    }
}

/* 删除购物车物品 */
void Sqlite::deleteItemFromCart(int productId, int userId)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("DELETE FROM `cart` "
                     "WHERE `productId`=:productId AND `userId`==:userId ;");
    sqlQuery.bindValue(":productId", productId);
    sqlQuery.bindValue(":userId", userId);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to delete item from cart. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Delete item from cart successed!" << sqlQuery.lastInsertId().toInt();
    }
}

/* 列出购物车 */
void Sqlite::queryCart(int userId, vector<productItem *> &productList, vector<int> &numberList, vector<bool> &checkedList)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT * FROM `cart` WHERE `userId`==:userId ");
    sqlQuery.bindValue(":userId", userId);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while (sqlQuery.next())
        {
            QSqlQuery sqlQuery2;
            sqlQuery2.prepare("SELECT * FROM `productItem` WHERE `id`==:productId ");
            sqlQuery2.bindValue(":productId", sqlQuery.value(2).toInt());
            sqlQuery2.exec();
            while (sqlQuery2.next())
            {
                productItem *tmp;
                switch (sqlQuery2.value(6).toInt())
                {
                case FOODTYPE:
                    tmp = new foodItem;
                    break;
                case CLOTHESTYPE:
                    tmp = new clothesItem;
                    break;
                case BOOKTYPE:
                    tmp = new bookItem;
                    break;
                default:
                    tmp = new productItem;
                }
                tmp->id = sqlQuery2.value(0).toInt();
                tmp->name = sqlQuery2.value(1).toString().toStdString();
                tmp->description = sqlQuery2.value(2).toString().toStdString();
                tmp->price = sqlQuery2.value(3).toDouble();
                tmp->remaining = sqlQuery2.value(4).toInt();
                tmp->mainPhoto = sqlQuery2.value(5).toInt();
                tmp->type = sqlQuery2.value(6).toInt();
                tmp->seller = sqlQuery2.value(8).toInt();
                QSqlQuery sqlQueryPhoto;
                sqlQueryPhoto.exec("SELECT * FROM `productPhoto` WHERE `productId` LIKE " + sqlQuery2.value(0).toString());
                while (sqlQueryPhoto.next())
                {
                    tmp->photo.push_back(sqlQueryPhoto.value(2).toByteArray().toBase64());
                }
                productList.push_back(tmp);
                numberList.push_back(sqlQuery.value(3).toInt());
                checkedList.push_back(sqlQuery.value(4).toBool());
            }
        }
    }
}

/* 查询全部数据 */
vector<productItem *> Sqlite::queryTable(string LIKE, string SORT, int productId) const
{
    vector<productItem *> productList;
    QSqlQuery sqlQuery;
    string sqlCommand = "SELECT * FROM `productItem` WHERE `deleted`==false ";
    if (LIKE != "")
    {
        sqlCommand += " AND (`name` LIKE '%" + LIKE + "%' OR `description` LIKE '%" + LIKE + "%')";
    }
    if (productId != -1)
    {
        sqlCommand += " AND `id`==" + to_string(productId) + ";";
    }
    sqlCommand += SORT;
    qDebug() << sqlCommand.c_str();
    sqlQuery.exec(sqlCommand.c_str());
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while (sqlQuery.next())
        {
            productItem *tmp;
            switch (sqlQuery.value(6).toInt())
            {
            case FOODTYPE:
                tmp = new foodItem;
                break;
            case CLOTHESTYPE:
                tmp = new clothesItem;
                break;
            case BOOKTYPE:
                tmp = new bookItem;
                break;
            default:
                tmp = new productItem;
            }

            tmp->id = sqlQuery.value(0).toInt();
            tmp->name = sqlQuery.value(1).toString().toStdString();
            tmp->description = sqlQuery.value(2).toString().toStdString();
            tmp->price = sqlQuery.value(3).toDouble();
            tmp->remaining = sqlQuery.value(4).toInt();
            tmp->mainPhoto = sqlQuery.value(5).toInt();
            tmp->type = sqlQuery.value(6).toInt();
            tmp->seller = sqlQuery.value(8).toInt();
            QSqlQuery sqlQueryPhoto;
            sqlQueryPhoto.exec("SELECT * FROM `productPhoto` WHERE `productId` LIKE " + sqlQuery.value(0).toString());
            while (sqlQueryPhoto.next())
            {
                tmp->photo.push_back(sqlQueryPhoto.value(2).toByteArray().toBase64());
            }
            productList.push_back(tmp);
        }
    }
    return productList;
}

/* 更新单条数据 */
void Sqlite::modifyData(productItem item, int updateImage) const
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("UPDATE `productItem` SET "
                     "`name`=:name,"
                     "`description`=:description,"
                     "`price`=:price,"
                     "`remaining`=:remaining,"
                     "`mainPhoto`=:mainPhoto,"
                     "`type`=:type "
                     "WHERE `id`=:id;");
    sqlQuery.bindValue(":name", item.name.c_str());
    sqlQuery.bindValue(":description", item.description.c_str());
    sqlQuery.bindValue(":price", item.price);
    sqlQuery.bindValue(":remaining", item.remaining);
    sqlQuery.bindValue(":mainPhoto", item.mainPhoto);
    sqlQuery.bindValue(":type", item.type);
    sqlQuery.bindValue(":id", item.id);

    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to update. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Update successed!";
    }
    if (updateImage) //对图片更改进行处理
    {
        const int productId = item.id;
        sqlQuery.prepare("DELETE FROM `productPhoto` "
                         "WHERE `productId`=:productId;");
        sqlQuery.bindValue(":productId", productId);
        if (!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to delete old photos. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Old photos deleting successed!";
        }
        for (int i = 0; i < int(item.photo.size()); i++)
        {
            sqlQuery.prepare("INSERT INTO `productPhoto` (`productId`,`photo`) VALUES (:productId, :photo)");
            sqlQuery.bindValue(":productId", productId);
            sqlQuery.bindValue(":photo", item.photo[i]);
            if (!sqlQuery.exec())
            {
                qDebug() << "Error: Fail to update photos. " << sqlQuery.lastError();
            }
            else
            {
                qDebug() << "Photos update successed!";
            }
        }
    }
}

/* 删除单条数据 */
void Sqlite::deleteData(int id) const
{
    QSqlQuery sqlQuery;

    sqlQuery.prepare("UPDATE `productItem` SET "
                     "`deleted`=true "
                     "WHERE `id`=:id;");
    sqlQuery.bindValue(":id", id);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to delete. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Deleting successed!";
    }
}

/* 新账户初始化 */
void Sqlite::newDiscount(int id) const
{
    QSqlQuery sqlQuery;
    QString createSql = QString("INSERT INTO `discount` (`fooddiscount`,`clothesdiscount`, `bookdiscount`, `seller`) VALUES (1, 1, 1, :seller)");
    sqlQuery.prepare(createSql);
    sqlQuery.bindValue(":seller", id);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to insert discount. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Insert success!";
    }
}

/* 获取所有折扣 */
vector<vector<double>> Sqlite::getDiscount() const
{
    vector<vector<double>> discount;
    QSqlQuery sqlQuery;
    QString sqlCommand = "SELECT * FROM `discount`;";
    sqlQuery.prepare(sqlCommand);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while (sqlQuery.next())
        {
            vector<double> tmp;
            tmp.push_back(sqlQuery.value(0).toDouble());
            tmp.push_back(sqlQuery.value(1).toDouble());
            tmp.push_back(sqlQuery.value(2).toDouble());
            tmp.push_back(sqlQuery.value(3).toInt());
            discount.push_back(tmp);
        }
    }
    return discount;
}

/* 保存折扣 */
void Sqlite::setDiscount(vector<vector<double>> discount) const
{
    QSqlQuery sqlQuery;
    for (int i = 0; i < (int)discount.size(); i++)
    {
        QString createSql = QString("UPDATE `discount` SET `fooddiscount`=:fooddiscount, `clothesdiscount`=:clothesdiscount, `bookdiscount`=:bookdiscount WHERE `seller`=:seller;");
        sqlQuery.prepare(createSql);
        sqlQuery.bindValue(":seller", discount[i][3]);
        sqlQuery.bindValue(":fooddiscount", discount[i][0]);
        sqlQuery.bindValue(":clothesdiscount", discount[i][1]);
        sqlQuery.bindValue(":bookdiscount", discount[i][2]);
        if (!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Table created!";
        }
    }
}

int Sqlite::generateOrder(int userId)
{
    vector<productItem *> productList;
    vector<int> numberList;
    vector<bool> checkedList;
    vector<productItem> orderList;
    vector<int> count;
    vector<double> price;
    vector<vector<double>> discount = getDiscount();
    queryCart(userId, productList, numberList, checkedList);

    double priceSum = 0;
    for (int i = 0; i < (int)productList.size(); i++)
    {
        if (numberList[i] > productList[i]->remaining)
        {
            for (int j = 0; j < i; j++)
            {
                if (checkedList[i])
                {
                    productList[i]->remaining+=numberList[i];

                    modifyData(*productList[i], 0);

                }
            }
            break;
        }
        if (checkedList[i])
        {
            priceSum += productList[i]->getPrice(discount) * numberList[i];
            orderList.push_back(*productList[i]);
            count.push_back(numberList[i]);
            price.push_back(productList[i]->getPrice(discount));
            productList[i]->remaining-=numberList[i];

            modifyData(*productList[i], 0);

        }
    }
    int orderId;
    QSqlQuery sqlQuery;
    //(`id` INTEGER PRIMARY KEY, `userId` INTEGER NOT NULL,`price` INTEGER NOT NULL, `time` INTEGER NOT NULL, `paied` BOOLEAN DEFAULT false)
    sqlQuery.prepare("INSERT INTO `order` (`userId`, `price`, `time`) VALUES (:userId, :price, :time)");
    sqlQuery.bindValue(":userId", userId);
    sqlQuery.bindValue(":price", priceSum);
    time_t t;
    time(&t);
    qDebug() << t;
    sqlQuery.bindValue(":time", QVariant::fromValue(t));
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to generate order. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Order generated!";
        orderId = sqlQuery.lastInsertId().toInt();
    }
    for (int i = 0; i < (int)orderList.size(); i++)
    {
        deleteItemFromCart(orderList[i].id, userId);
        sqlQuery.prepare("INSERT INTO `orderItem` (`orderId`, `productId`, `price`, `number`) VALUES (:orderId, :productId, :price, :number)");
        sqlQuery.bindValue(":orderId", orderId);
        sqlQuery.bindValue(":productId", orderList[i].id);
        sqlQuery.bindValue(":price", price[i]);
        sqlQuery.bindValue(":number", count[i]);
        if (!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to generate order. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Order generated!";
        }
    }
    return orderId;
}

void Sqlite::buyOne(int userId, int productId)
{
    vector<productItem *> productList = queryTable("","",productId);
    vector<int> numberList;
    numberList.push_back(1);
    vector<bool> checkedList;
    checkedList.push_back(true);

    vector<productItem> orderList;
    vector<int> count;
    vector<double> price;
    vector<vector<double>> discount = getDiscount();

    double priceSum = 0;
    for (int i = 0; i < (int)productList.size(); i++)
    {
        if (numberList[i] > productList[i]->remaining)
        {
            for (int j = 0; j < i; j++)
            {
                if (checkedList[i])
                {
                    productList[i]->remaining+=numberList[i];

                    modifyData(*productList[i], 0);

                }
            }
            break;
        }
        if (checkedList[i])
        {
            priceSum += productList[i]->getPrice(discount) * numberList[i];
            orderList.push_back(*productList[i]);
            count.push_back(numberList[i]);
            price.push_back(productList[i]->getPrice(discount));
            productList[i]->remaining-=numberList[i];

            modifyData(*productList[i], 0);

        }
    }
    int orderId;
    QSqlQuery sqlQuery;
    //(`id` INTEGER PRIMARY KEY, `userId` INTEGER NOT NULL,`price` INTEGER NOT NULL, `time` INTEGER NOT NULL, `paied` BOOLEAN DEFAULT false)
    sqlQuery.prepare("INSERT INTO `order` (`userId`, `price`, `time`) VALUES (:userId, :price, :time)");
    sqlQuery.bindValue(":userId", userId);
    sqlQuery.bindValue(":price", priceSum);
    time_t t;
    time(&t);
    qDebug() << t;
    sqlQuery.bindValue(":time", QVariant::fromValue(t));
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to generate order. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Order generated!";
        orderId = sqlQuery.lastInsertId().toInt();
    }
    for (int i = 0; i < (int)orderList.size(); i++)
    {
        deleteItemFromCart(orderList[i].id, userId);
        sqlQuery.prepare("INSERT INTO `orderItem` (`orderId`, `productId`, `price`, `number`) VALUES (:orderId, :productId, :price, :number)");
        sqlQuery.bindValue(":orderId", orderId);
        sqlQuery.bindValue(":productId", orderList[i].id);
        sqlQuery.bindValue(":price", price[i]);
        sqlQuery.bindValue(":number", count[i]);
        if (!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to generate order. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Order generated!";
        }
    }
    payOrder(orderId);
}

void Sqlite::getOrder(int orderId, bool &paied, long long &time, int &userId, vector<productItem *> &orderList, vector<int> &count, vector<double> &price, double &priceSum)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT * FROM `order` WHERE `id`==:orderId");
    sqlQuery.bindValue(":orderId", orderId);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while (sqlQuery.next())
        {
            paied = sqlQuery.value(4).toBool();
            time = sqlQuery.value(3).toLongLong();
            priceSum = sqlQuery.value(2).toDouble();
            userId = sqlQuery.value(1).toInt();
            QSqlQuery sqlQuery2;
            sqlQuery2.prepare("SELECT * FROM `orderItem` WHERE `orderId`==:orderId ");
            sqlQuery2.bindValue(":orderId", sqlQuery.value(0).toInt());
            sqlQuery2.exec();
            while (sqlQuery2.next())
            {
                QSqlQuery sqlQuery3;
                sqlQuery3.prepare("SELECT * FROM `productItem` WHERE `id`==:productId ");
                sqlQuery3.bindValue(":productId", sqlQuery2.value(2).toInt());
                sqlQuery3.exec();
                while (sqlQuery3.next())
                {
                    productItem *tmp;
                    switch (sqlQuery3.value(6).toInt())
                    {
                    case FOODTYPE:
                        tmp = new foodItem;
                        break;
                    case CLOTHESTYPE:
                        tmp = new clothesItem;
                        break;
                    case BOOKTYPE:
                        tmp = new bookItem;
                        break;
                    default:
                        tmp = new productItem;
                    }
                    tmp->id = sqlQuery3.value(0).toInt();
                    tmp->name = sqlQuery3.value(1).toString().toStdString();
                    tmp->description = sqlQuery3.value(2).toString().toStdString();
                    tmp->price = sqlQuery3.value(3).toDouble();
                    tmp->remaining = sqlQuery3.value(4).toInt();
                    tmp->mainPhoto = sqlQuery3.value(5).toInt();
                    tmp->type = sqlQuery3.value(6).toInt();
                    tmp->seller = sqlQuery3.value(8).toInt();
                    QSqlQuery sqlQueryPhoto;
                    sqlQueryPhoto.exec("SELECT * FROM `productPhoto` WHERE `productId` LIKE " + sqlQuery3.value(0).toString());
                    while (sqlQueryPhoto.next())
                    {
                        tmp->photo.push_back(sqlQueryPhoto.value(2).toByteArray().toBase64());
                    }
                    //(`id` INTEGER PRIMARY KEY, `orderId` INTEGER NOT NULL,`productId` INTEGER NOT NULL, `price` BOOLEAN DEFAULT false, `number` INTEGER NOT NULL)
                    orderList.push_back(tmp);
                    price.push_back(sqlQuery2.value(3).toDouble());
                    count.push_back(sqlQuery2.value(4).toInt());
                }
            }
        }
    }
}

void Sqlite::payOrder(int orderId)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("UPDATE `order` SET "
                     "`paied`=true "
                     "WHERE `id`==:id;");
    sqlQuery.bindValue(":id", orderId);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to pay order. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Order paied!";
    }
}


void Sqlite::getOrderList(int userId, vector<int> &orderId, vector<double> &priceSum, vector<long long> &time, vector<bool> &paid)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT * FROM `order` WHERE `userId`==:userId  ORDER BY `time` DESC");
    sqlQuery.bindValue(":userId", userId);
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to get orders. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Order get!";
        while (sqlQuery.next())
        {
            orderId.push_back(sqlQuery.value(0).toInt());
            priceSum.push_back(sqlQuery.value(2).toDouble());
            time.push_back(sqlQuery.value(3).toLongLong());
            paid.push_back(sqlQuery.value(4).toBool());
        }
    }
}






















/* 关闭数据库 */
void Sqlite::closeDb(void)
{
    db.close();
}
