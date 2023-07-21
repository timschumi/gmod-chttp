return {
    groupName = "CHTTP",
    cases = {
        {
            name = "CHTTP loads successfully",
            func = function()
                expect(require, "chttp").to.succeed()
                expect(CHTTP).to.exist()
            end
        },
        {
            name = "HTTPS requests run successfully",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "https://example.com",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        done()
                    end,
                    failed = function(err)
                        error("HTTP request failed: " .. err)
                        done()
                    end
                })
            end
        },
        {
            name = "Response has null byte in body",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "http://127.0.0.1:5000/response_null_byte_in_body",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("Hello World\0!")
                        done()
                    end,
                    failed = function(err)
                        error("HTTP request failed: " .. err)
                        done()
                    end
                })
            end
        },
        {
            name = "Response has multiple cookies",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "http://127.0.0.1:5000/response_multiple_cookies",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("Hello World!")

                        -- Header order is nondeterministic
                        expect((headers["Set-Cookie"] == "CookieA=1,CookieB=2") or (headers["Set-Cookie"] == "CookieB=2,CookieA=1")).to.beTrue()
                        done()
                    end,
                    failed = function(err)
                        error("HTTP request failed: " .. err)
                        done()
                    end
                })
            end
        },
        {
            name = "Response has multiple cookies with Expires properties",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "http://127.0.0.1:5000/response_multiple_cookies_with_expires",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("Hello World!")

                        local cookie_a = "CookieA=1; Expires=Sat, 02 Feb 2002 12:17:00 GMT"
                        local cookie_b = "CookieB=2; Expires=Fri, 21 Jul 2023 13:36:35 GMT"

                        -- Header order is nondeterministic
                        expect((headers["Set-Cookie"] == (cookie_a .. "," .. cookie_b)) or (headers["Set-Cookie"] == (cookie_b .. "," .. cookie_a))).to.beTrue()
                        done()
                    end,
                    failed = function(err)
                        error("HTTP request failed: " .. err)
                        done()
                    end
                })
            end
        },
        {
            name = "Response has multiple warnings",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "http://127.0.0.1:5000/response_multiple_warning",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("Hello World!")

                        -- Header order is nondeterministic
                        expect((headers["Warning"] == "199 - Warning1,199 - Warning2") or (headers["Warning"] == "199 - Warning2,199 - Warning1")).to.beTrue()
                        done()
                    end,
                    failed = function(err)
                        error("HTTP request failed: " .. err)
                        done()
                    end
                })
            end
        },
    }
}
