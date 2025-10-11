# gmod-chttp

This project aims to provide an open-source drop-in replacement for GMod's
built-in HTTP module. It provides the same interface as the original to
allow for replacing it easily.

The module *should* provide all the functions that the original has
(in terms of request methods and arguments).
However, it doesn't yet behave like the original in all edge cases,
especially when it comes to deviating from the spec (for example just
omitting the request body on DELETE requests).

If you like my work or otherwise profited from this module, consider
[buying me a beer](https://paypal.me/schumact)!

## Installation

The only thing required to install the library is to download the
appropriate file from the [releases page](https://github.com/timschumi/gmod-chttp/releases) and
putting it into the `<game or server directory>/garrysmod/lua/bin/` directory.
(The `bin` directory might not exist and should be created manually.)

Attention has to be paid to whether you are using the 32-bit or 64-bit
version of the game/server. **Servers and clients are both 32-bit by default,
unless manually changed by joining the appropriate beta.**

There are two other variations of the libraries, `-dbg` (with debugging symbols
included) and `-static` (includes even less dynamically linked dependencies),
as well as a combination of both.

Unless you are bug-hunting (in the case of `-dbg`) or are having issues with
loading the library due to dependencies (in the case of `-static`), you probably
shouldn't use either of those (except when instructed to do so).

Please note that when using the library variants that have a suffix (`-dbg`, `-static`, ...),
that suffix has to be removed to match the expected library name.
**The game will not find the library unless its exact name is `gmsv_chttp_<arch>.dll`,
where `<arch>` is one of the following: `linux`, `linux64`, `win32`, `win64`.**

## Configuration

### CA bundle

**Warning:** This section only applies to the Linux library. The Windows variant
uses the native SSL backend inside Windows (also known as "Schannel" or "WinSSL")
and as a result relies on the built-in certificate store.

Due to differences across various Linux distributions CHTTP will search for an
accessible CA bundle in the following locations (in this order) when the first
request is made:

* `/etc/ssl/certs/ca-certificates.crt` (Debian)
* `/etc/pki/tls/certs/ca-bundle.crt` (Red Hat and Mandriva)
* `/usr/share/ssl/certs/ca-bundle.crt` (Older Red Hat)
* `/usr/local/share/certs/ca-root-nss.crt` (FreeBSD)
* `/etc/ssl/cert.pem` (OpenBSD, FreeBSD)

Most paths in this list should also apply to their respective derivatives, for example
Ubuntu as a Debian-derivative and Fedora/CentOS being based on Red Hat Enterprise Linux.

In case you are still having issues related to the CA bundle (for example receiving the
error `Problem with the SSL CA cert (path? access rights?)`) you can override the chosen
path through the environment variable `CHTTP_CAINFO`.

As long as this environment variable is set, CHTTP will not search for the CA bundle
itself. Instead, whatever value is provided will be
[passed to libcurl as-is](https://curl.se/libcurl/c/CURLOPT_CAINFO.html) and no
other checks will be made by CHTTP to verify that the provided value is valid.

### Source Interface

**Warning:** This should not be your go-to solution for fixing routing issues. Manually
setting the source interface can introduce weird issues if not done correctly, and the
operating system is usually better at figuring out where traffic should go anyways.
Go fix your routes.

Usually, CHTTP will route your requests through the interface that is chosen by your
operating system. In case the wrong interface is chosen and you can't fix your routing
table so that traffic is sent through the correct interface by default, you can
force CHTTP to pick a specific interface instead through the use of the
environment variable `CHTTP_INTERFACE`.

Setting this will override the interface using the variable contents for each request.
The value is passed to libcurl as-is and is not checked for validity by CHTTP,
so make sure to review the documentation for
[libcurl's CURLOPT_INTERFACE option](https://curl.se/libcurl/c/CURLOPT_INTERFACE.html).

### Zero-delay timers

CHTTP has switched to using a zero-delay timer instead of a hook to allow for running
requests while in singleplayer or while the server is hibernating.

In case there are any issues (bug reports greatly appreciated), you can return to
using the old hooking method by setting the environment variable `CHTTP_FORCE_HOOK`.

### Update notifications and telemetry

CHTTP automatically checks for new updates and transmits light telemetry data
when the module is loaded. This behavior can be disabled by setting the
`CHTTP_DISABLE_UPDATE_NOTIFICATION` and `CHTTP_DISABLE_TELEMETRY` environment
variables respectively.
This behavior can additionally be disabled by utilizing the `OnCHTTPRequest` hook,
ready-to-use examples for this can be found in the [`filter`](filter/) directory.

Unless both features are disabled (in which case no request is sent at all),
the current module version is always transmitted (for the purpose of checking
if any newer versions are available).

If telemetry isn't disabled, the following additional properties are sent as well:

* `build_target`: `win32`, `win64`, `linux` or `linux64`
* `build_type`: `Release` or `Debug`
* `build_static` `0` or `1` (depending on whether the `-static` file is in use)

If telemetry isn't disabled and the host OS is Linux, the following additional
properties are sent:

* `os_sysname`: `Linux`
* `os_release`: Linux kernel version (e.g. `6.5.7-arch1-1` on Arch Linux)
* `os_version`: Linux kernel build string (e.g. `#1 SMP PREEMPT_DYNAMIC Tue, 10 Oct 2023 21:10:21 +0000`)
* `os_machine`: Processor architecture (e.g. `x86_64`)
* `dist_name`: Distribution name (e.g. `ubuntu`)
* `dist_version`: Distribution version (if present; e.g. `22.04`)

I'm collecting and aggregating this data for my own curiosity, as well as getting
an informed guess on how fast updates propagate and what systems the module is run on.
If you have any feedback regarding either feature, feel free to leave it [here](https://github.com/timschumi/gmod-chttp/discussions/29).

## Addon development

This is only required for developers who want to use CHTTP in their addons.
A normal end user doesn't need any of this.

### Loading the module

The module is loaded by running `require("chttp")` in the LUA console or in
a script. It will provide a `CHTTP()` method that is called in the same way
as the original HTTP-Method. For more information, please refer to the
[Garry's Mod documentation](https://wiki.facepunch.com/gmod/Global.HTTP).

### Automatic fallback to HTTP

If you want to use CHTTP when it's available and fall back to the built-in
HTTP implementation otherwise you can use the following Lua snippet to load
the library:

```
if pcall(require, "chttp") and CHTTP ~= nil then
	my_http = CHTTP
else
	my_http = HTTP
end
```

You will now have a function called `my_http` that behaves like `HTTP()`/`CHTTP()`
but automatically points to the correct implementation.

### Additional log output

CHTTP can provide additional information about what is going on internally, which
at the moment includes messages when a request or redirect is processed and when
invalid values in parameter or header tables are skipped.

To receive those messages in the console the developer log level has to be set to
a non-null value. This can be done by entering `developer 1` as a command, after
which additional messages should start showing up.

### Filter HTTP requests

CHTTP queries the `OnCHTTPRequest` hook with the `HTTPRequest` structure as an argument
and will adjust its behavior depending on the type of the returned value.

The following return types are currently defined:

- Returning a single `string` will intercept the request and use the return value
  as the failure message.

- Returning no value will launch the request as normal.

Other combinations of return value types are reserved for future use.

Editing the passed HTTPRequest structure will affect the request that is sent.

Examples for hook utilization can be found in the [`filter`](filter/) directory.
