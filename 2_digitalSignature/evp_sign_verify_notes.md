# EVP Sign/Verify Example Notes / EVP 签名验签示例说明

## 1. Overview / 概述

This project contains `evp_sign_verify.cpp`, a unified OpenSSL EVP high-level API example for signing and verifying messages with either RSA or ECDSA keys.

这个项目中的 `evp_sign_verify.cpp` 是一个基于 OpenSSL EVP 高层接口的统一示例，可使用 RSA 或 ECDSA 密钥对消息进行签名与验签。

The program:

该程序可以：

- load a PEM private key / 加载 PEM 私钥
- load a PEM public key / 加载 PEM 公钥
- sign a message with SHA-256 / 使用 SHA-256 对消息签名
- verify the generated signature / 验证生成的签名
- print the key type and signature in hex / 输出密钥类型和十六进制签名

---

## 2. Prerequisite: Install OpenSSL / 前置条件：安装 OpenSSL

You need OpenSSL headers and libraries before compiling this example.

在编译这个示例之前，需要先安装 OpenSSL 的头文件和库。

### macOS (Homebrew)

```bash
brew install openssl
```

Common Homebrew path on Apple Silicon:

Apple Silicon 上常见的 Homebrew 路径：

```text
/opt/homebrew/opt/openssl
```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install libssl-dev
```

### Check installation / 检查安装

```bash
openssl --version


---

## 3. Build Command / 编译命令

### macOS with Homebrew OpenSSL

```bash
clang++ -std=c++17 evp_sign_verify.cpp \
  -I/opt/homebrew/opt/openssl/include \
  -L/opt/homebrew/opt/openssl/lib \
  -lssl -lcrypto -o evp_sign_verify
```

### If OpenSSL is already in the default search path / 如果 OpenSSL 已在默认搜索路径中

```bash
clang++ -std=c++17 evp_sign_verify.cpp -lssl -lcrypto -o evp_sign_verify
```

---

## 4. Key Functions / 关键函数说明

### `load_private_key`

Code location: `evp_sign_verify.cpp:10`

Purpose:

作用：

- open a PEM private key file / 打开 PEM 私钥文件
- parse the key into `EVP_PKEY*` / 将私钥解析为 `EVP_PKEY*`

Important API:

关键 API：

- `fopen(...)`
- `PEM_read_PrivateKey(...)`

---

### `load_public_key`

Code location: `evp_sign_verify.cpp:26`

Purpose:

作用：

- open a PEM public key file / 打开 PEM 公钥文件
- parse the key into `EVP_PKEY*` / 将公钥解析为 `EVP_PKEY*`

Important API:

关键 API：

- `fopen(...)`
- `PEM_read_PUBKEY(...)`

---

### `key_type_name`

Code location: `evp_sign_verify.cpp:42`

Purpose:

作用：

- detect the key type / 检测密钥类型
- print readable text such as `RSA` or `ECDSA/EC` / 输出可读文本，如 `RSA` 或 `ECDSA/EC`

Important API:

关键 API：

- `EVP_PKEY_base_id(key)`

---

### `sign_message`

Code location: `evp_sign_verify.cpp:53`

Purpose:

作用：

- sign the input message using the private key / 使用私钥对输入消息签名
- use SHA-256 as the message digest / 使用 SHA-256 作为摘要算法

Signing steps:

签名流程：

1. create digest context / 创建摘要上下文
2. initialize signing / 初始化签名
3. feed the message / 输入消息
4. query signature length / 查询签名长度
5. generate signature bytes / 生成签名字节

Important API:

关键 API：

- `EVP_MD_CTX_new()`
- `EVP_DigestSignInit(...)`
- `EVP_DigestSignUpdate(...)`
- `EVP_DigestSignFinal(...)`
- `EVP_MD_CTX_free(...)`

---

### `verify_signature`

Code location: `evp_sign_verify.cpp:88`

Purpose:

作用：

- verify the signature using the public key / 使用公钥验证签名
- return `true` on success and `false` on signature mismatch / 验签成功返回 `true`，签名不匹配返回 `false`

Verification steps:

验签流程：

1. create digest context / 创建摘要上下文
2. initialize verification / 初始化验签
3. feed the original message / 输入原始消息
4. verify the signature bytes / 验证签名字节

Important API:

关键 API：

- `EVP_MD_CTX_new()`
- `EVP_DigestVerifyInit(...)`
- `EVP_DigestVerifyUpdate(...)`
- `EVP_DigestVerifyFinal(...)`
- `EVP_MD_CTX_free(...)`

---

### `to_hex`

Code location: `evp_sign_verify.cpp:124`

Purpose:

作用：

- convert raw signature bytes to a readable hex string / 将原始签名字节转换为可读的十六进制字符串

