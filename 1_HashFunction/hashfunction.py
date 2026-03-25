import hashlib

text1 = "Stan is a programmer"
text2 = "Stan is a Programmer"

htx1 = hashlib.sha256(text1.encode()).hexdigest()
htx2 = hashlib.sha256(text2.encode()).hexdigest()

print("Hash of text1:", htx1)
print("Hash of text2:", htx2)

num1 = 12345
num2 = 12344

#hn1 = hashlib.sha256(str(num1).encode()).hexdigest()
#hn2 = hashlib.sha256(str(num2).encode()).hexdigest()
hn1 = hashlib.sha3_512(str(num1).encode()).hexdigest()
hn2 = hashlib.sha3_256(str(num2).encode()).hexdigest()

print("Hash of num1:", hn1)
print("Hash of num2:", hn2)

text3 = "first MD5 algrithm"
text4 = "first MD5 Algrithm"

mhtx3 = hashlib.md5(text3.encode()).hexdigest()
mhtx4 = hashlib.md5(text4.encode()).hexdigest()

print("Hash of text3:", mhtx3)
print("Hash of text4:", mhtx4)  

tx1 = "e2ef06757e87ff128c34975da52fe855f4e58c7ab4679cffc36b71fdc82f8c7c"
tx2 = "777606dcc45b01de7c77908a02c2685071edcd0e9e2eb1352eccca997857af3a"
tx3 = "cdd5106f317b7ef39efbb2e7c16fe9cc3871ac6665b7c878e225228a587220a7"

tx12 = hashlib.sha256((tx1 + tx2).encode()).hexdigest()
tx23 = hashlib.sha256((tx3 + tx3).encode()).hexdigest()
txMerkleRoot = hashlib.sha256((tx12 + tx23).encode()).hexdigest()
print("Hash of tx12:", tx12)
print("Hash of tx23:", tx23)
print("Merkle Root:", txMerkleRoot)