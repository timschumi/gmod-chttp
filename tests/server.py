#!/usr/bin/env python3

import flask

app = flask.Flask(__name__)


@app.route("/", methods=["GET"])
def response_normal():
    return b"Hello World!", 200


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


if __name__ == "__main__":
    app.run(debug=False, host="0.0.0.0")
