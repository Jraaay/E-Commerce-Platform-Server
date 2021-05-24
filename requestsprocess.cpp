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
        db.openDb();
        db.deleteItemFromCart(data.value("productId").toInt(),
                              data.value("userId").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_queryCart:
    {
        vector<productItem *> productList;
        vector<int> numberList;
        vector<bool> checkedList;
        db.openDb();
        db.queryCart(data.value("userId").toInt(), productList, numberList, checkedList);
        db.closeDb();
        QJsonArray productArray;
        for (int i = 0; i < (int)productList.size(); i++)
        {
            productArray.append(productList[i]->getJson(db.getDiscount()));
        }
        QJsonObject object;
        object.insert("productList", productArray);
        QJsonArray result;
        std::copy (numberList.begin(), numberList.end(), std::back_inserter(result));
        object.insert("numberList", result);
        QJsonArray array;
        for (int i = 0; i < (int)checkedList.size(); i++)
        {
            array.append((int)checkedList[i]);
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
        db.openDb();
        db.modifyData(productItem(data.value("item").toObject()), data.value("item").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_deleteData:
    {
        db.openDb();
        db.deleteData(data.value("id").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_newDiscount:
    {
        db.openDb();
        db.newDiscount(data.value("id").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_setDiscount:
    {
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
        db.openDb();
        vector<productItem> orderList;
        for (int i = 0; i < data.value("orderList").toArray().size(); i++)
        {
            orderList.push_back(productItem(data.value("orderList").toArray()[i].toObject()));
        }
        vector<int> count;
        vector<double> price;
        for (int i = 0; i < data.value("count").toArray().size(); i++)
        {
            count.push_back(data.value("count").toArray()[i].toInt());
            price.push_back(data.value("price").toArray()[i].toDouble());
        }
        int orderId = db.generateOrder(data.value("userId").toInt(),
                         orderList,
                         count,
                         price,
                         data.value("priceSum").toDouble());
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
        bool paied;
        long long time;
        int userId;
        vector<productItem *> orderList;
        vector<int> count;
        vector<double> price;
        double priceSum;
        db.openDb();
        db.getOrder(data.value("orderId").toInt(), paied, time, userId, orderList, count, price, priceSum);
        db.closeDb();
        QJsonArray orderJsonList;
        for (int i = 0; i < (int)orderList.size(); i++)
        {
            orderJsonList.push_back(orderList[i]->getJson(db.getDiscount()));
        }
        QJsonObject object;
        object.insert("paied", paied);
        object.insert("time", time);
        object.insert("userId", userId);
        QJsonArray result;
        std::copy (count.begin(), count.end(), std::back_inserter(result));
        object.insert("count", result);
        std::copy (price.begin(), price.end(), std::back_inserter(result));
        object.insert("price", result);
        object.insert("priceSum", paied);
        object.insert("orderList", orderJsonList);
        QJsonDocument document;
        document.setObject(object);
        string jsonStr = document.toJson(QJsonDocument::Compact).toStdString();
        ((TcpServer *)father)->sendData(jsonStr.c_str());
        break;
    }
    case SQLITE_getOrderList:
    {
        vector<int> orderId;
        vector<double> priceSum;
        vector<long long> time;
        vector<bool> paid;
        db.openDb();
        db.getOrderList(data.value("orderId").toInt(), orderId, priceSum, time, paid);
        db.closeDb();
        QJsonObject object;
        QJsonArray result;
        std::copy (orderId.begin(), orderId.end(), std::back_inserter(result));
        object.insert("orderId", result);
        std::copy (priceSum.begin(), priceSum.end(), std::back_inserter(result));
        object.insert("priceSum", result);
        std::copy (time.begin(), time.end(), std::back_inserter(result));
        object.insert("time", result);
        QJsonArray array;
        for (int i = 0; i < (int)paid.size(); i++)
        {
            array.append((qlonglong)paid[i]);
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
        db.openDb();
        db.payOrder(data.value("orderId").toInt());
        db.closeDb();
        ((TcpServer *)father)->sendData("0");
        break;
    }
    case SQLITE_getDiscount:
    {
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
    }
    case USER_createUser:
    case USER_modifyUserInfo:
    case USER_loginCheck:
    case USER_getUser:
    default:
        return;
    }
}
