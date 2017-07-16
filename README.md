# Installation

Install a server with php, for example on raspberry pi Jessie:

```
sudo apt-get install apache2 php5
```

# Clone

Git clone this repository into /var/www/html like so:

```
cd ~
sudo git clone git@gitlab.com:Tama/guard.git html
```

# Configure access

Edit /etc/apache2/apache2.conf such that for the website (/var/www/html):

```
search for "<Directory /var/www/>"
change AllowOverride to "All" instead of "None", so that the .htaccess and .htpasswd files are used
```

# Update the website

```
cd /var/www/html
sudo git pull
```
