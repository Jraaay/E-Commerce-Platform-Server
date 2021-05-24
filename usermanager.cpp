#include "usermanager.h"

userManager::userManager()
{

}

void userManager::changePassword(int userId, string password)
{
    int type = -1;
    int numToChange = 0;
    vector<sellerClass> sellerList = getSellerList();
    vector<consumerClass> consumerList = getConsumerList();
    for (int i = 0; i < (int)sellerList.size(); i++)
    {
        if (sellerList[i].uid == userId)
        {
            numToChange = i;
            type = SELLERTYPE;
            break;
        }
    }
    for (int i = 0; i < (int)consumerList.size() && type != -1; i++)
    {
        if (consumerList[i].uid == userId)
        {
            numToChange = i;
            type = CONSUMERTYPE;
            break;
        }
    }
    if (type == SELLERTYPE)
    {
        sellerList[numToChange].setPass(password);
        QJsonArray sellerJsonList;
        for (int i = 0; i < (int)sellerList.size(); i++)
        {
            sellerJsonList.push_back(sellerList[i].getJson());
        }
        QJsonObject object;
        object.insert("data", sellerJsonList);
        QJsonDocument document;
        document.setObject(object);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("sellerFile.json");
        outFile << byteArray.toStdString();
        outFile.close();
    }
    else
    {
        consumerList[numToChange].setPass(password);
        QJsonArray consumerJsonList;
        for (int i = 0; i < (int)consumerList.size(); i++)
        {
            consumerJsonList.push_back(consumerList[i].getJson());
        }
        QJsonObject object;
        object.insert("data", consumerJsonList);
        QJsonDocument document;
        document.setObject(object);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("consumerFile.json");
        outFile << byteArray.toStdString();
        outFile.close();
    }
    return;
}

void userManager::recharge(int userId, double moneyToCharge)
{
    int type = -1;
    int numToChange = 0;
    vector<sellerClass> sellerList = getSellerList();
    vector<consumerClass> consumerList = getConsumerList();
    for (int i = 0; i < (int)sellerList.size(); i++)
    {
        if (sellerList[i].uid == userId)
        {
            numToChange = i;
            type = SELLERTYPE;
            break;
        }
    }
    for (int i = 0; i < (int)consumerList.size() && type != -1; i++)
    {
        if (consumerList[i].uid == userId)
        {
            numToChange = i;
            type = CONSUMERTYPE;
            break;
        }
    }
    if (type == SELLERTYPE)
    {
        sellerList[numToChange].balance += moneyToCharge;
        QJsonArray sellerJsonList;
        for (int i = 0; i < (int)sellerList.size(); i++)
        {
            sellerJsonList.push_back(sellerList[i].getJson());
        }
        QJsonObject object;
        object.insert("data", sellerJsonList);
        QJsonDocument document;
        document.setObject(object);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("sellerFile.json");
        outFile << byteArray.toStdString();
        outFile.close();
    }
    else
    {
        consumerList[numToChange].balance += moneyToCharge;
        QJsonArray consumerJsonList;
        for (int i = 0; i < (int)consumerList.size(); i++)
        {
            consumerJsonList.push_back(consumerList[i].getJson());
        }
        QJsonObject object;
        object.insert("data", consumerJsonList);
        QJsonDocument document;
        document.setObject(object);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("consumerFile.json");
        outFile << byteArray.toStdString();
        outFile.close();
    }
    return;
}

