# gmod-chttp

This project aims to provide an open-source drop-in replacement for GMod's
built-in HTTP module. It provides the same interface as the original to
allow for replacing it easily.

The module *should* provide all the functions that the original has
(in terms of request methods and arguments).
However, it doesn't yet behave like the original in all edge cases,
especially when it comes to deviating from the spec (for example just
omitting the request body on DELETE requests).

## Installation

The "base directory" is the directory where the main executable lives.
This is usually `hl2.exe`/`hl2_linux` for clients or `srcds.exe`/`srcds_run`
for dedicated servers.

### Windows

1. Download the following files from the [releases page](https://github.com/timschumi/gmod-chttp/releases):
    - `gmsv_chttp_win32.dll`
    - `libcurl.dll`
    - `libssl-1_1.dll`
    - `libcrypto-1_1.dll`

2. Put `libcurl.dll`, `libssl-1_1.dll`, and `libcrypto-1_1.dll` into the Garry's Mod
   base directory.

3. Put `gmsv_chttp_win32.dll` into `<base directory>\garrysmod\lua\bin\`. You may have to
   create the `bin` directory if it doesn't exist.

### Linux

1. Install libcurl and its dependencies. If you can't install libcurl and/or
   the dependencies in a way that they end up in the library search path,
   they need to be copied to the Garry's Mod base directory.

2. Download `gmsv_chttp_linux.dll` (yes, dll) from the [releases page](https://github.com/timschumi/gmod-chttp/releases)
   and put it into `<base directory>/garrysmod/lua/bin/`. You may have to
   create the `bin` directory if it doesn't exist.

## Usage

The module is loaded by running `require("chttp")` in the LUA console or in
a script. It will provide a `CHTTP()` method that is called in the same way
as the original HTTP-Method. For more information, please refer to the
[Garry's Mod documentation](https://wiki.garrysmod.com/page/Global/HTTP).

In the use case where you want to use CHTTP instead of HTTP if it's available,
use the following code snippet to load it:

```
if pcall(require, "chttp") and CHTTP ~= nil then
	HTTP = CHTTP
end
```