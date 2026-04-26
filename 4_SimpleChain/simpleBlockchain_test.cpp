#include <cassert>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "simpleChain.hpp"

namespace {

std::string captureStdout(const std::function<void()>& action) {
    std::ostringstream buffer;
    std::streambuf* original = std::cout.rdbuf(buffer.rdbuf());

    try {
        action();
    } catch (...) {
        std::cout.rdbuf(original);
        throw;
    }

    std::cout.rdbuf(original);
    return buffer.str();
}

void testNewBlockchainStartsEmpty() {
    SimpleBlockchain blockchain;
    assert(blockchain.getBlockNum() == 0);
}

void testAddBlockIncreasesCountAndPrintsSummary() {
    SimpleBlockchain blockchain;
    Block block;

    const std::string output = captureStdout([&] { blockchain.addBlock(block); });

    assert(blockchain.getBlockNum() == 1);
    assert(output.find("block num:1") != std::string::npos);
    assert(output.find("tx num:0") != std::string::npos);
    assert(output.find("block hash:") != std::string::npos);
}

void testAddMultipleBlocksTracksChainSize() {
    SimpleBlockchain blockchain;
    Block firstBlock;
    Block secondBlock;

    captureStdout([&] { blockchain.addBlock(firstBlock); });
    const std::string output = captureStdout([&] { blockchain.addBlock(secondBlock); });

    assert(blockchain.getBlockNum() ==  2);
    assert(output.find("block num:2") != std::string::npos);
    assert(output.find("tx num:0") != std::string::npos);
}

}  // namespace

int main() {
    testNewBlockchainStartsEmpty();
    testAddBlockIncreasesCountAndPrintsSummary();
    testAddMultipleBlocksTracksChainSize();

    std::cout << "All SimpleBlockchain tests passed." << std::endl;
    return 0;
}
