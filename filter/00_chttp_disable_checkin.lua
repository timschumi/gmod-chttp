hook.Add("OnCHTTPRequest", "00_chttp_disable_checkin", function(req)
    if req.url ~= "https://chttp.timschumi.net/checkin" then
        return
    end

    -- Note that this will never actually get printed by default.
    -- The checkin request does not have a fail handler.
    return "CHTTP checkin request blocked by hook"
end)
