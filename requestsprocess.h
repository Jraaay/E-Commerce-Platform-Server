#ifndef REQUESTSPROCESS_H
#define REQUESTSPROCESS_H

#define SQLITE_singleInsertData 0
#define SQLITE_modifyItemInCart 1
#define SQLITE_deleteItemFromCart 2
#define SQLITE_queryCart 3
#define SQLITE_queryTable 4
#define SQLITE_modifyData 5
#define SQLITE_deleteData 6
#define SQLITE_newDiscount 7
#define SQLITE_setDiscount 8
#define SQLITE_generateOrder 9
#define SQLITE_getOrder 10
#define SQLITE_getOrderList 11
#define USER_createUser 12
#define USER_changeUserName 13
#define USER_loginCheck 14
#define USER_getUser 15
#define pay 16
#define SQLITE_getDiscount 17
#define SQLITE_buyOneThing 18
#define USER_recharge 19
#define USER_changePassword 20

#include <QObject>
#include <QJsonObject>
#include <QJsonParseError>
#include "sqlite.h"
#include "productitem.h"
#include <QJsonArray>
#include "usermanager.h"
#include "user.h"

using namespace std;

class RequestsProcess : public QObject
{
    Q_OBJECT
public:
    explicit RequestsProcess(void *father, QObject *parent = nullptr);
    static void process(string jsonStr, void *father, void *ui);
    void *_father = nullptr;

signals:


};

#endif // REQUESTSPROCESS_H
