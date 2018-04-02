# Installation

Install a server with php, for example on raspberry pi Jessie:

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git build-essential apache2 php5 gnuplot cmake
```

# Clone

Git clone this repository to where you will host the website:

```
cd ~
git clone git@gitlab.com:Tama/guard.git
```

# Build GuardDog

Follow [GuardDog's README.](https://github.com/TamaHobbit/GuardDog/blob/master/GuardDog/README.md)

# Install the service that starts GuardDog on boot

```
sudo cp service /etc/init.d/guarddog
sudo update-rc.d guarddog defaults
```

# Configure access

Edit /etc/apache2/apache2.conf:

```
search for "<Directory /var/www/>", change that to "<Directory /home/pi/guard/>"
change AllowOverride to "All" instead of "None", so that the .htaccess and .htpasswd files are used
```

Edit /etc/apache2/sites-available/000-default.conf, also to change the Directory to /home/pi/guard/.
