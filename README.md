# URL Shortener

This is a simple URL shortener that uses a base62 encoding to shorten URLs. It is written in C++23 and uses the Crow web framework. It uses SQLite as the database and Blake3 as a hash function to generate the shortened URLs.

## Building

To build the project, you need to have CMake installed. You can build the project by running the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

## Running

After building the project, you can run the server by running the following command:

```bash
export URL_SHORTENER_DB_PATH=database.sqlite
export URL_SHORTENER_BASE_URL=http://localhost:8080
export URL_SHORTENER_PORT=8080
./build/url_shortener
```

Alternatively, you can use the `run.sh` script to run the server:

```bash
./start.sh
```
or
```bash
./start_tests.sh
```

to run the tests as well.

The server will be running on `http://localhost:8080`. You can change the port by changing the `URL_SHORTENER_PORT` environment variable. The server uses SQLite as the database, and the database file is specified by the `URL_SHORTENER_DB_PATH` environment variable.

## API

The server has the following endpoints:

- `GET /`: This is the home page of the server. It returns a simple HTML page with a form to shorten URLs.

- `POST /shorten`: This endpoint is used to shorten URLs. It expects a JSON object with a `url` field containing the URL to shorten. It returns a JSON object with a `short_url` field containing the shortened URL.

- `GET /{short_url}`: This endpoint is used to redirect to the original URL. It expects a shortened URL as a path parameter and redirects to the original URL if it exists.

## Examples

You can use the following `curl` commands to interact with the server.

To shorten a URL:

```bash
curl -v -X POST -H "Content-Type: application/json" -d '{"url":"https://www.schwaberow.de"}' http://localhost:8080/shorten
```

To redirect to the original URL:

```bash
curl -v http://localhost:8080/{short_url}
```

To shorten a URL and redirect to the original URL:



## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
