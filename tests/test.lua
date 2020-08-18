-- Load utility functions
include("testlib.lua")

require("chttp")

chttp_test_assertEquals("CHTTP() should return nothing if no request table is given.", 0, select('#', CHTTP()))

chttp_test_assertEquals("CHTTP({}) should return a value.", 1, select('#', CHTTP({})))
chttp_test_assertTrue("CHTTP({}) should return true.", CHTTP({}))