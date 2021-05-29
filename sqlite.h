#ifndef SQLITE_H
#define SQLITE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "productitem.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <time.h>
#include <QVariant>
#include "user.h"
#include "usermanager.h"
#include "log.h"

#define LACKOFBALANCE 1
#define LACKOFPRODUCT 2
#define PAYORDERCANCELED 3
#define ORDERPAID 1
#define ORDERCANCELED 2

class Sqlite
{
public:
    Sqlite();
    // 打开数据库
    bool openDb(void);
    // 创建数据表
    void createTable(void) const;
    // 判断数据表是否存在
    bool isTableExist(QString &tableName) const;
    // 查询全部数据
    vector<productItem *> queryTable(string LIKE = "", string SORT = "", int productId = -1) const;
    // 插入单条数据
    void singleInsertData(productItem item) const;
    void modifyItemInCart(int productId, int userId, int number = -1, bool checked = true);
    void deleteItemFromCart(int productId, int userId);
    void queryCart(int userId, vector<productItem *> &productList, vector<int> &numberList, vector<bool> &checkedList);
    // 修改数据
    void modifyData(productItem item, int updateImage) const;
    // 删除数据
    void deleteData(int id) const;
    void newDiscount(int id) const;
    vector<vector<double>> getDiscount() const;
    void setDiscount(vector<vector<double>> discount) const;
    int buyOne(int userId, int productId);
    int generateOrder(int userId);
    void getOrder(int orderId, bool &canceled, bool &paied, long long &time, int &userId, vector<productItem *> &orderList, vector<int> &count, vector<double> &price, double &priceSum);
    int payOrder(int orderId);
    int cancelOrder(int orderId);
    void getOrderList(int userId, vector<int> &orderId, vector<double> &priceSum, vector<long long> &time, vector<bool> &paid, vector<bool> &canceled);
    // 关闭数据库
    void closeDb(void);

private:
    QSqlDatabase db;
};

#endif // SQLITE_H
