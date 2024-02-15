#!/bin/bash

# Clean up if there was a previous generation
rm public_key.pem private_key.pem

python3 gen_keypair.py 2048
