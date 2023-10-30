REQUIRED_PKG="libsqlite3-dev gcc cmake libbluetooth3-dev git build-essential libssl-dev"
for i in $REQUIRED_PKG;do 
	PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $i)
	echo Checkng for $i:$PKG_OK
	if [ "" = "$PKG_OK" ]; then
		echo "NO $i,Setting up $i."
		sudo apt-get --yes install $i
	fi
done