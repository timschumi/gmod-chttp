hook.Add("OnCHTTPRequest", "00_chttp_disable_telemetry", function(req)
    if req.url ~= "https://chttp.timschumi.net/checkin" then
        return
    end

    req.parameters = req.parameters or {}
    req.parameters["build_target"] = nil
    req.parameters["build_type"] = nil
    req.parameters["build_static"] = nil
    req.parameters["os_sysname"] = nil
    req.parameters["os_release"] = nil
    req.parameters["os_version"] = nil
    req.parameters["os_machine"] = nil
    req.parameters["dist_name"] = nil
    req.parameters["dist_version"] = nil
end)
