return {
    groupName = "CHTTP",
    cases = {
        {
            name = "CHTTP loads successfully",
            func = function()
                expect(require, "chttp").to.succeed()
                expect(CHTTP).to.exist()

                -- Replace HTTP with CHTTP for tests/http.lua
                HTTP = CHTTP
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
    }
}
