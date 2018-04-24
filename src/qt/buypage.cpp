#include "buypage.h"
#include "ui_buypage.h"

#include "init.h"
#include "base58.h"
#include "addressbookpage.h"

#include <iostream>

BuyPage::BuyPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuyPage)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateOrderStatus()));
}

BuyPage::~BuyPage()
{
    if (timer->isActive())
        timer->stop();
    delete timer;
    delete ui;
}

void BuyPage::setModel(WalletModel *model)
{
    if(!model)
        return;
    this->model = model;
}

void BuyPage::show()
{
    if (pwalletMain->lastOrderId != "") {
        ui->stackedWidget->setCurrentIndex(2);
        if (!timer->isActive())
            timer->start(1000);
        updateOrderStatus(true);
    } else {
        ui->stackedWidget->setCurrentIndex(0);
        getPackages();
    }
}

// Get available packages and fill the array.
void BuyPage::getPackages()
{
    QJsonDocument jsonDocument(QJsonDocument::fromJson(apiGet("packages").toUtf8()));
    QJsonObject json = jsonDocument.object();

    if (json.contains("packages")) {
        ui->comboAvailablePackages->clear();
        vAvailablePackages.clear();

        QJsonArray packagesArray = json["packages"].toArray();
        for (int i = 0; i < packagesArray.size(); i++) {
            QJsonObject packageObject = packagesArray[i].toObject();
            Package package;
            package.read(packageObject);
            vAvailablePackages.append(package);

            // Update combo
            ui->comboAvailablePackages->addItem(package.name());
        }
    }
}

void BuyPage::updateOrderStatus(bool fForce)
{
    static int64_t nOrderStatusUpdated = 0;

    // automatically update order status only once in ORDER_UPDATE_SECONDS seconds
    int64_t nSecondsTillUpdate = nOrderStatusUpdated + ORDER_UPDATE_SECONDS - GetTime();

    if (nOrderStatusUpdated > 0)
        ui->labelUpdateOrderStatus->setText("Order status will automatically update in (sec): " + QString::number(nSecondsTillUpdate));

    if (nSecondsTillUpdate > 0 && !fForce) return;
    nOrderStatusUpdated = GetTime();

    QJsonDocument jsonDocument(QJsonDocument::fromJson(apiGet("orders/" + QString::fromStdString(pwalletMain->lastOrderId)).toUtf8()));
    QJsonObject json = jsonDocument.object();

    if (json.contains("order")) {
        QJsonObject orderObject = json["order"].toObject();
        Order order;
        order.read(orderObject);

        // Update labels
        ui->labelStatusSelectedPackageValue->setText(order.package()->name());
        ui->labelStatusPaymentMethodValue->setText(order.currency());
        ui->labelStatusTotalOrderValue->setText(QString("%1 %2").arg(order.price(), 8, 'f').arg(order.currency()));
        ui->labelDepositAddressValue->setText(order.depositAddress());

        switch (order.status()) {
            case 0:
                ui->labelOrderStatusValue->setText("Created");
                break;
            case 1:
                ui->labelOrderStatusValue->setText("Paid Partially");
                break;
            case 2:
                ui->labelOrderStatusValue->setText("Order Paid");
                break;
            case 3:
                ui->labelOrderStatusValue->setText("Funds send to user");

                updateOrderId("");
                ui->cancelOrderButton->setText("New order");
                break;
        }
    }
}

void BuyPage::updateOrderId(std::string orderId)
{
    CWalletDB walletdb(pwalletMain->strWalletFile);
    LOCK(pwalletMain->cs_wallet);

    pwalletMain->lastOrderId = orderId;
    if (pwalletMain->fFileBacked)
        walletdb.WriteLastOrderId(orderId);
}

void BuyPage::on_choosePackageButton_clicked()
{
    if (!model)
        return;

    if (ui->comboAvailablePackages->currentIndex() == -1)
        return;

    Package selectedPackage = vAvailablePackages[ui->comboAvailablePackages->currentIndex()];
    double packagePrice;

    for (const PackagePrice& price : selectedPackage.prices()) {
        if (price.currency() == ui->comboPaymentMethod->currentText()) {
            packagePrice = price.price();
        }
    }

    ui->labelSelectedPackageValue->setText(selectedPackage.name());
    ui->labelPaymentMethodValue->setText(ui->comboPaymentMethod->currentText());
    ui->labelTotalOrderValue->setText(QString("%1 %2").arg(packagePrice, 8, 'f').arg(ui->comboPaymentMethod->currentText()));

    ui->stackedWidget->setCurrentIndex(1);
}

