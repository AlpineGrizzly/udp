'''
gen_keypair.py 

Python script for generating dynamic key pair pem encoded files to be used for RSA encryption

Logic for using cryptography library used from here: https://dev.to/aaronktberry/generating-encrypted-key-pairs-in-python-69b

Author Dalton Kinney
Created Feb 4th, 2024
'''
import sys
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import serialization

def main():
    # Declaration of private and public pem file names 
    priv_fn = "private_key.pem"
    pub_fn = "public_key.pem"
    keysize = 0 # Size of keys to be generated

    # Take in key size argument from user
    if len(sys.argv) == 2:
        try: 
            keysize = int(sys.argv[1])
        except:
            print("Must provide integer key value!")
            exit(0)        
    else:
        print("Invalid arguments!")
        exit(0)

    # Verify keysize
    if not 512 < keysize < 4096:
        print("Invalid Keysize (512 < keysize < 4096)")
        exit(0)

    print(f"Generating keys of size {keysize}")
    # Generate the private key  
    private_key = rsa.generate_private_key(
        public_exponent=65537, # widely used e value
        key_size=keysize
    )

    # PEM encode our private key
    pem_private_key = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.NoEncryption()
    )

    # Generate / PEM encode the public key
    pem_public_key = private_key.public_key().public_bytes(
      encoding=serialization.Encoding.PEM,
      format=serialization.PublicFormat.SubjectPublicKeyInfo
    ) 

    # Write out to PEM files
    private_key_file = open(priv_fn, "w")
    private_key_file.write(pem_private_key.decode())
    private_key_file.close()

    public_key_file = open(pub_fn, "w")
    public_key_file.write(pem_public_key.decode())
    public_key_file.close()

    print(f"Keys written to {priv_fn} and {pub_fn}")

if __name__ == "__main__":
    main()