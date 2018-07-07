# Telementry
Telementry software to receive data from a vehicle

# Dependencies
* Python 3.5 or later
    * [https://www.python.org/](https://www.python.org/)
* Grafana
    * [https://grafana.com/](https://grafana.com/)
* Grafana Track Map plugin
	* [https://github.com/beaver71/grafana-track-map](https://github.com/beaver71/grafana-track-map)
* PostgreSQL
    * [https://www.postgresql.org/](https://www.postgresql.org/)

# Usage
Supports python 3.5 or later

`python serialReader.py <COM PORT number> <Database connection string>`

Example:

`python serialReader.py 7 "dbname='test' user='postgres' host='localhost' password='admin'"`