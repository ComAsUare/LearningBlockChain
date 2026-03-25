#include "EVP_hash_support.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <ios>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// Bitcoin Transaction Input
struct TxInput {
  std::string prev_tx_hash; // 32 bytes, previous transaction hash
  uint32_t output_index;    // 4 bytes, index of the output in previous tx
  std::string script_sig;   // variable, unlocking script
  uint32_t sequence;        // 4 bytes, sequence number (usually 0xFFFFFFFF)

  TxInput(const std::string &hash, uint32_t idx, const std::string &sig,
          uint32_t seq = 0xFFFFFFFF)
      : prev_tx_hash(hash), output_index(idx), script_sig(sig), sequence(seq) {}
};

// Bitcoin Transaction Output
struct TxOutput {
  uint64_t value;            // 8 bytes, amount in satoshis
  std::string script_pubkey; // variable, locking script

  TxOutput(uint64_t val, const std::string &script)
      : value(val), script_pubkey(script) {}
};

// Bitcoin Transaction
class TransactionBTC {
private:
  // no witness data for simplicity
  uint32_t version;              // 4 bytes, transaction version
  std::vector<TxInput> inputs;   // variable, list of inputs
  std::vector<TxOutput> outputs; // variable, list of outputs
  uint32_t locktime;             // 4 bytes, lock time

public:
  TransactionBTC(uint32_t ver = 1, uint32_t lock = 0)
      : version(ver), locktime(lock) {}

  void addInput(const TxInput &input) { inputs.push_back(input); }

  void addOutput(const TxOutput &output) { outputs.push_back(output); }

  // Serialize transaction to raw bytes format
  std::string serialize() const {
    std::string result;

    // Version (4 bytes, little-endian)
    for (int i = 0; i < 4; i++) {
      result += static_cast<char>((version >> (i * 8)) & 0xFF);
    }

    // Input count (varint)
    result += static_cast<char>(inputs.size());

    // Inputs
    for (const auto &input : inputs) {
      result += input.prev_tx_hash;
      for (int i = 0; i < 4; i++) {
        result += static_cast<char>((input.output_index >> (i * 8)) & 0xFF);
      }
      result += static_cast<char>(input.script_sig.size());
      result += input.script_sig;
      for (int i = 0; i < 4; i++) {
        result += static_cast<char>((input.sequence >> (i * 8)) & 0xFF);
      }
    }

    // Output count (varint)
    result += static_cast<char>(outputs.size());

    // Outputs
    for (const auto &output : outputs) {
      for (int i = 0; i < 8; i++) {
        result += static_cast<char>((output.value >> (i * 8)) & 0xFF);
      }
      result += static_cast<char>(output.script_pubkey.size());
      result += output.script_pubkey;
    }

    // Locktime (4 bytes, little-endian)
    for (int i = 0; i < 4; i++) {
      result += static_cast<char>((locktime >> (i * 8)) & 0xFF);
    }

    return result;
  }

  // Get transaction hash (double SHA256)
  std::string getTxHash() const {
    std::string serialized = serialize();
    std::string hash1 = EVP_Hash(serialized, HashName::SHA256);
    std::string hash2 = EVP_Hash(hash1, HashName::SHA256);
    return hash2;
  }

  // Display transaction info
  void display() const {
    std::cout << "=== Bitcoin Transaction ===" << std::endl;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Inputs: " << inputs.size() << std::endl;
    for (size_t i = 0; i < inputs.size(); i++) {
      std::cout << "  Input " << i << ":" << std::endl;
      std::cout << "    Prev TX: " << inputs[i].prev_tx_hash << std::endl;
      std::cout << "    Output Index: " << inputs[i].output_index << std::endl;
      std::cout << "    ScriptSig: " << inputs[i].script_sig << std::endl;
      std::cout << "    Sequence: " << inputs[i].sequence << std::endl;
    }
    std::cout << "Outputs: " << outputs.size() << std::endl;
    for (size_t i = 0; i < outputs.size(); i++) {
      std::cout << "  Output " << i << ":" << std::endl;
      std::cout << "    Value: " << outputs[i].value << " satoshis"
                << std::endl;
      std::cout << "    ScriptPubKey: " << outputs[i].script_pubkey
                << std::endl;
    }
    std::cout << "Locktime: " << locktime << std::endl;
    std::cout << "TX Hash: " << getTxHash() << std::endl;
  }
};

std::string generate_random_bytes(int digits) {
  std::random_device rd;        // 硬件随机数生成器（熵源）
  std::mt19937 generator(rd()); // 以硬件随机数为种子的梅森旋转算法
  std::uniform_int_distribution<uint32_t> distribution(
      0, 255); // 生成0-255之间的随机数
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (int i = 0; i < digits; i++) {
    uint32_t random_byte = distribution(generator);
    ss << std::setw(2) << random_byte; // 每个字节占两位十六进制
  }
  return ss.str();
}

