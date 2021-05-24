#ifndef PRODUCTITEM_H
#define PRODUCTITEM_H

#define FOODTYPE 1
#define CLOTHESTYPE 2
#define BOOKTYPE 3

#include <string>
#include <QImage>
#include <vector>
#include <QDebug>
#include <math.h>
#include <QJsonObject>
#include <QJsonArray>

using namespace std;

class productItem
{
public:
    string name;
    string sellerName;
    string description;
    double price;
    int remaining;
    vector<QByteArray> photo;
    int mainPhoto;
    int type;
    int id;
    int seller;
    double discount;
    productItem(double discount1, string name1, string description1, double price1, int remaining1, vector<QByteArray> photo1, int mainPhoto1, int type1, int id1, int seller1);
    productItem();
    productItem(QJsonObject data);
    virtual double getPrice(vector<vector<double>> discountList)
    {
        qDebug() << discountList;
        return price;
    };
    virtual ~productItem()
    {
    }
    QJsonObject getJson(vector<vector<double>> discountList);
};

class foodItem : public productItem
{
public:
    virtual double getPrice(vector<vector<double>> discountList) override
    {
        double discountThis = 1;
        for (int i = 0; i < (int)discountList.size(); i++)
        {
            if ((int)discountList[i][3] == seller)
            {
                discountThis = discountList[i][FOODTYPE - 1];
                break;
            }
        }
        if (discountThis == 1)
        {
            return price;
        }
        return (double)floor(price * discountThis * 100) / 100;
    }
    virtual ~foodItem()
    {
    }
};

class clothesItem : public productItem
{
public:
    virtual double getPrice(vector<vector<double>> discountList) override
    {
        double discountThis = 1;
        for (int i = 0; i < (int)discountList.size(); i++)
        {
            if ((int)discountList[i][3] == seller)
            {
                discountThis = discountList[i][CLOTHESTYPE - 1];
                break;
            }
        }
        if (discountThis == 1)
        {
            return price;
        }
        return (double)floor(price * discountThis * 100) / 100;
    }
    virtual ~clothesItem()
    {
    }
};

class bookItem : public productItem
{
public:
    virtual double getPrice(vector<vector<double>> discountList) override
    {
        double discountThis = 1;
        for (int i = 0; i < (int)discountList.size(); i++)
        {
            if ((int)discountList[i][3] == seller)
            {
                discountThis = discountList[i][BOOKTYPE - 1];
                break;
            }
        }
        if (discountThis == 1)
        {
            return price;
        }
        return (double)floor(price * discountThis * 100) / 100;
    }
    virtual ~bookItem()
    {
    }
};

#endif // PRODUCTITEM_H
