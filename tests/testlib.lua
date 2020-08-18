function chttp_test_info(msg)
    MsgC(Color(61, 245, 229), msg)
end

function chttp_test_success(msg)
    MsgC(Color(69, 255, 122), msg)
end

function chttp_test_failed(msg)
    MsgC(Color(255, 84, 84), msg)
end

function chttp_test_assertEquals(message, expected, actual)
    chttp_test_info("Test: " .. message .. "\n")

    if expected == actual then
        chttp_test_success("Successful.\n\n")
        return
    end

    chttp_test_failed("expected: " .. expected .. "\n")
    chttp_test_failed("actual: " .. actual .. "\n")
    MsgC("\n")
end

function chttp_test_assertTrue(message, actual)
    chttp_test_info("Test: " .. message .. "\n")

    if actual then
        chttp_test_success("Successful.\n\n")
        return
    end

    chttp_test_failed("Failed.\n\n")
end

function chttp_test_assertFalse(message, actual)
    chttp_test_info("Test: " .. message .. "\n")

    if actual then
        chttp_test_success("Successful.\n\n")
        return
    end

    chttp_test_failed("Failed.\n\n")
end

function chttp_test_dumpResponse(code, body, headers)
    print("Code: " .. code .."\n")

    print("Body:")
    print(body)

    PrintTable(headers)
end