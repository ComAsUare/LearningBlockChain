##pip install pycryptodome

from Crypto.Hash import keccak
import binascii

text1 = b"Stan is a programmer" #input must be in bytes
text2 = b"Stan is a Programmer"

keccak_hash1 = keccak.new(digest_bits=256)
keccak_hash1.update(text1)
htx1 = keccak_hash1.hexdigest()

keccak_hash2 = keccak.new(digest_bits=256)
keccak_hash2.update(text2)
htx2 = keccak_hash2.hexdigest()


print("Hash of text1:", htx1)
print("Hash of text2:", htx2)