void BuyPage::on_startOverButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void BuyPage::on_cancelOrderButton_clicked()
{
    updateOrderId("");
    getPackages();
    ui->stackedWidget->setCurrentIndex(0);
}

void BuyPage::on_comboAvailablePackages_currentIndexChanged(int index)
{
    // Remove previous pricing options.
    ui->packagePrices->setText("");

    if (index < 0 || vAvailablePackages.isEmpty() || index > vAvailablePackages.size()) {
        ui->packagePrices->setText("");
        return;
    }

    // Add pricing options per selected package.
    QVector<PackagePrice> prices = vAvailablePackages[index].prices();
    QString strPrices;
    for (int i = 0; i < prices.size(); i++) {
        strPrices += prices[i].currency() + " <span style='font-weight:bold;'>" + QString::number(prices[i].price()) + "</span><br>";
    }
    ui->packagePrices->setText(strPrices);

    // Update available currencies buy option.
    ui->comboPaymentMethod->clear();
    for (const QString& str : vAvailablePackages[index].buyWithCurrencies()) {
        ui->comboPaymentMethod->addItem(str);
    }
}

void BuyPage::on_addressBookButton_clicked()
{
    if(!model)
        return;
    AddressBookPage dlg(AddressBookPage::ForSending, AddressBookPage::ReceivingTab, this);
    dlg.setModel(model->getAddressTableModel());
    if(dlg.exec())
    {
        ui->payTo->setText(dlg.getReturnValue());
    }
}

void BuyPage::on_getDepositAddressButton_clicked()
{
    if (!model)
        return;

    ui->getDepositAddressButton->setEnabled(false);

    // Validate input
    bool valid = true;

    if(!ui->payTo->hasAcceptableInput() ||
        (model && !model->validateAddress(ui->payTo->text())))
    {
        ui->payTo->setValid(false);
        valid = false;
    }

    if (!valid) {
        ui->getDepositAddressButton->setEnabled(true);
        return;
    }

    Package package = vAvailablePackages[ui->comboAvailablePackages->currentIndex()];

    QJsonObject jsonData;
    jsonData.insert("package_id", package.id());
    jsonData.insert("currency", ui->comboPaymentMethod->currentText());
    jsonData.insert("address", ui->payTo->text());

    QJsonDocument jsonDoc(jsonData);

    QJsonDocument jsonResponse(QJsonDocument::fromJson(apiPost("orders", jsonDoc).toUtf8()));
    QJsonObject json = jsonResponse.object();

    if (!json.contains("order")) {
        // ERROR
        QMessageBox::warning(this, tr("BUY LABH!"),
            tr("Error: Unable to process your request, please try again later."),
            QMessageBox::Ok, QMessageBox::Ok);
        ui->getDepositAddressButton->setEnabled(true);
        return;
    }

    QJsonObject orderObject = json["order"].toObject();
    Order order;
    order.read(orderObject);

    updateOrderId(order.id().toStdString());

    timer->start(1000);
    updateOrderStatus(true);

    ui->stackedWidget->setCurrentIndex(2);
    ui->getDepositAddressButton->setEnabled(true);
}

void BuyPage::on_pasteButton_clicked()
{
    // Paste text from clipboard into recipient field
    ui->payTo->setText(QApplication::clipboard()->text());
}

QString BuyPage::apiGet(QString url)
{
    url = baseURL + url;

    QString Response = "";

    QEventLoop loop; // create custom temporary event loop on stack
    QNetworkAccessManager *manager = new QNetworkAccessManager( this );
    QNetworkReply *resp;

    // the HTTP request
    QNetworkRequest req = QNetworkRequest(QUrl(url));
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    resp = manager->get(req);
    loop.exec(); // blocks until "finished()" has been called

    resp->deleteLater();
    manager->deleteLater();

    // Error. we probably timed out i.e SIGNAL(finished()) did not happen
    if (resp == NULL) {
        resp->abort();
        return "{}";
    }

    if (resp->error() == QNetworkReply::NoError) {
        Response = resp->readAll();
    } else {
        Response = "{}";
    }

    LogPrint("buy", "%s(): %s", __func__, Response.toStdString());

    return Response;
}

