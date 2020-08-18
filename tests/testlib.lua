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
    if expected == actual then
        return
    end

    failed(message .. "\n")
    failed("expected: " .. expected .. "\n")
    failed("actual: " .. actual .. "\n")
end

function assertTrue(message, actual)
    if actual then
        return
    end

    failed("Test failed: " .. message .. "\n")
end

function assertFalse(message, actual)
    if actual then
        return
    end

    failed("Test failed: " .. message .. "\n")
end