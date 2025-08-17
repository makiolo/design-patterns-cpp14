FROM nimmis/apache-php5

MAINTAINER Ricardo Marmolejo García <makiolo@gmail.com>

RUN echo "upload_max_filesize=800M" >> /etc/php5/apache2/php.ini
RUN echo "post_max_size=800M" >> /etc/php5/apache2/php.ini
RUN echo "max_input_time=300" >> /etc/php5/apache2/php.ini
RUN echo "max_execution_time=300" >> /etc/php5/apache2/php.ini
RUN echo "error_reporting = E_ALL" >> /etc/php5/apache2/php.ini
RUN echo "display_errors = On" >> /etc/php5/apache2/php.ini

WORKDIR /var/www/html/cpp
RUN mkdir -p /var/www/html/packages
RUN chmod -R 777 /var/www/html/packages

