#ifndef BUYPAGE_H
#define BUYPAGE_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDialog>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QClipboard>
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>

#include "walletmodel.h"

#define ORDER_UPDATE_SECONDS 60

namespace Ui {
    class BuyPage;
}

class ClientModel;
class Package;
class PackagePrice;
class Order;
class OrderPackage;

const QString baseURL = "https://buy.labhcoin.com/v1.0/";

/** Buy page widget */
class BuyPage : public QDialog
{
    Q_OBJECT

public:
    explicit BuyPage(QWidget *parent = 0);
    ~BuyPage();
    void setModel(WalletModel *model);
    void show();
    void getPackages();
    void updateOrderId(std::string orderId);
    QString currentOrder() const {
        return strCurrentOrder;
    }

private:
    Ui::BuyPage *ui;
    WalletModel *model;
    QTimer *timer;

    int nStatus;
    QVector<Package> vAvailablePackages;
    QString strCurrentOrder;

private slots:
    /** User selected package and go to wallet address selection **/
    void on_choosePackageButton_clicked();
    /** User decided to start over **/
    void on_startOverButton_clicked();
    /** Confirm order and retreive deposit address from server **/
    void on_getDepositAddressButton_clicked();
    /** User changed available package **/
    void on_comboAvailablePackages_currentIndexChanged(int index);
    /** Select address from address book **/
    void on_addressBookButton_clicked();
    /** Paste into deposit address **/
    void on_pasteButton_clicked();
    /** User cancelled order **/
    void on_cancelOrderButton_clicked();
    /** Manage API requests **/
    QString apiGet(QString url);
    QString apiPost(QString url, QJsonDocument json);

public Q_SLOTS:
    /** Update order status and timer label **/
    void updateOrderStatus(bool fForce = false);
};

/** Package **/
class Package
{
    Q_GADGET;

public:
    Package() = default;
    QString name() const {
        return strName;
    }
    QString id() const {
        return strId;
    }
    QVector<PackagePrice> prices()  {
        return vPrices;
    }
    QStringList buyWithCurrencies() const {
        return vBuyWithCurrencies;
    }

    void read(const QJsonObject &json);

private:
    QString strId;
    QString strName;
    QStringList vBuyWithCurrencies;
    QVector<PackagePrice> vPrices;
};

/** PackagePrice **/
class PackagePrice
{
    Q_GADGET;

public:
    PackagePrice() = default;
    QString currency() const {
        return strCurrency;
    }
    double price() const {
        return nPrice;
    }

    void read(const QJsonObject &json);

private:
    QString strCurrency;
    double nPrice;
};

/** Order **/
class Order
{
    Q_GADGET;

public:
    Order() = default;
    QString id() const {
        return strId;
    }
    QDateTime timestamp() const {
        return dateTimestamp;
    }
    OrderPackage *package() const {
        return pPackage;
    }
    QString currency() const {
        return strCurrency;
    }
    double price() const {
        return nPrice;
    }
    int status() const {
        return nStatus;
    }
    QString depositAddress() const {
        return strDepositAddress;
    }
    QString customerAddress() const {
        return strCustomerAddress;
    }
    QStringList txIds() const {
        return vTxIds;
    }

    void read(const QJsonObject &json);

private:
    QString strId;
    QDateTime dateTimestamp;
    OrderPackage *pPackage;
    QString strCurrency;
    double nPrice;
    int nStatus;
    QString strDepositAddress;
    QString strCustomerAddress;
    QStringList vTxIds;
};

class OrderPackage
{
    Q_GADGET;

public:
    OrderPackage() = default;
    QString id() const {
        return strId;
    }
    QString name() const {
        return strName;
    }
    QString currency() const {
        return strCurrency;
    }
    double value() const {
        return nValue;
    }
    double price() const {
        return nPrice;
    }

    void read(const QJsonObject &json);

private:
    QString strId;
    QString strName;
    QString strCurrency;
    double nValue;
    double nPrice;
};

#endif // BUYPAGE_H
