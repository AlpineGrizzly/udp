# udp
Udp datagram sender and receiver C programs

## Requirements
## Sender

### Usage
```sh
Usage: sender [data filename] [src_ip] [dst_ip] [src_port] [dst_port] [datagram file name]
UDP datagram sender 

-h      Show this information
```

## Receiver 

## Usage
```sh
Usage: receiver [src_ip] [dst_ip] [datagram file name]
UDP datagram receiver 

-h      Show this information
```

## Encryption support with RSA implementation 
By generating a public and private key pair using the RSA python tool, you can encrypt your
udp traffic and decrypt with the sender and receive modules respectively by using the dec and enc scripts
in the modules.

Read me is provided with the RSA module. Steps to make an encrypted communication.
```sh
# Initialize keys + create rsa exectuable to be used for encryption and decryption
(cd rsa &&
python3 gen_keypair.py 4095) &&
make &&
mv public_key.pem ../sender/ && mv private_key.pem ../receiver)

# Build and intialize udp sender and encrypt payload 
(cd sender/ && 
make && 
./testcase1.sh && 
./enc_rsa.sh)

# Build and initialize udp receiver and decrypt payload before receiving
(cd receiver/ && 
make && 
./dec_rsa.sh && 
./testcase1.sh)
```