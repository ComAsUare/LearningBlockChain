#include "simpleChain.hpp"

#include <iostream>
#include <limits>
#include <string>

#include "checkZeroDigits.hpp"
#include "hash.hpp"

TxData::TxData() : locktime(0) {}

TxData::TxData(std::string hash) : prevTxHash(hash), locktime(0) {}

std::string TxData::serialize() const {
    std::string res;
    if (!prevTxHash.empty())
        res += prevTxHash;
    for (auto t : tx)
        res += t;
    res += locktime;
    return res;
}

void TxData::addTx(const std::string& transaction) { tx.push_back(transaction); }

void TxData::set_prevtx(std::string hash) { prevTxHash = hash; }

Block::Block() : nonce(0) {}

Block::Block(std::vector<TxData> input) : txData(input), nonce(0) {}

void Block::addTxData(TxData td) {
    if (!txData.empty())
        td.set_prevtx(EVP_Hash(txData[txData.size() - 1].serialize(), HashName::SHA256));
    txData.push_back(td);
}

void Block::setNonce(uint32_t n) { nonce = n; }

void Block::setPreBlockHash(std::string hash) { preBlockHash = hash; }

int Block::getTxDataNum() { return txData.size(); }

std::string const Block::getHash() {
    std::string ser;
    if (!preBlockHash.empty())
        ser += preBlockHash;
    for (auto td : txData)
        ser += td.serialize();
    ser += nonce;
    return EVP_Hash(ser, HashName::SHA256);
}

void Mining(Block& block, int zeroDigits) {
    for (uint32_t i = 0; i < std::numeric_limits<uint32_t>::max(); i++) {
        block.setNonce(i);
        std::string hash = block.getHash();
        if (checkZeroDigits(hash, zeroDigits))
            break;
    }
}

SimpleBlockchain::SimpleBlockchain() {}

unsigned long long SimpleBlockchain::getBlockNum() { return chain.size(); }

void SimpleBlockchain::addBlock(Block bk) {
    if (!chain.empty())
        bk.setPreBlockHash(chain[chain.size() - 1].getHash());
    Mining(bk, PowDigits);
    chain.push_back(bk);

    std::cout << "block num:" << getBlockNum() << std::endl;
    std::cout << "tx num:" << chain[chain.size() - 1].getTxDataNum() << std::endl;
    std::cout << "block hash:" << chain[chain.size() - 1].getHash() << std::endl;
}
