#include "requestsprocess.h"
#include "tcpserver.h"
#include "ui_tcpserver.h"

RequestsProcess::RequestsProcess(void *father, QObject *parent) : QObject(parent)
{
    _father = father;
}

void RequestsProcess::process(string jsonStr, void *father, void *ui)
{
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
        QImage img;
        img.loadFromData(tmp.photo[0]);
        ((Ui::TcpServer *)ui)->label->setPixmap(QPixmap::fromImage(img));
        db.singleInsertData(tmp);
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_modifyItemInCart:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_modifyItemInCart");
        db.openDb();
        int number = -1;
        bool checked  = true;
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
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_deleteItemFromCart");
        db.openDb();
        db.deleteItemFromCart(data.value("productId").toInt(),
                              data.value("userId").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_queryCart:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_queryCart");
        vector<productItem *> productList;
        vector<int> numberList;
        vector<bool> checkedList;
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
        db.openDb();
        db.modifyData(productItem(data.value("item").toObject()), data.value("item").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_deleteData:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_deleteData");
        db.openDb();
        db.deleteData(data.value("id").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_newDiscount:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_newDiscount");
        db.openDb();
        db.newDiscount(data.value("id").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_setDiscount:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_setDiscount");
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
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_generateOrder");
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
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_getOrderList");
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
        ((Ui::TcpServer *)ui)->textBrowser->append("pay");
        db.openDb();
        db.payOrder(data.value("orderId").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_getDiscount:
    {
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_getDiscount");
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
        ((Ui::TcpServer *)ui)->textBrowser->append("SQLITE_buyOneThing");
        db.openDb();
        db.buyOne(data.value("userId").toInt(), data.value("productId").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case USER_createUser:
    case USER_modifyUserInfo:
    case USER_loginCheck:
    case USER_getUser:
    default:
        return;
    }
}
