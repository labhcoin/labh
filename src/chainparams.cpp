// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

//
// Main network
//

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x3f;
        pchMessageStart[1] = 0x4c;
        pchMessageStart[2] = 0xbe;
        pchMessageStart[3] = 0xd4;
        vAlertPubKey = ParseHex("04570b1f958d27475d9f8d3a01dcb515da31deffe9fe83e20dfff9d3014ff195ac6d6bdea089c31b1477092879257d682fb2394f2536bf2dd92c09cd6e7a13d336");
        nDefaultPort = 26667;
        nRPCPort = 26668;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);

        const char* pszTimestamp = "18 April 2018 - Philippines Senator Wants Harsher Penalties for Cryptocurrency Crimes";
        std::vector<CTxIn> vin;
        std::vector<CTxOut> vout;
        vin.resize(1);
        vout.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        vout[0].nValue = 0;
        vout[0].scriptPubKey = CScript() << ParseHex("0449f970bf624677ddb2d6ec0d95409bb540a8cbb70de0e1f99ab10c38b1555e5ea7f1688c5b38c47bca9e4f34235a73062b9585173531203e86878eb3503d5ef6") << OP_CHECKSIG;
        CTransaction txNew(1, 1524046794, vin, vout, 0);
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1524046794;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 3386701;

        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0x00000172a7e27a0c53a9c6fd4d322aa3150bb74dc2669b8a41e76b09eb8befe6"));
        assert(genesis.hashMerkleRoot == uint256("0x03a08719de74ac48559e57007342c2c71beda542df15c687dd4c78e6ff322ff1"));

        vSeeds.push_back(CDNSSeedData("seed1", "seed1.labhcoin.com"));
        vSeeds.push_back(CDNSSeedData("seed2", "seed2.labhcoin.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 48); // L
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 125); // s
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1, 45); // K
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        nLastPOWBlock = 5000;
        nMinStakingBeforeSuperblock = 10;
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet
//

class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x76;
        pchMessageStart[1] = 0x3d;
        pchMessageStart[2] = 0xa9;
        pchMessageStart[3] = 0x74;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        vAlertPubKey = ParseHex("04554f17a7d120239ce82c375efe544dc3d4a0f1e28337841732e052d9a5dca0ce276496e1f468eb9e866fe014d220e6ecd69e50c09fa705cf1ad0b3c148ff18c4");
        nDefaultPort = 16667;
        nRPCPort = 16668;
        strDataDir = "testnet";

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 452629;
        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0x0000bde475eff44f3279af2d6462b692619688f6ab5719574139070b828ba088"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 127); // t
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 125); // s
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1, 107); // k
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        nLastPOWBlock = 0x7fffffff;
        nMinStakingBeforeSuperblock = 2;
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        pchMessageStart[0] = 0x9c;
        pchMessageStart[1] = 0x4b;
        pchMessageStart[2] = 0x5f;
        pchMessageStart[3] = 0xa9;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1411111111;
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 196647;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        strDataDir = "regtest";

        assert(hashGenesisBlock == uint256("0x6c80deb3a4d673f8e8cb5768ae60195bf107b451ea4608e96db72e0174eb0253"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest) {
        return false;
    }

    if (fRegTest) {
        SelectParams(CChainParams::REGTEST);
    } else if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
