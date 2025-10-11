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
                        fail("HTTP request failed: " .. err)
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
                        fail("HTTP request failed: " .. err)
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
                        fail("HTTP request failed: " .. err)
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
                        fail("HTTP request failed: " .. err)
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
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Redirect followed",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "http://127.0.0.1:5000/response_redirect",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("Redirected!")
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Redirect not followed",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    url = "http://127.0.0.1:5000/response_redirect",
                    noredirect = true,
                    success = function(code, body, headers)
                        expect(code).to.equal(307)
                        expect(body).to.equal("Redirecting...")
                        expect(headers["Location"]).to.equal("http://127.0.0.1:5000/response_redirect_landing")
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Content-Type for GET request",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    method = "GET",
                    url = "http://127.0.0.1:5000/echo_content_type",
                    success = function(code, body, headers)
                        expect(code).to.equal(404)
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Content-Type for POST request",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    method = "POST",
                    url = "http://127.0.0.1:5000/echo_content_type",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("application/x-www-form-urlencoded")
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Content-Type for POST request with body",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    method = "POST",
                    url = "http://127.0.0.1:5000/echo_content_type",
                    body = "Hello world!",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("text/plain; charset=utf-8")
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Content-Type for POST request with header override",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    method = "POST",
                    url = "http://127.0.0.1:5000/echo_content_type",
                    headers = {
                        ["Content-Type"] = "application/octet-stream",
                    },
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("application/octet-stream")
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "Content-Type for POST request with body and header override",
            async = true,
            timeout = 1,
            func = function()
                CHTTP({
                    method = "POST",
                    url = "http://127.0.0.1:5000/echo_content_type",
                    headers = {
                        ["Content-Type"] = "application/octet-stream",
                    },
                    body = "Hello world!",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("application/octet-stream")
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end
        },
        {
            name = "OnCHTTPRequest is called with request data",
            async = true,
            timeout = 1,
            func = function()
                hook.Add("OnCHTTPRequest", "testsuite", function(req)
                   expect(req.url).to.equal("http://127.0.0.1:5000")
                end)

                CHTTP({
                    url = "http://127.0.0.1:5000",
                    success = function(code, body, headers)
                        done()
                    end,
                    failed = function(err)
                        fail("HTTP request failed: " .. err)
                    end
                })
            end,
            cleanup = function()
                hook.Remove("OnCHTTPRequest", "testsuite")
            end,
        },
        {
            name = "OnCHTTPRequest can reject launched requests",
            async = true,
            timeout = 1,
            func = function()
                hook.Add("OnCHTTPRequest", "testsuite", function(req)
                   return "computer says no"
                end)

                CHTTP({
                    url = "http://127.0.0.1:5000",
                    success = function(code, body, headers)
                        fail("HTTP request succeeded")
                    end,
                    failed = function(err)
                        expect(err).to.equal("computer says no")
                        done()
                    end
                })
            end,
            cleanup = function()
                hook.Remove("OnCHTTPRequest", "testsuite")
            end,
        },
        {
            name = "OnCHTTPRequest rejection reason defaults to non-empty string",
            async = true,
            timeout = 1,
            func = function()
                hook.Add("OnCHTTPRequest", "testsuite", function(req)
                   return ""
                end)

                CHTTP({
                    url = "http://127.0.0.1:5000",
                    success = function(code, body, headers)
                        fail("HTTP request succeeded")
                    end,
                    failed = function(err)
                        expect(string.len(err) > 0).to.beTrue()
                        done()
                    end
                })
            end,
            cleanup = function()
                hook.Remove("OnCHTTPRequest", "testsuite")
            end,
        },
    }
}