Useful for debugging and display.

便于调试和显示。

---

### `main`

Code location: `evp_sign_verify.cpp:133`

Purpose:

作用：

- parse command-line arguments / 解析命令行参数
- load keys / 加载密钥
- check key type match / 检查密钥类型是否匹配
- sign and verify the message / 对消息签名并验签
- print the result / 输出结果

---

## 5. Key Variables / 关键变量说明

### `EVP_PKEY* private_key`

- private key object / 私钥对象
- used for signing / 用于签名

### `EVP_PKEY* public_key`

- public key object / 公钥对象
- used for verification / 用于验签

### `const EVP_MD* digest`

- digest algorithm descriptor / 摘要算法描述对象
- currently passed as `EVP_sha256()` / 当前传入的是 `EVP_sha256()`

Meaning:

含义：

- controls how the message is hashed before signing or verifying / 控制消息在签名和验签前使用哪种哈希算法

### `EVP_MD_CTX* context`

- digest/signature operation context / 摘要或签名操作上下文
- created for each sign or verify operation / 每次签名或验签都会创建一个上下文

### `std::vector<unsigned char> signature`

- stores the raw signature bytes / 存储原始签名字节
- later converted to hex for printing / 后续可转换为十六进制输出

### `size_t signature_length`

- stores the required or actual signature length / 存储签名长度
- first used to query the size, then used after final signing / 先查询长度，再保存最终签名长度

### `std::string message`

- the original input message / 原始输入消息
- same message must be used in both sign and verify / 签名和验签时必须使用同一份消息

---

## 6. Test Steps / 测试步骤

### Step 1: Generate RSA test keys / 第一步：生成 RSA 测试密钥

Generate RSA private key:

生成 RSA 私钥：

```bash
openssl genpkey -algorithm RSA -out rsa_private.pem -pkeyopt rsa_keygen_bits:2048
```

Export RSA public key:

导出 RSA 公钥：

```bash
openssl pkey -in rsa_private.pem -pubout -out rsa_public.pem
```

Run test:

运行测试：

```bash
./evp_sign_verify rsa_private.pem rsa_public.pem "hello rsa"
```

Expected output example:

期望输出示例：

```text
key type:  RSA
message:   hello rsa
signature: <hex string>
verify:    success
```

---

### Step 2: Generate ECDSA test keys / 第二步：生成 ECDSA 测试密钥

Generate ECDSA private key using curve `prime256v1`:

使用 `prime256v1` 曲线生成 ECDSA 私钥：

```bash
openssl genpkey -algorithm EC -out ecdsa_private.pem -pkeyopt ec_paramgen_curve:prime256v1
```

Export ECDSA public key:

导出 ECDSA 公钥：

```bash
openssl pkey -in ecdsa_private.pem -pubout -out ecdsa_public.pem
```

Run test:

运行测试：

```bash
./evp_sign_verify ecdsa_private.pem ecdsa_public.pem "hello ecdsa"
```

Expected output example:

期望输出示例：

```text
key type:  ECDSA/EC
message:   hello ecdsa
signature: <hex string>
verify:    success
```

---

### Step 3: Negative test / 第三步：反向测试

You can verify that tampering breaks verification.

可以验证消息被篡改后会导致验签失败。

For example, if you later modify the program to verify a different message, verification should fail.

例如，如果后续把验签时的消息改成不同内容，验签应失败。

You can also test mismatched key types.

也可以测试密钥类型不匹配的情况。

Example:

示例：

- RSA private key + ECDSA public key / RSA 私钥 + ECDSA 公钥
- the program should throw `private key and public key types do not match` / 程序应报错 `private key and public key types do not match`

---

## 7. Notes / 补充说明

### Why use EVP? / 为什么使用 EVP？

EVP is OpenSSL's high-level unified cryptographic interface.

EVP 是 OpenSSL 的高层统一密码学接口。

Benefits:

优点：

- same code structure for RSA and ECDSA / RSA 和 ECDSA 共用同一套代码结构
- easier to switch digest algorithms / 更容易切换摘要算法
- closer to modern OpenSSL usage / 更符合现代 OpenSSL 用法
- easier to extend to file signing / 更容易扩展到文件签名

### Current digest algorithm / 当前摘要算法

The example currently uses:

当前示例使用：

```cpp
EVP_sha256()
```

If needed, it can be changed to `EVP_sha512()` or another digest.

如果需要，也可以改成 `EVP_sha512()` 或其他摘要算法。

### Signature format / 签名格式

- RSA signature length is usually related to key size / RSA 签名长度通常与密钥位数相关
- ECDSA signature is usually DER encoded / ECDSA 签名通常是 DER 编码

---

