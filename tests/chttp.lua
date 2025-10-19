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
            name = "Request methods are checked and translated",
            async = true,
            timeout = 1,
            func = function()
                -- We have to check multiple async requests, so we effectively implement something like a semaphore here.
                -- We initialize to 1 and decrement once after all requests have been queued so we can reliably check at all exits.
                local pending_requests = 1

                local function on_request_done()
                    pending_requests = pending_requests - 1
                    if pending_requests == 0 then
                        done()
                    end
                end

                local function test_method(method_in, method_out)
                    pending_requests = pending_requests + 1

                    local queued = CHTTP({
                        method = method_in,
                        url = "http://127.0.0.1:5000/echo_method",
                        success = function(code, body, headers)
                            expect(code).to.equal(200)
                            expect(body).to.equal(method_out)
                            on_request_done()
                        end,
                        failed = function(err)
                            expect(err).to.equal("invalid method")
                            on_request_done()
                        end,
                    })
                    expect(queued).to.beTrue()
                end

                -- An unset method should default to GET
                test_method(nil, "GET")

                -- Request methods noted as supported in Structures/HTTPRequest
                test_method("get", "GET")
                test_method("GET", "GET")
                test_method("post", "POST")
                test_method("POST", "POST")
                test_method("put", "PUT")
                test_method("PUT", "PUT")
                test_method("delete", "DELETE")
                test_method("DELETE", "DELETE")
                test_method("patch", "PATCH")
                test_method("PATCH", "PATCH")
                test_method("options", "OPTIONS")
                test_method("OPTIONS", "OPTIONS")

                -- HEAD is also one of those, but we don't get back the response body.
                test_method("head", "")
                test_method("HEAD", "")

                -- Other request methods defined by RFC 9110, those are unsupported.
                test_method("trace", nil)
                test_method("TRACE", nil)
                test_method("connect", nil)
                test_method("CONNECT", nil)

                -- All requests scheduled, check just in case the scheduler beat us every time.
                -- That said, the way GLuaTest runs tests should make this impossible.
                on_request_done()
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
        {
            name = "OnCHTTPRequest can modify launched requests",
            async = true,
            timeout = 1,
            func = function()
                hook.Add("OnCHTTPRequest", "testsuite", function(req)
                   req.url = "http://127.0.0.1:5000/response_redirect_landing"
                end)

                CHTTP({
                    url = "http://127.0.0.1:5000",
                    success = function(code, body, headers)
                        expect(code).to.equal(200)
                        expect(body).to.equal("Redirected!")
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
    }
}