int userManager::createUser(int curType, string loginName, string loginPassword)
{
    int regStatus = -1;
    int uidMax = getMaxUid();
    vector<sellerClass> sellerList = getSellerList();
    vector<consumerClass> consumerList = getConsumerList();
    if (curType == SELLERTYPE)
    {
        sellerList = getSellerList();
    }
    if (curType == CONSUMERTYPE)
    {
        consumerList = getConsumerList();
    }
    if (curType == SELLERTYPE)
    {
        bool only = true;
        for (int i = 0; i < (int)sellerList.size(); i++)
        {
            if (sellerList[i].name == loginName)
            {
                only = false;
                break;
            }
        }
        if (only)
        {
            sellerClass tmp;
            tmp.uid = uidMax + 1;
            uidMax++;
            tmp.name = loginName;
            tmp.setPass(loginPassword);
            tmp.type = SELLERTYPE;
            tmp.balance = 0;
            sellerList.push_back(tmp);
            QJsonArray sellerJsonList;
            for (int i = 0; i < (int)sellerList.size(); i++)
            {
                sellerJsonList.push_back(sellerList[i].getJson());
            }
            QJsonObject object;
            object.insert("data", sellerJsonList);
            QJsonDocument document;
            document.setObject(object);
            QByteArray byteArray = document.toJson(QJsonDocument::Compact);
            ofstream outFile;
            outFile.open("sellerFile.json");
            outFile << byteArray.toStdString();
            outFile.close();
            regStatus = 0;
            QJsonObject object2;
            object2.insert("uid", uidMax);
            QJsonDocument document2;
            document2.setObject(object2);
            QByteArray array2 = document2.toJson(QJsonDocument::Compact);
            ofstream outFile2;
            outFile.open("uidMaxFile.json");
            outFile << array2.toStdString();
            outFile.close();
            Sqlite db;
            db.newDiscount(tmp.uid);
            db.closeDb();
        }
        else
        {
            regStatus = ALREADYEXIST;
        }
    }
    else
    {
        bool only = true;
        for (int i = 0; i < (int)consumerList.size(); i++)
        {
            if (consumerList[i].name == loginName)
            {
                only = false;
                break;
            }
        }
        if (only)
        {
            consumerClass tmp;
            tmp.uid = uidMax + 1;
            uidMax++;
            tmp.name = loginName;
            tmp.setPass(loginPassword);
            tmp.type = CONSUMERTYPE;
            tmp.balance = 0;
            consumerList.push_back(tmp);
            QJsonArray consumerJsonList;
            for (int i = 0; i < (int)consumerList.size(); i++)
            {
                consumerJsonList.push_back(consumerList[i].getJson());
            }
            QJsonObject object;
            object.insert("data", consumerJsonList);
            QJsonDocument document;
            document.setObject(object);
            QByteArray byteArray = document.toJson(QJsonDocument::Compact);
            ofstream outFile;
            outFile.open("consumerFile.json");
            outFile << byteArray.toStdString();
            outFile.close();
            regStatus = 0;

            QJsonObject object2;
            object2.insert("uid", uidMax);
            QJsonDocument document2;
            document2.setObject(object2);
            QByteArray array2 = document.toJson(QJsonDocument::Compact);
            ofstream outFile2;
            outFile.open("uidMaxFile.json");
            outFile << array2.toStdString();
            outFile.close();
        }
        else
        {
            regStatus = ALREADYEXIST;
        }
    }
    return regStatus;
}

int userManager::loginCheck(int curType, string loginName, string loginPassword, userClass* &curUser)
{
    int loginStatus = -1;
    vector<sellerClass> sellerList = getSellerList();
    vector<consumerClass> consumerList = getConsumerList();
    if (curType == SELLERTYPE)
    {
        sellerList = getSellerList();
    }
    if (curType == CONSUMERTYPE)
    {
        consumerList = getConsumerList();
    }
    bool flag = true;
    if (curType == SELLERTYPE)
    {
        for (int i = 0; i < (int)sellerList.size(); i++)
        {
            if (sellerList[i].name == loginName)
            {
                flag = false;
                if (sellerList[i].getPass() == loginPassword)
                {
                    loginStatus = 0;
                    curUser = new sellerClass(sellerList[i].getJson());
                }
                else
                {
                    loginStatus = PASSWORDWRONG;
                }
                break;
            }
        }
        if (flag)
        {
            loginStatus = USERNOTEXIST;
        }
    }
    else
    {
        for (int i = 0; i < (int)consumerList.size(); i++)
        {
            if (consumerList[i].name == loginName)
            {
                flag = false;
                if (consumerList[i].getPass() == loginPassword)
                {
                    curUser = new consumerClass(consumerList[i].getJson());
                    loginStatus = 0;
                }
                else
                {
                    loginStatus = PASSWORDWRONG;
                }
                break;
            }
        }
        if (flag)
        {
            loginStatus = USERNOTEXIST;
        }
    }
    return loginStatus;
}

int userManager::getMaxUid()
{
    ifstream infile;
    string uidMaxJson = "";
    int uidMax = 0;
    infile.open("uidMaxFile.json");
    infile >> uidMaxJson;
    infile.close();
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(uidMaxJson.c_str(),&jsonError);
    if(!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();
            if(object.contains("uid"))
            {
                QJsonValue value = object.value("uid");
                uidMax = value.toInt();
            }
        }
    }
    else
    {
        uidMaxJson = "";
        qDebug() << jsonError.error << endl;
        const int tmp = 0;
        QJsonObject object;
        object.insert("uid", tmp);
        QJsonDocument document;
        document.setObject(object);
        QByteArray array = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("uidMaxFile.json");
        outFile << array.toStdString();
        outFile.close();
    }
    return uidMax;
}

