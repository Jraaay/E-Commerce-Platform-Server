#include "requestsprocess.h"
#include "tcpserver.h"
#include "ui_tcpserver.h"

RequestsProcess::RequestsProcess(void *father, QObject *parent) : QObject(parent)
{
    _father = father;
}

bool RequestsProcess::auth(string key, int userId, void *ui)
{
    QByteArray secretKey = "d=n+sia*&j#0^p@8!u20^f4g8r@p3(tgh=8uhx5_sxklwy_$$x";
    if ((int)key.find(".") == -1)
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("Auth failed, key is " + QString::fromStdString(key));
        qDebug() << "Auth failed";
        return false;
    }
    else
    {
        QByteArray jsonStr = key.substr(0, key.find(".")).c_str();
        QByteArray keyStr = key.substr(key.find(".") + 1, key.size()).c_str();
        if (QMessageAuthenticationCode::hash(jsonStr, secretKey, QCryptographicHash::Sha256).toBase64() != keyStr)
        {
            ((Ui::TcpServer *)ui)->textBrowser->append("Auth failed, key is " + QString::fromStdString(key));
            qDebug() << "Auth failed";
            return false;
        }
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(QByteArray::fromBase64(jsonStr),&jsonError);
        if(!document.isNull() && (jsonError.error == QJsonParseError::NoError))
        {
            if(document.isObject())
            {
                QJsonObject object = document.object();
                if (object.value("userId").toInt() != userId)
                {
                    ((Ui::TcpServer *)ui)->textBrowser->append("Auth failed, key is " + QString::fromStdString(key));
                    qDebug() << "Auth failed";
                    return false;
                }
            }
        }
    }
    ((Ui::TcpServer *)ui)->textBrowser->append("Auth successfully, user id is " + QString::fromStdString(to_string(userId)));
    qDebug() << "Auth successfully";
    return true;
}

