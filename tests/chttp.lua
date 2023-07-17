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
