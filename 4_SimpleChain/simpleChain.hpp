#pragma once

#include <cstdint>
#include <string>
#include <vector>

class TxData {
   private:
    std::string prevTxHash;
    std::vector<std::string> tx;
    uint32_t locktime;

   public:
    TxData();
    explicit TxData(std::string hash);

    std::string serialize() const;
    void addTx(const std::string& transaction);
    void set_prevtx(std::string hash);
};

class Block {
   private:
    std::string preBlockHash;
    std::vector<TxData> txData;
    uint32_t nonce;

   public:
    Block();
    explicit Block(std::vector<TxData> input);

    void addTxData(TxData td);
    void setNonce(uint32_t n);
    void setPreBlockHash(std::string hash);
    int getTxDataNum();
    std::string const getHash();
};

void Mining(Block& block, int zeroDigits);

class SimpleBlockchain {
   private:
    std::vector<Block> chain;
    int PowDigits = 3;

   public:
    SimpleBlockchain();
    unsigned long long getBlockNum();
    void addBlock(Block bk);
};
