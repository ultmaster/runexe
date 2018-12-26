# runexe

Modified from the `runexe` written by the famous Mirzayanov from Saratov State U.

The motivation is to write a cross-platform runner for Hypercube, but as far as
I'm concerned, there isn't a package for that, until I noticed runexe. 
They have provided an exe in the polygon package; also a legendary version (old and
unmaintained) of runexe is on the Github. So I want to modify this, so that the API 
will eventually match the Windows version, at least for those features I'm going to use.

Here is what I have done:

* Interaction
* Wrap the old `invocationResult` in `invocationResults`.
* `SUCCESS` to `SUCCEEDED`.
* A total refactorization of prettification of the project code.
* Use `rlimit` to limit the resource.
* Add `G` as a possible unit for memory.
* Remove other features that I'm not gonna use and also are confusing me, such as, 
  login switch, environment setup, idleness check (on by default) and etc.

No security features (restrictions on system calls) are added, as it was not intended
to build a sandbox for some online judge. However, this could be a good start, if you
are also seeking some solution in untrusted-code-running-with-restrictions.

## Command line help 

```
RunExe for *NIX, Version 1.0

This program runs other program(s) for given period of time with specified
restrictions.

Command line format:
  runexe [<global options>] [<process properties>] program [<parameters>]

Global options:
  -help         - show help
  -xml          - print result in xml format (otherwise, use human-readable)
  -interactor="<process properties> interactor <parameters>"
                  INTERACTOR MODE
    Launch another process and cross-connect its stdin&stdout with the main
    program. Inside this flag, you can specify any process-controlling flags:
    interactor can have its own limits, credentials, environment, directory.
    In interactor mode, however, -i and -o have no effects on both main
    program and interactor.
  -ri=<f>       - in interactor mode, record program input to file <f>.
  -ro=<f>       - in interactor mode, record program output to file <f>.

Process properties:
  -t <value>    - time limit. Terminate after <value> seconds, you can use
                  suffix ms to switch to milliseconds. Suffix "s" (seconds)
                  can be omitted.
  -m <value>    - memory limit. Terminate if anonymous virtual memory of the
                  process exceeds <value> bytes. Use suffixes K, M, G to
                  specify kilo, mega, gigabytes.
  -d <value>    - current directory for the process.
  -i <filename> - redirect standard input to <filename>.
  -o <filename> - redirect standard output to <filename>.
  -e <filename> - redirect standard error to <filename>.
```

## Reference

This part is taken from the Windows version and it's only for my reference.


```
runexe 2.0 version c5a5c20c3bc925a2bbb96cab39d7c21c871d2960 build 138

This program runs other program(s) for given period of time with specified
restrictions.

Command line format:
  runexe [<global options>] [<process properties>] program [<parameters>]

Global options:
  -help         - show help
  -xml          - print result in xml format (otherwise, use human-readable)
  -show-kernel-mode-time - include kernel-mode time in human-readable format
                  (always included in xml)
  -x            - return process exit code (not implemented)
  -logfile=<f>  - for runexe developers only
  -interactor="<process properties> interactor <parameters>"
                  INTERACTOR MODE
    Launch another process and cross-connect its stdin&stdout with the main
    program. Inside this flag, you can specify any process-controlling flags:
    interactor can have its own limits, credentials, environment, directory.
    In interactor mode, however, -i and -o have no effects on both main
    program and interactor.
  -ri=<f>       - in interactor mode, record program input to file <f>.
  -ro=<f>       - in interactor mode, record program output to file <f>.

Process properties:
  -t <value>    - time limit. Terminate after <value> seconds, you can use
                  suffix ms to switch to milliseconds. Suffix "s" (seconds)
                  can be omitted.
  -m <value>    - memory limit. Terminate if anonymous virtual memory of the
                  process exceeds <value> bytes. Use suffixes K, M, G to
                  specify kilo, mega, gigabytes.
  -D k=v        - environment. If any is specified, existing environment is
                  cleared.
  -d <value>    - current directory for the process.
  -l <value>    - login name. Create process under <value> user.
  -p <value>    - password for user specified in -l. On linux, ignored (but
                  must be present).
  -j <filename> - inject <filename> DLL into process.
  -i <filename> - redirect standard input to <filename>.
  -o <filename> - redirect standard output to <filename>.
  -e <filename> - redirect standard error to <filename>.
  -z            - run process in trusted mode.
  -no-idleness-check - switch off idleness checking.
```

And here is an example of verbose output and xml output.

```
Program successfully terminated
  exit code:    0
  time consumed: 0.02 sec
  time passed:  0.17 sec
  peak memory:  6963200 bytes

Interactor successfully terminated
  exit code:    0
  time consumed: 0.00 sec
  time passed:  0.70 sec
  peak memory:  27480064 bytes
```

```
<?xml version="1.1" encoding="UTF-8"?>
<invocationResults>
<invocationResult id="program">
<invocationVerdict>SUCCEEDED</invocationVerdict>
<exitCode>0</exitCode>
<processorUserModeTime>15</processorUserModeTime>
<processorKernelModeTime>62</processorKernelModeTime>
<passedTime>176</passedTime>
<consumedMemory>6909952</consumedMemory>
</invocationResult>
<invocationResult id="interactor">
<invocationVerdict>SUCCEEDED</invocationVerdict>
<exitCode>0</exitCode>
<processorUserModeTime>0</processorUserModeTime>
<processorKernelModeTime>31</processorKernelModeTime>
<passedTime>695</passedTime>
<consumedMemory>27402240</consumedMemory>
</invocationResult>
</invocationResults>
```