// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,      uint256("0x00000172a7e27a0c53a9c6fd4d322aa3150bb74dc2669b8a41e76b09eb8befe6") )
        ( 10000,  uint256("0x9a2dd91c7f73ca2542e22b3ee9f63ad9e7fb8679f5021744688e7d92bd8267ba") )
        ( 15000,  uint256("0xcaca63ae9f8a774f0bc4c5391d08c536c38465f42b865dd6b6c8186ce88a88d3") )
        ( 20000,  uint256("0x2de82840a6e8478966bb196fec2a53f436a9042d8f69baf2f7fff6113dd51859") )
        ( 25000,  uint256("0x2c54e5949176f3f6a9026b40b8aea1ea562fa8b168f412e02c43324da99bcd42") )
        ( 30000,  uint256("0x5c0d7fe76c43986c2cadf6890db68de6eb9c3619d8f081934cbad5ada8f6cb2c") )
        ( 35000,  uint256("0xf0620aced91009b07c55596bcbb1b607c2b5b03129dae19f98f0caf8ef96afe7") )
        ( 40000,  uint256("0xa1b2d1e9a87ae243249d7b2b3fdc73b2bb725e954296f77e46dc5403b247e5d0") )
        ( 45000,  uint256("0x16dfd446006f39549fba599c614bfc0106b03fb5cc67757cc4d6b52bb43d89c8") )
        ( 50000,  uint256("0xb8e17fed3e668304bcc9b692fcf2d736dc8f668492abcbc23a70c4905f99dc8d") )
        ( 51718,  uint256("0x768d3fd2b3bcd11087108eda200bdd3c7f18d6887029d6878cbd51a4404b85ce") )
        ( 55000,  uint256("0xa65a6b6b0666952d9beff5c25f058006b552620a0dbaedda25683e08f8d7930d") )
        ( 57878,  uint256("0xd2e0da2b391044aa0501b8c23030667efeda7b3c0f2cd0db00caa5015fa9e14b") )
        ( 76118,  uint256("0x2e78d418fcf4ea971987142de0c15778f4c436aad228eab5e96be03c1b1176af") )
        ( 90000,  uint256("0x4961f45686a719fa79d82e90d4bb9cf8013efb6eb7aa53a431de3fc808c53f4a") )
        ( 116094, uint256("0xd6c3c39b77b1355d302edfa96388a6c32f46f412f3829879c609ae4f99d55b9e") )
        ( 159202, uint256("0x5b19e9f462f5349623843eb2fc43480c9dec7ec3fff3d90244e7210bc14db4a1") )
        ( 191892, uint256("0x3c921318d38293b820072669e4f29fee20857d110af961778b541ae98154953a") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
