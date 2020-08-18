-- Load utility functions
include("testlib.lua")

require("chttp")

assertEquals("CHTTP() should return nothing if no request table is given.", 0, select('#', CHTTP()))

assertEquals("CHTTP({}) should return a value.", 1, select('#', CHTTP({})))
assertTrue("CHTTP({}) should return true.", CHTTP({}))