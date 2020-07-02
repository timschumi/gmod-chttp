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

The only thing required to install the library is to download the
appropriate file from the [releases page](https://github.com/timschumi/gmod-chttp/releases) and
putting it into the `<game or server directory>/garrysmod/lua/bin/` directory.
(The `bin` directory might not exist and should be created manually.)

Attention has to be paid to whether you are using the x86 or x64
version of the game/server. Servers and clients are both x86 by default,
unless manually changed by joining the appropriate beta.

Contrary to earlier releases, installing libcurl and its dependencies
ISN'T required anymore, since those are now included in the library itself.

Also, all Linux libraries are now compiled on Debian 9 for compatibility
reasons, so there is no longer a seperate compatibility version of the
library.

## Usage

This is only required for developers who want to use CHTTP in their scripts.
A normal end user doesn't need any of this.

The module is loaded by running `require("chttp")` in the LUA console or in
a script. It will provide a `CHTTP()` method that is called in the same way
as the original HTTP-Method. For more information, please refer to the
[Garry's Mod documentation](https://wiki.facepunch.com/gmod/Global.HTTP).

In the use case where you want to use CHTTP instead of HTTP if it's available,
use the following code snippet to load it:

```
if pcall(require, "chttp") and CHTTP ~= nil then
	HTTP = CHTTP
end
```
