function info(msg)
    MsgC(Color(61, 245, 229), msg)
end

function success(msg)
    MsgC(Color(69, 255, 122), msg)
end

function failed(msg)
    MsgC(Color(255, 84, 84), msg)
end

function assertEquals(message, expected, actual)
    info("Test: " .. message .. "\n")

    if expected == actual then
        success("Successful.\n\n")
        return
    end

    failed("expected: " .. expected .. "\n")
    failed("actual: " .. actual .. "\n")
    MsgC("\n")
end

function assertTrue(message, actual)
    info("Test: " .. message .. "\n")

    if actual then
        success("Successful.\n\n")
        return
    end

    failed("Failed.\n\n")
end

function assertFalse(message, actual)
    info("Test: " .. message .. "\n")

    if actual then
        success("Successful.\n\n")
        return
    end

    failed("Failed.\n\n")
end

function dumpResponse(code, body, headers)
    print("Code: " .. code .."\n")

    print("Body:")
    print(body)

    PrintTable(headers)
end