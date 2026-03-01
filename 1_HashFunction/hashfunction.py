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