QString BuyPage::apiPost(QString url, QJsonDocument json)
{
    url = baseURL + url;

    QString Response = "";

    QEventLoop loop; // create custom temporary event loop on stack
    QNetworkAccessManager *manager = new QNetworkAccessManager( this );
    QNetworkReply *resp;

    QByteArray jsonData = json.toJson();

    // the HTTP request
    QNetworkRequest req = QNetworkRequest(QUrl(url));

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonData.size()));

    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    resp = manager->post(req, jsonData);
    loop.exec(); // blocks until "finished()" has been called

    resp->deleteLater();
    manager->deleteLater();

    // Error. we probably timed out i.e SIGNAL(finished()) did not happen
    if (resp == NULL) {
        resp->abort();
        return "{}";
    }

    if (resp->error() == QNetworkReply::NoError) {
        Response = resp->readAll();
    } else {
        Response = "{}";
    }

    return Response;
}

void Package::read(const QJsonObject &json)
{
    if (json.contains("id") && json["id"].isString()) {
        strId = json["id"].toString();
    }

    if (json.contains("name") && json["name"].isString()) {
        strName = json["name"].toString();
    }

    if (json.contains("buy_with_currencies") && json["buy_with_currencies"].isArray()) {
        vBuyWithCurrencies.clear();
        QJsonArray buyWithCurrenciesArray = json["buy_with_currencies"].toArray();
        for (int i = 0; i < buyWithCurrenciesArray.size(); i++) {
            vBuyWithCurrencies.append(buyWithCurrenciesArray[i].toString());
        }
    }

    if (json.contains("prices") && json["prices"].isArray()) {
        vPrices.clear();
        QJsonArray pricesArray = json["prices"].toArray();
        for (int i = 0; i < pricesArray.size(); i++) {
            QJsonObject priceObject = pricesArray[i].toObject();
            PackagePrice price;
            price.read(priceObject);
            vPrices.append(price);
        }
    }
}

void PackagePrice::read(const QJsonObject &json)
{
    if (json.contains("currency") && json["currency"].isString()) {
        strCurrency = json["currency"].toString();
    }

    if (json.contains("price") && json["price"].isDouble()) {
        nPrice = json["price"].toDouble();
    }
}

void Order::read(const QJsonObject &json)
{
    if (json.contains("id") && json["id"].isString()) {
        strId = json["id"].toString();
    }

    if (json.contains("timestamp") && json["timestamp"].isString()) {
        dateTimestamp = QDateTime::fromString(json["timestamp"].toString());
    }

    if (json.contains("package") && json["package"].isObject()) {
        pPackage = new OrderPackage();
        pPackage->read(json["package"].toObject());
    }

    if (json.contains("currency") && json["currency"].isString()) {
        strCurrency = json["currency"].toString();
    }

    if (json.contains("price") && json["price"].isDouble()) {
        nPrice = json["price"].toDouble();
    }

    if (json.contains("status") && json["status"].isDouble()) {
        nStatus = (int)json["status"].toDouble();
    }

    if (json.contains("deposit_address") && json["deposit_address"].isString()) {
        strDepositAddress = json["deposit_address"].toString();
    }

    if (json.contains("customer_address") && json["customer_address"].isString()) {
        strCustomerAddress = json["customer_address"].toString();
    }

    if (json.contains("txids") && json["txids"].isArray()) {
        vTxIds.clear();
        QJsonArray txidsArray = json["txids"].toArray();
        for (int i = 0; i < txidsArray.size(); i++) {
            vTxIds.append(txidsArray[i].toString());
        }
    }
}

void OrderPackage::read(const QJsonObject &json)
{
    if (json.contains("id") && json["id"].isString()) {
        strId = json["id"].toString();
    }

    if (json.contains("name") && json["name"].isString()) {
        strName = json["name"].toString();
    }

    if (json.contains("currency") && json["currency"].isString()) {
        strCurrency = json["currency"].toString();
    }

    if (json.contains("value") && json["value"].isDouble()) {
        nValue = json["value"].toDouble();
    }

    if (json.contains("price") && json["price"].isDouble()) {
        nPrice = json["price"].toDouble();
    }
}