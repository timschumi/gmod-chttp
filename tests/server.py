#!/usr/bin/env python3

import flask

app = flask.Flask(__name__)


@app.route("/", methods=["GET"])
def response_normal():
    return b"Hello World!", 200


@app.route("/echo_content_type", methods=["GET", "POST"])
def echo_content_type():
    if "Content-Type" not in flask.request.headers:
        return b"", 404

    return flask.request.headers["Content-Type"], 200


@app.route("/response_null_byte_in_body", methods=["GET"])
def response_null_byte_in_body():
    return b"Hello World\0!", 200


@app.route("/response_multiple_cookies", methods=["GET"])
def response_multiple_cookies():
    return (
        b"Hello World!",
        200,
        {
            "Set-Cookie": [
                "CookieA=1",
                "CookieB=2",
            ],
        },
    )


@app.route("/response_multiple_cookies_with_expires", methods=["GET"])
def response_multiple_cookies_with_expires():
    return (
        b"Hello World!",
        200,
        {
            "Set-Cookie": [
                "CookieA=1; Expires=Sat, 02 Feb 2002 12:17:00 GMT",
                "CookieB=2; Expires=Fri, 21 Jul 2023 13:36:35 GMT",
            ],
        },
    )


@app.route("/response_multiple_warning", methods=["GET"])
def response_multiple_warning():
    return (
        b"Hello World!",
        200,
        {
            "Warning": [
                "199 - Warning1",
                "199 - Warning2",
            ],
        },
    )

@app.route("/response_redirect", methods=["GET"])
def response_redirect():
    return (
        b"Redirecting...",
        307,
        {
            "Location": "/response_redirect_landing",
        },
    )

@app.route("/response_redirect_landing", methods=["GET"])
def response_redirect_landing():
    return b"Redirected!", 200

if __name__ == "__main__":
    app.run(debug=False, host="0.0.0.0")
