#ifndef OPTIONSMODEL_H
#define OPTIONSMODEL_H

#include <QAbstractListModel>

#include "util.h"

/** Interface from Qt to configuration data structure for Bitcoin client.
   To Qt, the options are presented as a list with the different options
   laid out vertically.
   This can be changed to a tree once the settings become sufficiently
   complex.
 */
class OptionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit OptionsModel(QObject *parent = 0);

    enum OptionID {
        StartAtStartup,    // bool
        MinimizeToTray,    // bool
        MapPortUPnP,       // bool
        MinimizeOnClose,   // bool
        ProxyUse,          // bool
        ProxyIP,           // QString
        ProxyPort,         // int
        Fee,               // CAmount
        ReserveBalance,    // CAmount
        DisplayUnit,       // BitcoinUnits::Unit
        Language,          // QString
        CoinControlFeatures, // bool
        OptionIDRowCount,
    };

    void Init();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    /* Explicit getters */
    CAmount getTransactionFee();
    CAmount getReserveBalance();
    bool getMinimizeToTray();
    bool getMinimizeOnClose();
    int getDisplayUnit();
    bool getCoinControlFeatures();
    QString getLanguage() { return language; }

private:
    int nDisplayUnit;
    bool fMinimizeToTray;
    bool fMinimizeOnClose;
    bool fCoinControlFeatures;
    QString language;

signals:
    void displayUnitChanged(int unit);
    void transactionFeeChanged(CAmount);
    void reserveBalanceChanged(CAmount);
    void coinControlFeaturesChanged(bool);
};

#endif // OPTIONSMODEL_H
