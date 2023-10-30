# Encrypt Telemati output file for OTA version update

#To generate private and key.bin file a telematic file:

	openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:2048
	openssl rsa -pubout -in private_key.pem -out public_key.pem
	openssl rand -base64 32 > key.bin

#To Encrypt a telematic file:

	openssl enc -aes-256-cbc -salt -in telematic   -out telematic.enc -pass file:./key.bin

#To generate key.bin.enc for decrypt a telematic file 

	openssl rsautl -encrypt -inkey public_key.pem -pubin -in key.bin -out key.bin.enc 

#To delete key.bin 

	shred -u key.bin

#To Decrypt a telematic file:#

	openssl rsautl -decrypt -inkey private_key.pem -in key.bin.enc -out key.bin  
	openssl enc -d -aes-256-cbc -in telematic.enc   -out telematic_dec -pass file:./key.bin

#NOTE: The generated private_key.pem & key.bin.enc  file should be added to the encryption directory to decrypt file i target devices
