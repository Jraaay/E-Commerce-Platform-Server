#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <fstream>
#include <string>
#include <QDebug>
#include "user.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "sqlite.h"

#define PASSWORDWRONG 1
#define USERNOTEXIST 2
#define ALREADYEXIST 1


using namespace std;

class userManager
{
public:
    userManager();
    static void changePassword(int userId, string password);
    static void recharge(int userId, double moneyToCharge);
    static int getMaxUid();
    static int loginCheck(int curType, string loginName, string loginPassword, userClass* &curUser);
    static int createUser(int curType, string loginName, string loginPassword);
    static vector<sellerClass> getSellerList();
    static vector<consumerClass> getConsumerList();
    static void getUser(int userId, userClass &user);
    static int changeUserName(int userId, string userName);
};

#endif // USERMANAGER_H