void RequestsProcess::process(string jsonStr, void *father, void *ui)
{
    string key;
    int type = -1;
    QJsonObject data;
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(jsonStr.c_str(),&jsonError);
    if(!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();
            if(object.contains("type"))
            {
                type = object.value("type").toInt();
            }
            if (object.contains("data"))
            {
                data = object.value("data").toObject();
            }
            if (type != USER_loginCheck && type != USER_createUser)
            {
                if (object.contains("key"))
                {
                    key = object.value("key").toString().toStdString();
                }
            }
        }
    }
    Sqlite db;
    switch (type)
    {
    case SQLITE_singleInsertData:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_singleInsertData");
        db.openDb();
        productItem tmp = productItem(data);
        if (!auth(key, tmp.seller, ui))
        {
            break;
        }
        db.singleInsertData(tmp);
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_modifyItemInCart:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_modifyItemInCart");
        db.openDb();
        int number = -1;
        bool checked  = true;
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        if (data.contains("number"))
        {
            number = data.value("number").toInt();
        }
        if (data.contains("checked"))
        {
            checked = data.value("checked").toBool();
        }
        db.modifyItemInCart(data.value("productId").toInt(),
                            data.value("userId").toInt(),
                            number,
                            checked);
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_deleteItemFromCart:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_deleteItemFromCart");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        db.openDb();
        db.deleteItemFromCart(data.value("productId").toInt(),
                              data.value("userId").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_queryCart:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_queryCart");
        vector<productItem *> productList;
        vector<int> numberList;
        vector<bool> checkedList;
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        db.openDb();
        db.queryCart(data.value("userId").toInt(), productList, numberList, checkedList);
        QJsonArray productArray;
        for (int i = 0; i < (int)productList.size(); i++)
        {
            productArray.append(productList[i]->getJson(db.getDiscount()));
        }
        db.closeDb();
        QJsonObject object;
        object.insert("productList", productArray);
        QJsonArray result;
        std::copy (numberList.begin(), numberList.end(), std::back_inserter(result));
        object.insert("numberList", result);
        QJsonArray array;
        for (int i = 0; i < (int)checkedList.size(); i++)
        {
            array.append((bool)checkedList[i]);
        }
        object.insert("checkedList", array);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_queryTable:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_queryTable");
        db.openDb();
        vector<productItem *> productList = db.queryTable(data.value("LIKE").toString().toStdString(),
                                                          data.value("SORT").toString().toStdString());
        QJsonArray productArray;
        for (int i = 0; i < (int)productList.size(); i++)
        {
            productArray.append(productList[i]->getJson(db.getDiscount()));
        }
        db.closeDb();
        QJsonObject object;
        object.insert("productList", productArray);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_modifyData:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_modifyData");
        if (!auth(key, productItem(data.value("item").toObject()).seller, ui))
        {
            break;
        }
        db.openDb();
        db.modifyData(productItem(data.value("item").toObject()), data.value("updateImage").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_deleteData:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_deleteData");
        db.openDb();
        if (!auth(key, db.queryTable("", "", data.value("id").toInt())[0]->seller, ui))
        {
            break;
        }
        db.deleteData(data.value("id").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_newDiscount:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_newDiscount");
        db.openDb();
        if (!auth(key, data.value("id").toInt(), ui))
        {
            break;
        }
        db.newDiscount(data.value("id").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_setDiscount:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_setDiscount");
        if (!auth(key, data.value("userId").toDouble(), ui))
        {
            break;
        }
        db.openDb();
        vector<vector<double>> discount = db.getDiscount();
        for (int i = 0; i < (int)discount.size(); i++)
        {
            if (discount[i][3] == data.value("userId").toDouble())
            {
                discount[i][0] = data.value("fooddiscount").toDouble();
                discount[i][1] = data.value("clothesdiscount").toDouble();
                discount[i][2] = data.value("bookdiscount").toDouble();
                break;
            }
        }
        db.setDiscount(discount);
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_generateOrder:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_generateOrder");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        db.openDb();
        int orderId = db.generateOrder(data.value("userId").toInt());
        db.closeDb();
        QJsonObject object;
        object.insert("orderId", orderId);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_getOrder:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_getOrder");
        bool paied;
        long long time;
        int userId;
        vector<productItem *> orderList;
        vector<int> count;
        vector<double> price;
        double priceSum;
        db.openDb();
        db.getOrder(data.value("orderId").toInt(), paied, time, userId, orderList, count, price, priceSum);
        QJsonArray orderJsonList;
        for (int i = 0; i < (int)orderList.size(); i++)
        {
            orderJsonList.push_back(orderList[i]->getJson(db.getDiscount()));
        }
        db.closeDb();
        if (!auth(key, userId, ui))
        {
            break;
        }
        QJsonObject object;
        object.insert("paied", paied);
        object.insert("time", time);
        object.insert("userId", userId);
        QJsonArray result;
        std::copy (count.begin(), count.end(), std::back_inserter(result));
        object.insert("count", result);
        QJsonArray result2;
        std::copy (price.begin(), price.end(), std::back_inserter(result2));
        object.insert("price", result2);
        object.insert("priceSum", priceSum);
        object.insert("orderList", orderJsonList);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_getOrderList:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_getOrderList");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        vector<int> orderId;
        vector<double> priceSum;
        vector<long long> time;
        vector<bool> paid;
        db.openDb();
        db.getOrderList(data.value("userId").toInt(), orderId, priceSum, time, paid);
        db.closeDb();
        QJsonObject object;
        QJsonArray result;
        std::copy (orderId.begin(), orderId.end(), std::back_inserter(result));
        object.insert("orderId", result);
        QJsonArray result1;
        std::copy (priceSum.begin(), priceSum.end(), std::back_inserter(result1));
        object.insert("priceSum", result1);
        QJsonArray result2;
        std::copy (time.begin(), time.end(), std::back_inserter(result2));
        object.insert("time", result2);
        QJsonArray array;
        for (int i = 0; i < (int)paid.size(); i++)
        {
            array.append((bool)paid[i]);
        }
        object.insert("paid", array);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case pay:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("pay");
        bool paied;
        long long time;
        int userId;
        vector<productItem *> orderList;
        vector<int> count;
        vector<double> price;
        double priceSum;
        db.openDb();
        db.getOrder(data.value("orderId").toInt(), paied, time, userId, orderList, count, price, priceSum);
        if (!auth(key, userId, ui))
        {
            break;
        }
        int payStatus = db.payOrder(data.value("orderId").toInt());
        db.closeDb();
        QJsonObject object;
        object.insert("payStatus", payStatus);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_getDiscount:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_getDiscount");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        db.openDb();
        vector<vector<double>> discount = db.getDiscount();
        db.closeDb();
        QJsonObject object;
        for (int i = 0;i < (int)discount.size(); i++)
        {
            if (discount[i][3] == data.value("userId").toInt())
            {
                object.insert("fooddiscount", discount[i][0]);
                object.insert("clothesdiscount", discount[i][1]);
                object.insert("bookdiscount", discount[i][2]);
            }
        }
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_buyOneThing:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_buyOneThing");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        db.openDb();
        int payStatus = db.buyOne(data.value("userId").toInt(), data.value("productId").toInt());
        db.closeDb();
        QJsonObject object;
        object.insert("payStatus", payStatus);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case USER_createUser:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("USER_createUser");
        int regStatus = userManager::createUser(data.value("curType").toInt(),
                                data.value("loginName").toString().toStdString(),
                                data.value("loginPassword").toString().toStdString());
        QJsonObject object;
        object.insert("regStatus", regStatus);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case USER_changeUserName:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("USER_changeUserName");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        int status = userManager::changeUserName(data.value("userId").toInt(),
                                data.value("userName").toString().toStdString());
        QJsonObject object;
        object.insert("status", status);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case USER_loginCheck:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        QByteArray secretKey = "d=n+sia*&j#0^p@8!u20^f4g8r@p3(tgh=8uhx5_sxklwy_$$x";
        ((Ui::TcpServer *)ui)->textBrowser->append("USER_loginCheck");
        userClass *curUser;
        int loginStatus = userManager::loginCheck(data.value("curType").toInt(),
                                data.value("loginName").toString().toStdString(),
                                data.value("loginPassword").toString().toStdString(),
                                curUser);
        qDebug() << loginStatus;
        QJsonObject object;
        object.insert("loginStatus", loginStatus);
        if (loginStatus == 0)
        {
            object.insert("curUser", curUser->getJson());
            ((Ui::TcpServer *)ui)->textBrowser->append("Login successfully, user id is " + QString::fromStdString(to_string(curUser->uid)));
            QJsonObject keyGen;
            keyGen.insert("userId", curUser->uid);
            time_t t;
            time(&t);
            keyGen.insert("time", QVariant::fromValue(t).toLongLong());
            QJsonDocument documentKeyGen;
            documentKeyGen.setObject(keyGen);
            QByteArray keyJson = documentKeyGen.toJson(QJsonDocument::Compact).toBase64();
            QByteArray key = keyJson + "." + QMessageAuthenticationCode::hash(keyJson, secretKey, QCryptographicHash::Sha256).toBase64();
            object.insert("key", key.toStdString().c_str());
            ((Ui::TcpServer *)ui)->textBrowser->append("The key has been sent successfully, key is " + key);
        }
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case USER_getUser:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("USER_getUser");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        sellerClass curUser;
        userManager::getUser(data.value("userId").toInt(), curUser);
        QJsonObject object;
        object.insert("user", curUser.getJson());
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case USER_recharge:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("USER_recharge");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        userManager::recharge(data.value("userId").toInt(), data.value("moneyToCharge").toDouble());
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case USER_changePassword:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append(jsonStr.c_str());
        ((Ui::TcpServer *)ui)->textBrowser->append("USER_changePassword");
        if (!auth(key, data.value("userId").toInt(), ui))
        {
            break;
        }
        userManager::changePassword(data.value("userId").toInt(), data.value("password").toString().toStdString());
        ((TcpServer *)father)->sendData("0");
        break;
    }
    default:
        return;
    }
}
