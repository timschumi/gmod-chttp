hook.Add("OnCHTTPRequest", "00_chttp_disable_update_notification", function(req)
    if req.url ~= "https://chttp.timschumi.net/checkin" then
        return
    end

    req.success = nil
end)