vector<sellerClass> userManager::getSellerList()
{
    vector<sellerClass> sellerList;
    ifstream infile;
    string sellerJson = "";
    infile.open("sellerFile.json");
    infile >> sellerJson;
    infile.close();
    QJsonParseError jsonError;
    QJsonArray userListJson;
    QJsonDocument document = QJsonDocument::fromJson(sellerJson.c_str(),&jsonError);
    if(!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();
            if(object.contains("data"))
            {
                QJsonValue value = object.value("data");
                userListJson = value.toArray();
            }
        }
        for (int i = 0; i < (int)userListJson.size(); i++)
        {
            sellerClass tmp(userListJson[i].toObject());
            sellerList.push_back(tmp);

        }
    }
    else
    {
        qDebug() << jsonError.error << endl;
        QString tmp = "";
        QJsonObject object;
        object.insert("data", tmp);
        QJsonDocument document;
        document.setObject(object);
        QByteArray array = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("sellerFile.json");
        outFile << array.toStdString();
        outFile.close();
    }
    return sellerList;
}

vector<consumerClass> userManager::getConsumerList()
{
    vector<consumerClass> consumerList;
    ifstream infile;
    string consumerJson = "";
    infile.open("consumerFile.json");
    infile >> consumerJson;
    infile.close();
    QJsonParseError jsonError;
    QJsonArray userListJson;
    QJsonDocument document = QJsonDocument::fromJson(consumerJson.c_str(),&jsonError);
    if(!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();
            if(object.contains("data"))
            {
                QJsonValue value = object.value("data");
                userListJson = value.toArray();
            }
        }
        for (int i = 0; i < (int)userListJson.size(); i++)
        {
            consumerClass tmp(userListJson[i].toObject());
            consumerList.push_back(tmp);
        }
    }
    else
    {
        qDebug() << jsonError.error << endl;
        QString tmp = "";
        QJsonObject object;
        object.insert("data", tmp);
        QJsonDocument document;
        document.setObject(object);
        QByteArray array = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("consumerFile.json");
        outFile << array.toStdString();
        outFile.close();
    }
    return consumerList;
}

void userManager::getUser(int userId, userClass &user)
{
    vector<sellerClass> sellerList = getSellerList();
    vector<consumerClass> consumerList = getConsumerList();
    for (int i = 0; i < (int) sellerList.size(); i++)
    {
        if (sellerList[i].uid == userId)
        {
            user.uid = sellerList[i].uid;
            user.name = sellerList[i].name;
            user.type = sellerList[i].type;
            user.balance = sellerList[i].balance;
            user.setPass(sellerList[i].getPass());
            return;
        }
    }
    for (int i = 0; i < (int) consumerList.size(); i++)
    {
        if (consumerList[i].uid == userId)
        {
            user.uid = consumerList[i].uid;
            user.name = consumerList[i].name;
            user.type = consumerList[i].type;
            user.balance = consumerList[i].balance;
            user.setPass(consumerList[i].getPass());
            return;
        }
    }
}

int userManager::changeUserName(int userId, string userName)
{
    vector<sellerClass> sellerList = getSellerList();
    vector<consumerClass> consumerList = getConsumerList();
    bool only = true;
    int numToChange = -1;
    for (int i = 0; i < (int)sellerList.size(); i++)
    {
        if (sellerList[i].name == userName && sellerList[i].uid != userId)
        {
            only = false;
            break;
        }
        if (sellerList[i].uid == userId)
        {
            numToChange = i;
        }
    }
    if (only && numToChange != -1)
    {
        sellerList[numToChange].name = userName;
        QJsonArray sellerJsonList;
        for (int i = 0; i < (int)sellerList.size(); i++)
        {
            sellerJsonList.push_back(sellerList[i].getJson());
        }
        QJsonObject object;
        object.insert("data", sellerJsonList);
        QJsonDocument document;
        document.setObject(object);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("sellerFile.json");
        outFile << byteArray.toStdString();
        outFile.close();
        return 0;
    }
    else if (numToChange == -1)
    {
        bool only2 = true;
        int numToChange = -1;
        for (int i = 0; i < (int)consumerList.size(); i++)
        {
            if (consumerList[i].name == userName && consumerList[i].uid != userId)
            {
                only2 = false;
                break;
            }
            if (consumerList[i].uid == userId)
            {
                numToChange = i;
            }
        }
        if (only2)
        {
            consumerList[numToChange].name = userName;
            QJsonArray consumerJsonList;
            for (int i = 0; i < (int)consumerList.size(); i++)
            {
                consumerJsonList.push_back(consumerList[i].getJson());
            }
            QJsonObject object;
            object.insert("data", consumerJsonList);
            QJsonDocument document;
            document.setObject(object);
            QByteArray byteArray = document.toJson(QJsonDocument::Compact);
            ofstream outFile;
            outFile.open("consumerFile.json");
            outFile << byteArray.toStdString();
            outFile.close();
            return 0;
        }
    }
    return 1;
}
