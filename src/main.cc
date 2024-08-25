#include "url_shortener.hh"
#include "config.hh"
#include <crow.h>
#include <iostream>
#include <fmt/format.h>

int main()
{
    try
    {
        Config config;
        URLShortener url_shortener(config);

        crow::SimpleApp app;

        CROW_ROUTE(app, "/shorten")
            .methods("POST"_method)([&url_shortener, &config](const crow::request &req)
                                    {
            auto json = crow::json::load(req.body);
            if (!json || !json.has("url")) {
                return crow::response(400, "Invalid JSON or missing 'url' field");
            }

            std::string long_url = json["url"].s();
            std::string short_code = url_shortener.shorten(long_url);
            
            crow::json::wvalue response_json({
                {"short_code", short_code},
                {"short_url", fmt::format("{}/{}", config.base_url(), short_code)}
            });
            
            return crow::response(201, response_json); });

        CROW_ROUTE(app, "/<string>")
        ([&url_shortener](const crow::request &, crow::response &res, std::string short_code)
         {
            std::optional<std::string> long_url = url_shortener.expand(short_code);
            if (long_url) {
                res.code = 301;
                res.add_header("Location", *long_url);
                res.end();
            } else {
                res.code = 404;
                res.write("Short URL not found");
                res.end();
            } });

        app.port(config.port()).multithreaded().run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}