class MerkleTree {
private:
  int txNum; // number of trasaction hash data
  std::vector<std::string> txHashData;
  void buildMerkleTree(std::vector<std::string> inputTxData);

public:
  MerkleTree() : txNum(0) {}
  MerkleTree(std::initializer_list<std::string> il_input) {
    std::vector<std::string> input = il_input;
    buildMerkleTree(input);
  }
  std::string getRootHash() {
    if (txNum == 0)
      return "";
    return txHashData[0];
  }
  std::string printMerkleTree() {
    std::string res;
    for (int i = 0; i < txHashData.size(); i++) {
      res += "Node " + std::to_string(i) + ": " + txHashData[i] + "\n";
    }
    return res;
  }

  std::vector<int> getProof(int proofIndex) {
    std::vector<int> res;
    int logBase2 = std::floor(std::log2(txNum));
    int arrayIndex = proofIndex + std::pow(2, logBase2 + 1) - 2;
    for (int layer = logBase2 + 2; layer > 1; layer--) {
        //debug
        //std::cout << "layer: " << layer << ", array index: " << arrayIndex << std::endl;
      if (arrayIndex % 2 == 0)
        res.push_back(arrayIndex - 1);
      else
        res.push_back(arrayIndex + 1);
      arrayIndex = (arrayIndex - 1) / 2;
    }
    return res;
  }

  bool VerifyTxHash(std::string txHash, int proofIndex) { 
    std::vector<int> proofPath = getProof(proofIndex);
    //debug
    //std::cout << "txHash: " << txHash << std::endl;
    for(int index:proofPath){
        //debug
        //std::cout << "proof index: " << index << std::endl;
      if(index%2==0)
        txHash = EVP_Hash(txHash+txHashData[index], HashName::SHA256);
      else
        txHash = EVP_Hash(txHashData[index]+txHash, HashName::SHA256);
        //debug
        //std::cout << "txHash: " << txHash << std::endl;
    }
    if(txHash!=getRootHash())
      return false;
    return true; 
}
};

void MerkleTree::buildMerkleTree(std::vector<std::string> inputTxData) {
  txNum = inputTxData.size();
  int logBase2 = std::floor(std::log2(txNum));
  // no filling nodes in bottom layer of merkle tree
  int nodeNum =
      std::pow(2, logBase2 + 1) - 1 + txNum; // merkle tree's total node num

  std::vector<std::string> res(nodeNum);
  // fill nodes in last layer
  for (int i = 0; i < txNum; i++) {
    res[nodeNum - txNum + i] = inputTxData[i];
  }
  std::string lastNode = inputTxData[txNum - 1];

  // fill nodes from second bottom layer to root
  for (int layer = logBase2 + 1; layer > 0; layer--) {
    for (int i = std::pow(2, layer - 1) - 1; i < std::pow(2, layer) - 1; i++) {
      // debug
      // std::cout << "layer: " << layer << ", node: " << i << std::endl;
      int l = 2 * i + 1;
      int r = 2 * i + 2;
      std::string left, right;
      if (layer == logBase2 + 1 && l > nodeNum - 1)
        left = lastNode;
      else
        left = res[l];
      if (layer == logBase2 + 1 && r > nodeNum - 1)
        right = lastNode;
      else
        right = res[r];
      res[i] = EVP_Hash((left + right), HashName::SHA256);
    }
  }
  txHashData = res;
  return;
}

TransactionBTC createSampleTransaction(int txInNum, std::vector<int> txOut) {
  TransactionBTC tx;
  std::random_device rd;        // 硬件随机数生成器（熵源）
  std::mt19937 generator(rd()); // 以硬件随机数为种子的梅森旋转算法
  std::uniform_int_distribution<uint32_t> distribution(
      0, 800); // 生成0-255之间的随机数
  for (int i = 0; i < txInNum; i++) {
    tx.addInput(
        TxInput(generate_random_bytes(32), distribution(generator), "OP_SIG"));
  }
  for (int txOutNum : txOut) {
    tx.addOutput(
        TxOutput(distribution(generator) * 100000000,
                 "OP_DUP OP_HASH160 <pubkeyhash> OP_EQUALVERIFY OP_CHECKSIG"));
  }
  return tx;
}

int main() {
  // Create a sample Bitcoin transaction
  TransactionBTC tx1 = createSampleTransaction(3, {20000, 50000});
  TransactionBTC tx2 = createSampleTransaction(2, {10000, 30000});
  TransactionBTC tx3 = createSampleTransaction(4, {150000, 20000, 50000});
  // Display transaction hash
  std::cout << "Transaction 1 Hash: " << tx1.getTxHash() << std::endl;
  std::cout << "Transaction 2 Hash: " << tx2.getTxHash() << std::endl;
  std::cout << "Transaction 3 Hash: " << tx3.getTxHash() << std::endl;

  // Build a Merkle tree with the transaction hashes
  MerkleTree merkleTree{tx1.getTxHash(), tx2.getTxHash(), tx3.getTxHash()};
  std::cout << "Merkle Root Hash: " << merkleTree.getRootHash() << std::endl;
  std::cout << "Merkle Tree: " << merkleTree.printMerkleTree() << std::endl;

  //verify the first transaction hash with its proof
  std::cout << "Verify Transaction 1: " << (merkleTree.VerifyTxHash(tx1.getTxHash(), 1) ? "Success" : "Failure") << std::endl;
  return 0;
}