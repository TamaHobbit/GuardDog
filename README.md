# Installation

Install a server with php, for example on raspberry pi Jessie:

```
sudo apt-get install apache2 php5
```

# Clone

Git clone this repository to where you will host the website:

```
cd ~
git clone git@gitlab.com:Tama/guard.git
git submodule update --init
```

# Build GuardDog

See the GuardDog's README.

```
cd guard/GuardDog
mkdir build
cd build
cmake ..
make
```

# Install the service that starts GuardDog on boot

```
sudo cp service /etc/init.d/guarddog
sudo update-rc.d NameOfYourScript defaults
```

# Configure access

Edit /etc/apache2/apache2.conf:

```
search for "<Directory /var/www/>", change that to "<Directory /home/pi/guard/>"
change AllowOverride to "All" instead of "None", so that the .htaccess and .htpasswd files are used
```

